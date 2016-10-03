#include "EditEntityCommand.h"

#include <QObject>

fur::EditCanvasOffsetCommand::EditCanvasOffsetCommand(entity::UserScene *scene, const osg::Vec3f &translate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_translate(translate)
{
    this->setText(QObject::tr("Offset %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditCanvasOffsetCommand::undo()
{
    m_canvas->translate(osg::Matrix::translate(-m_translate.x(), -m_translate.y(), -m_translate.z()));
    m_scene->updateWidgets();
}

void fur::EditCanvasOffsetCommand::redo()
{
    m_canvas->translate(osg::Matrix::translate(m_translate.x(), m_translate.y(), m_translate.z()));
    m_scene->updateWidgets();
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditCanvasRotateCommand::EditCanvasRotateCommand(entity::UserScene *scene, const osg::Quat &rotate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_rotate(rotate)
    , m_center(m_canvas->getCenter())
{
    this->setText(QObject::tr("Rotate %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditCanvasRotateCommand::undo()
{
    double angle;
    osg::Vec3d axis;
    m_rotate.getRotate(angle, axis);
    m_canvas->rotate(osg::Matrix::rotate(-angle, axis), m_center);
    m_scene->updateWidgets();
}

void fur::EditCanvasRotateCommand::redo()
{
    m_canvas->rotate(osg::Matrix::rotate(m_rotate), m_center);
    m_scene->updateWidgets();
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/* only works for strokes, any photos remain unchanged */
fur::EditStrokesPushCommand::EditStrokesPushCommand(entity::UserScene *scene, const std::vector<entity::Entity2D *> &entities,
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditStrokesPushCommand::undo()
{
    this->doPushStrokes(*(m_canvasTarget.get()), *(m_canvasCurrent.get()));
}

void fur::EditStrokesPushCommand::redo()
{
    this->doPushStrokes(*(m_canvasCurrent.get()), *(m_canvasTarget.get()));
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

void fur::EditStrokesPushCommand::doPushStrokes(entity::Canvas& source, entity::Canvas& target)
{
    osg::Matrix M = source.getTransform()->getMatrix();
    osg::Matrix invM = osg::Matrix::inverse(target.getTransform()->getMatrix());

    const osg::Plane plane = target.getPlane();
    const osg::Vec3f center = target.getCenter();

    for (unsigned int i=0; i<m_entities.size(); ++i){
        entity::Stroke* stroke = dynamic_cast<entity::Stroke*> (m_entities.at(i));
        if (!stroke) continue;
        osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(stroke->getVertexArray());
        for (unsigned int j=0; j<verts->size(); ++j){
            osg::Vec3f p = (*verts)[j];
            osg::Vec3f P = p * M;
            osg::Vec3f dir = P - m_eye;
            double len = plane.dotProductNormal(center-P) / plane.dotProductNormal(dir);
            osg::Vec3f P_ = dir * len + P;
            osg::Vec3f p_ = P_ * invM;
            (*verts)[j] = osg::Vec3f(p_.x(), p_.y(), 0.f);
        }
        m_scene->addEntity(&target, stroke);
        m_scene->removeEntity(&source, stroke);

        verts->dirty();
        stroke->dirtyBound();
    }
    target.updateFrame();
}

fur::EditCanvasDeleteCommand::EditCanvasDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_bookmarks(scene->getBookmarksModel())
{
this->setText(QObject::tr("Delete %1")
              .arg(QString(m_canvas->getName().c_str())));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditCanvasDeleteCommand::undo()
{
    if (!m_scene->addCanvas(m_canvas.get()))
        qFatal("EditCanvasDelete::undo() - failed adding canvas to scene graph");
}

void fur::EditCanvasDeleteCommand::redo()
{
    if (!m_scene->removeCanvas(m_canvas.get()))
        qFatal("EditCanvasDelete::redo() - failed removing canvas from scene graph");
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditStrokeDeleteCommand::EditStrokeDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, entity::Stroke *stroke, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_stroke(stroke)
{
    this->setText(QObject::tr("Delete stroke from %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditStrokeDeleteCommand::undo()
{
    m_scene->addEntity(m_canvas.get(), m_stroke.get());
}

void fur::EditStrokeDeleteCommand::redo()
{
    m_scene->removeEntity(m_canvas.get(), m_stroke.get());
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditPhotoDeleteCommand::EditPhotoDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, entity::Photo *photo, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_photo(photo)
    , m_bookmarks(scene->getBookmarksModel())
{
this->setText(QObject::tr("Delete photo from %1")
              .arg(QString(m_canvas->getName().c_str())));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditPhotoDeleteCommand::undo()
{
    if (!m_scene->addEntity(m_canvas.get(), m_photo.get()))
        qFatal("EditPhotoDeleteCommand:: undo() failed");
}

void fur::EditPhotoDeleteCommand::redo()
{
    if (!m_scene->removeEntity(m_canvas.get(), m_photo.get()))
        qFatal("EditPhotoDeleteCommand:: redo() failed");
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditEntitiesMoveCommand::EditEntitiesMoveCommand(entity::UserScene *scene, const std::vector<entity::Entity2D *> &entities, entity::Canvas *canvas, double du, double dv, QUndoCommand *parent)
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditEntitiesMoveCommand::undo()
{
    m_canvas->moveEntities(m_entities, -m_du, -m_dv);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void fur::EditEntitiesMoveCommand::redo()
{
    m_canvas->moveEntities(m_entities, m_du, m_dv);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditEntitiesScaleCommand::EditEntitiesScaleCommand(entity::UserScene *scene, const std::vector<entity::Entity2D *> &entities, entity::Canvas *canvas, double scaleX, double scaleY, osg::Vec3f center, QUndoCommand *parent)
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditEntitiesScaleCommand::undo()
{
    m_canvas->scaleEntities(m_entities, 1/m_scaleX, 1/m_scaleY, m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void fur::EditEntitiesScaleCommand::redo()
{
    m_canvas->scaleEntities(m_entities, m_scaleX, m_scaleY, m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditEntitiesRotateCommand::EditEntitiesRotateCommand(entity::UserScene *scene, const std::vector<entity::Entity2D *> &entities, entity::Canvas *canvas, double theta, osg::Vec3f center, QUndoCommand *parent)
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditEntitiesRotateCommand::undo()
{
    m_canvas->rotateEntities(m_entities, -m_theta, m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void fur::EditEntitiesRotateCommand::redo()
{
    m_canvas->rotateEntities(m_entities, m_theta, m_center);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditPasteCommand::EditPasteCommand(entity::UserScene *scene, entity::Canvas *target, const std::vector<osg::ref_ptr<entity::Entity2D> > &buffer, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(target)
{
    for (size_t i=0; i<buffer.size(); ++i){
        const entity::Stroke& copy = dynamic_cast<const entity::Stroke&> (*buffer.at(i));
        entity::Stroke* stroke = new entity::Stroke;
        if (!stroke) continue;
        stroke->copyFrom(&copy);
        Q_ASSERT(stroke->getIsShadered());
        m_entities.push_back(stroke);
    }

    this->setText(QObject::tr("Paste strokes to canvas %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditPasteCommand::undo()
{
    m_canvas->unselectEntities();
    for (size_t i=0; i<m_entities.size();++i){
        entity::Entity2D* entity = m_entities.at(i);
        if (!entity) continue;
        m_scene->removeEntity(m_canvas.get(), entity);
    }
}

void fur::EditPasteCommand::redo()
{
    m_canvas->unselectEntities();
    for (size_t i=0; i<m_entities.size(); ++i){
        entity::Entity2D* entity = m_entities.at(i);
        if (!entity) continue;
        entity->moveDelta(0.2, 0.2);
        m_scene->addEntity(m_canvas.get(), entity);
        m_canvas->addEntitySelected(entity);
        if (entity->getEntityType() == cher::ENTITY_STROKE){
            entity::Stroke* s = dynamic_cast<entity::Stroke*>(entity);
            if (!s) continue;
            s->getProgram()->updateTransform(m_canvas->getTransform());
        }
    }
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditCutCommand::EditCutCommand(entity::UserScene* scene, entity::Canvas*  canvas,
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditCutCommand::undo()
{
    m_scene->setCanvasCurrent(m_canvas.get());
    for (size_t i=0; i<m_buffer.size(); ++i){
        entity::Entity2D* entity = m_buffer.at(i).get();
        if (!entity) continue;
        m_scene->addEntity(m_canvas.get(), entity);
        m_canvas->addEntitySelected(entity);
    }
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void fur::EditCutCommand::redo()
{
    for (size_t i=0; i<m_selected.size(); ++i){
        entity::Stroke* stroke = dynamic_cast<entity::Stroke*> ( m_selected.at(i));
        if (!stroke) continue;
        m_buffer.push_back(stroke);
    }
    m_scene->setCanvasCurrent(m_canvas.get());
    for (size_t i=0; i<m_buffer.size(); ++i){
        m_scene->removeEntity(m_canvas.get(), m_buffer.at(i));
    }
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::EditPhotoPushCommand::EditPhotoPushCommand(entity::UserScene *scene, entity::Canvas *source, entity::Canvas *destination, entity::Photo *photo, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_source(source)
    , m_destination(destination)
    , m_photo(photo)
{
    this->setText(QObject::tr("Move photo %1 from canvas %2 to canvas %3")
                  .arg(QString(m_photo->getName().c_str()) ,
                       QString(m_source->getName().c_str()),
                       QString(m_destination->getName().c_str()) ));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::EditPhotoPushCommand::undo()
{
    m_scene->addEntity(m_source.get(), m_photo.get());
    m_scene->removeEntity(m_destination.get(), m_photo.get());
}

void fur::EditPhotoPushCommand::redo()
{
    m_photo->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());
    m_scene->addEntity(m_destination.get(), m_photo.get());
    m_scene->addEntity(m_source.get(), m_photo.get());
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::UndoCommand::UndoCommand(entity::UserScene *scene, entity::Canvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
{
}

fur::EditEntityDeleteCommand::EditEntityDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, entity::Entity2D *entity)
    : UndoCommand(scene, canvas)
    , m_entity(entity)
{
    this->setText(QObject::tr("Delete entity %1 from %2")
                  .arg(QString(entity->getName().c_str()))
                  .arg(QString(canvas->getName().c_str())));
}

void fur::EditEntityDeleteCommand::undo()
{
    if (!m_scene->addEntity(m_canvas.get(), m_entity.get()))
        qFatal("EditEntityDeleteCommand::undo() failed");
}

void fur::EditEntityDeleteCommand::redo()
{
    if (!m_scene->removeEntity(m_canvas.get(), m_entity.get()))
        qFatal("EditEntityDeleteCommand: redo() failed");
}
