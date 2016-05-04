#include "EditEntityCommand.h"

#include <QObject>

EditCanvasOffsetCommand::EditCanvasOffsetCommand(entity::UserScene *scene, const osg::Vec3f &translate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_translate(translate)
{
    this->setText(QObject::tr("Offset %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

EditCanvasOffsetCommand::~EditCanvasOffsetCommand()
{
}

void EditCanvasOffsetCommand::undo()
{
    m_canvas->translate(osg::Matrix::translate(-m_translate.x(), -m_translate.y(), -m_translate.z()));
    m_scene->updateWidgets();
}

void EditCanvasOffsetCommand::redo()
{
    m_canvas->translate(osg::Matrix::translate(m_translate.x(), m_translate.y(), m_translate.z()));
    m_scene->updateWidgets();
}

/* =====================*/

EditCanvasRotateCommand::EditCanvasRotateCommand(entity::UserScene *scene, const osg::Quat &rotate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_rotate(rotate)
    , m_center(m_canvas->getCenter())
{
    this->setText(QObject::tr("Rotate %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

EditCanvasRotateCommand::~EditCanvasRotateCommand()
{

}

void EditCanvasRotateCommand::undo()
{
    double angle;
    osg::Vec3d axis;
    m_rotate.getRotate(angle, axis);
    m_canvas->rotate(osg::Matrix::rotate(-angle, axis), m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void EditCanvasRotateCommand::redo()
{
    m_canvas->rotate(osg::Matrix::rotate(m_rotate), m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

/* only works for strokes, any photos remain unchanged */
EditStrokesPushCommand::EditStrokesPushCommand(entity::UserScene *scene, const std::vector<entity::Entity2D *> &entities,
                                               entity::Canvas *current, entity::Canvas *target,
                                               const osg::Vec3f &eye,
                                               QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_entities(entities)
    , m_canvasCurrent(current)
    , m_canvasTarget(target)
    , m_eye(eye)
{
    m_canvasCurrent->unselectEntities();
    m_canvasCurrent->updateFrame(m_canvasTarget.get());
    this->setText(QObject::tr("Push set of strokes from %1 to %2")
                  .arg(QString(m_canvasCurrent->getName().c_str()),
                       QString(m_canvasTarget->getName().c_str())));
}

void EditStrokesPushCommand::undo()
{
    this->doPushStrokes(*(m_canvasTarget.get()), *(m_canvasCurrent.get()));
    m_scene->updateWidgets();
}

void EditStrokesPushCommand::redo()
{
    this->doPushStrokes(*(m_canvasCurrent.get()), *(m_canvasTarget.get()));
    m_scene->updateWidgets();
}

void EditStrokesPushCommand::doPushStrokes(entity::Canvas& source, entity::Canvas& target)
{
    osg::Matrix M = source.getTransform()->getMatrix();
    osg::Matrix invM = osg::Matrix::inverse(target.getTransform()->getMatrix());

    const osg::Plane plane = target.getPlane();
    const osg::Vec3f center = target.getCenter();

    for (unsigned int i=0; i<m_entities.size(); ++i){
        entity::Stroke* s = dynamic_cast<entity::Stroke*> (m_entities.at(i));
        if (!s) continue;
        osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(s->getVertexArray());
        for (unsigned int j=0; j<verts->size(); ++j){
            osg::Vec3f p = (*verts)[j];
            osg::Vec3f P = p * M;
            osg::Vec3f dir = P - m_eye;
            double len = plane.dotProductNormal(center-P) / plane.dotProductNormal(dir);
            osg::Vec3f P_ = dir * len + P;
            osg::Vec3f p_ = P_ * invM;
            (*verts)[j] = osg::Vec3f(p_.x(), p_.y(), 0.f);
        }
        target.getGeodeData()->addDrawable(s);
        source.getGeodeData()->removeDrawable(s);

        verts->dirty();
        s->dirtyBound();
    }
    target.updateFrame();
}

EditCanvasDeleteCommand::EditCanvasDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
{
this->setText(QObject::tr("Delete %1")
              .arg(QString(m_canvas->getName().c_str())));
}

void EditCanvasDeleteCommand::undo()
{
    emit m_scene->canvasAdded(m_canvas->getName());
    m_scene->addChild(m_canvas);
    m_scene->setCanvasCurrent(m_canvas);
    /* see if any canvas contains any photos, they will be added to canvas widget */
    if (m_canvas->getGeodeData()){
        for (size_t i=0; i<m_canvas->getGeodeData()->getNumChildren(); ++i){
            entity::Photo* photo = dynamic_cast<entity::Photo*>(m_canvas->getGeodeData()->getChild(i));
            if (!photo) continue;
            emit m_scene->photoAdded(photo->getName(), m_scene->getCanvasIndex(m_canvas.get()));
        }
    }
    m_scene->updateWidgets();
}

void EditCanvasDeleteCommand::redo()
{
    if (m_canvas == m_scene->getCanvasCurrent())
        m_scene->setCanvasCurrent(m_scene->getCanvasPrevious());
    if (m_canvas == m_scene->getCanvasPrevious() ||
            m_scene->getCanvasCurrent() == m_scene->getCanvasPrevious()){
        for (unsigned int i = 0; i < m_scene->getNumChildren(); ++i){
            entity::Canvas* cnvi = dynamic_cast<entity::Canvas*>( m_scene->getChild(i));
            if (cnvi != NULL && cnvi != m_scene->getCanvasCurrent() && cnvi != m_canvas){
                m_scene->setCanvasPrevious(cnvi);
                break;
            }
        }
    }
    // now delete the canvas
    emit m_scene->canvasRemoved(m_scene->getCanvasIndex(m_canvas.get()));
    m_scene->removeChild(m_canvas);
    m_scene->updateWidgets();
}

EditStrokeDeleteCommand::EditStrokeDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, entity::Stroke *stroke, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_stroke(stroke)
{
    this->setText(QObject::tr("Delete stroke from %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditStrokeDeleteCommand::undo()
{
    m_canvas->getGeodeData()->addDrawable(m_stroke.get());
    m_scene->updateWidgets();
}

void EditStrokeDeleteCommand::redo()
{
    m_canvas->getGeodeData()->removeDrawable(m_stroke.get());
    m_scene->updateWidgets();
}

EditPhotoDeleteCommand::EditPhotoDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, entity::Photo *photo, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_photo(photo)
{
this->setText(QObject::tr("Delete photo from %1")
              .arg(QString(m_canvas->getName().c_str())));
}

void EditPhotoDeleteCommand::undo()
{
    m_canvas->getGeodeData()->addDrawable(m_photo.get());
    emit m_scene->photoAdded(m_photo->getName(), m_scene->getCanvasIndex(m_canvas.get()));
    m_scene->updateWidgets();
}

void EditPhotoDeleteCommand::redo()
{
    /* order is important: first remove from widget tree;
     * then remove from scene graph */
    emit m_scene->photoRemoved(m_scene->getCanvasIndex(m_canvas.get()),
                               m_scene->getPhotoIndex(m_photo.get(), m_canvas.get()));
    m_canvas->getGeodeData()->removeDrawable(m_photo.get());
    m_scene->updateWidgets();
}

EditEntitiesMoveCommand::EditEntitiesMoveCommand(entity::UserScene *scene, const std::vector<entity::Entity2D *> &entities, entity::Canvas *canvas, double du, double dv, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_entities(entities)
    , m_canvas(canvas)
    , m_du(du)
    , m_dv(dv)
{
    this->setText(QObject::tr("Move strokes within %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditEntitiesMoveCommand::undo()
{
    m_canvas->moveEntities(m_entities, -m_du, -m_dv);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void EditEntitiesMoveCommand::redo()
{
    m_canvas->moveEntities(m_entities, m_du, m_dv);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

EditEntitiesScaleCommand::EditEntitiesScaleCommand(entity::UserScene *scene, const std::vector<entity::Entity2D *> &entities, entity::Canvas *canvas, double scaleX, double scaleY, osg::Vec3f center, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_entities(entities)
    , m_canvas(canvas)
    , m_scaleX(scaleX)
    , m_scaleY(scaleY)
    , m_center(center)
{
    this->setText(QObject::tr("Scale strokes within %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditEntitiesScaleCommand::undo()
{
    m_canvas->scaleEntities(m_entities, 1/m_scaleX, 1/m_scaleY, m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void EditEntitiesScaleCommand::redo()
{
    m_canvas->scaleEntities(m_entities, m_scaleX, m_scaleY, m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

EditEntitiesRotateCommand::EditEntitiesRotateCommand(entity::UserScene *scene, const std::vector<entity::Entity2D *> &entities, entity::Canvas *canvas, double theta, osg::Vec3f center, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_entities(entities)
    , m_canvas(canvas)
    , m_theta(theta)
    , m_center(center)
{
    this->setText(QObject::tr("Rotate strokes within %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditEntitiesRotateCommand::undo()
{
    m_canvas->rotateEntities(m_entities, -m_theta, m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void EditEntitiesRotateCommand::redo()
{
    m_canvas->rotateEntities(m_entities, m_theta, m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

EditPasteCommand::EditPasteCommand(entity::UserScene *scene, entity::Canvas *target, const std::vector<osg::ref_ptr<entity::Entity2D> > &buffer, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(target)
{
    for (size_t i=0; i<buffer.size(); ++i){
        const entity::Stroke& copy = dynamic_cast<const entity::Stroke&> (*buffer.at(i));
        entity::Stroke* stroke = new entity::Stroke(copy, osg::CopyOp::DEEP_COPY_ALL);
        if (!stroke) continue;
        m_entities.push_back(stroke);
    }

    this->setText(QObject::tr("Paste strokes to canvas %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditPasteCommand::undo()
{
    m_canvas->unselectEntities();
    for (size_t i=0; i<m_entities.size();++i){
        m_canvas->getGeodeData()->removeDrawable(m_entities.at(i));
    }
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void EditPasteCommand::redo()
{
    m_canvas->unselectEntities();
    for (size_t i=0; i<m_entities.size(); ++i){
        entity::Entity2D* entity = m_entities.at(i);
        if (!entity) continue;
        entity->moveDelta(0.2, 0.2);
        m_canvas->getGeodeData()->addDrawable(entity);
        m_canvas->addEntitySelected(entity);
    }
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

EditCutCommand::EditCutCommand(entity::UserScene* scene, entity::Canvas*  canvas,
                               const std::vector<entity::Entity2D *> &selected,
                               std::vector<osg::ref_ptr<entity::Entity2D> > &buffer, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_buffer(buffer)
    , m_selected(selected)
{
    this->setText(QObject::tr("Cutted strokes to buffer from canvas %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditCutCommand::undo()
{
    m_scene->setCanvasCurrent(m_canvas.get());
    for (size_t i=0; i<m_buffer.size(); ++i){
        entity::Entity2D* entity = m_buffer.at(i).get();
        if (!entity) continue;
        m_canvas->getGeodeData()->addDrawable(entity);
        m_canvas->addEntitySelected(entity);
    }
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void EditCutCommand::redo()
{
    for (size_t i=0; i<m_selected.size(); ++i){
        entity::Stroke* stroke = dynamic_cast<entity::Stroke*> ( m_selected.at(i));
        if (!stroke) continue;
        m_buffer.push_back(stroke);
    }
    m_scene->setCanvasCurrent(m_canvas.get());
    for (size_t i=0; i<m_buffer.size(); ++i){
        m_canvas->getGeodeData()->removeDrawable(m_buffer.at(i));
    }
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

EditPhotoPushCommand::EditPhotoPushCommand(entity::UserScene *scene, entity::Canvas *current, entity::Canvas *previous, entity::Photo *photo, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_current(current)
    , m_previous(previous)
    , m_photo(photo)
{
    this->setText(QObject::tr("Move photo %1 from canvas %2 to canvas %3")
                  .arg(QString(m_photo->getName().c_str()) ,
                       QString(m_current->getName().c_str()),
                       QString(m_previous->getName().c_str()) ));
}

void EditPhotoPushCommand::undo()
{
    m_current->getGeodeData()->addDrawable(m_photo.get());
    //m_current->getGeodeData()->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());
    m_previous->getGeodeData()->removeChild(m_photo.get());
    m_current->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void EditPhotoPushCommand::redo()
{
    m_previous->getGeodeData()->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());
    m_previous->getGeodeData()->addDrawable(m_photo.get());
    m_current->getGeodeData()->removeDrawable(m_photo.get());
    m_previous->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}
