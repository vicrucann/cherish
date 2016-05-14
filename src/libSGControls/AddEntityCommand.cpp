#include "AddEntityCommand.h"

#include <QObject>

AddCanvasCommand::AddCanvasCommand(entity::UserScene* scene, const osg::Matrix& R, const osg::Matrix& T, const std::string& name, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(new entity::Canvas)
{
    m_canvas->initializeSG();
    m_canvas->setName(name);
    m_canvas->setMatrixRotation(R);
    m_canvas->setMatrixTranslation(T);
    this->setText(QObject::tr("Add canvas %1")
                  .arg(QString(name.c_str())));
}

AddCanvasCommand::AddCanvasCommand(entity::UserScene* scene, const entity::Canvas& copy, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(copy.clone())
{
    this->setText(QObject::tr("Clone to canvas %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

AddCanvasCommand::AddCanvasCommand(entity::UserScene *scene, const osg::Vec3f &normal, const osg::Vec3f &center, const std::string &name, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(new entity::Canvas())
{
    m_canvas->initializeSG();
    m_canvas->setName(name);

    osg::Matrix mR = osg::Matrix::identity(); //::rotate(cher::PI*0.5, rotaxis.x(), rotaxis.y(), rotaxis.z());
    mR.makeRotate(osg::Vec3f(0,0,1), normal); // from default global normal to new normal
    m_canvas->setMatrixRotation(mR);

    osg::Matrix mT = osg::Matrix::translate(center);
    m_canvas->setMatrixTranslation(mT);

    this->setText(QObject::tr("Add canvas %1")
                  .arg(QString(name.c_str())));
}

void AddCanvasCommand::undo()
{
    // make sure current/previous rules hold
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
    m_canvas->unselectAll();
    m_scene->removeChild(m_canvas);
    if (m_scene->getCanvasCurrent()) m_scene->getCanvasCurrent()->updateFrame(0);
    m_scene->updateWidgets();
}

void AddCanvasCommand::redo()
{
    emit m_scene->canvasAdded(m_canvas->getName());

    m_scene->addChild(m_canvas);
    m_scene->setCanvasCurrent(m_canvas);
    m_scene->updateWidgets();
}

AddPhotoCommand::AddPhotoCommand(entity::UserScene* scene, const std::string& fname, const std::string &ename, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_photo(new entity::Photo)
{
    m_photo->setName(ename);
    m_photo->loadImage(fname);

    m_photo->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());
//    m_canvas->getGeodeData()->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute() );

    this->setText(QObject::tr("Add photo to %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void AddPhotoCommand::undo()
{
    emit m_scene->photoRemoved(m_scene->getCanvasIndex(m_canvas.get()),
                               m_scene->getPhotoIndex(m_photo.get(), m_canvas.get()));
    if (!m_canvas->getGeodeData()->removeChild(m_photo.get()))
        outErrMsg("Could not remove photo from current canvas");
    m_canvas->removeEntitySelected(m_photo.get());
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void AddPhotoCommand::redo()
{
    if (!m_canvas->getGeodeData()->addDrawable(m_photo.get()))
        outErrMsg("Could not add photo to current canvas");

    emit m_scene->photoAdded(m_photo->getName(), m_scene->getCanvasIndex(m_canvas.get()));
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}


AddStrokeCommand::AddStrokeCommand(entity::UserScene* scene, entity::Stroke* stroke, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_stroke(stroke)
{
    this->setText(QObject::tr("Add new stroke to %1")
                  .arg(QString( m_canvas->getName().c_str() ) ) );
}

AddStrokeCommand::~AddStrokeCommand()
{
}

void AddStrokeCommand::undo()
{
    if (!m_canvas->getGeodeData()->removeDrawable(m_stroke))
        outErrMsg("undo(): problem while removing stroke from a canvas");
    m_canvas->removeEntitySelected(m_stroke);
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

void AddStrokeCommand::redo()
{
    if (!m_canvas->getGeodeData()->addDrawable(m_stroke))
        outErrMsg("redo(): problem while adding stroke to a canvas");
    m_canvas->updateFrame(m_scene->getCanvasPrevious());
    m_scene->updateWidgets();
}

AddCanvasSeparationCommand::AddCanvasSeparationCommand(entity::UserScene *scene, entity::Canvas *source, entity::Canvas *copy, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_source(source)
    , m_target(new entity::Canvas)
    , m_entities(0)
{
    m_target->initializeSG();
    m_target->setName(copy->getName());
    m_target->setMatrixRotation(copy->getMatrixRotation());
    m_target->setMatrixTranslation(copy->getMatrixTranslation());

    this->setText(QObject::tr("Separate to canvas %1") .arg(QString(m_target->getName().c_str())));
    for (size_t i=0; i<copy->getGeodeData()->getNumChildren(); ++i){
        entity::Entity2D* ent = dynamic_cast<entity::Entity2D*>(copy->getGeodeData()->getDrawable(i));
        if (!ent) continue;
        m_entities.push_back(ent);
    }
    if (m_entities.size() != m_target->getGeodeData()->getNumChildren())
        outErrMsg("AddCanvasSeparationCommand: failed to initialize entities");
}

void AddCanvasSeparationCommand::undo()
{
    // make sure current/previous rules hold
    if (m_target == m_scene->getCanvasCurrent())
        m_scene->setCanvasCurrent(m_scene->getCanvasPrevious());
    if (m_target == m_scene->getCanvasPrevious() ||
            m_scene->getCanvasCurrent() == m_scene->getCanvasPrevious()){
        for (unsigned int i = 0; i < m_scene->getNumChildren(); ++i){
            entity::Canvas* cnvi = dynamic_cast<entity::Canvas*>( m_scene->getChild(i));
            if (cnvi != NULL && cnvi != m_scene->getCanvasCurrent() && cnvi != m_target){
                m_scene->setCanvasPrevious(cnvi);
                break;
            }
        }
    }

    // move entities to source, remove from target
    this->moveEntities(m_target.get(), m_source.get());

    // delete target
    emit m_scene->canvasRemoved(m_scene->getCanvasIndex(m_target.get()));
    m_target->unselectAll();
    m_scene->removeChild(m_target);
    if (m_scene->getCanvasCurrent()) m_scene->getCanvasCurrent()->updateFrame(0);
    m_scene->updateWidgets();
}

void AddCanvasSeparationCommand::redo()
{
    emit m_scene->canvasAdded(m_target->getName());

    // add entities to target, remove from source
    this->moveEntities(m_source.get(), m_target.get());

    // add target to scene graph
    m_scene->addChild(m_target);
    m_scene->setCanvasCurrent(m_target);
    m_scene->updateWidgets();
}

void AddCanvasSeparationCommand::moveEntities(entity::Canvas *from, entity::Canvas *to)
{
    if (!from || !to) return;

    for (size_t i=0; i<m_entities.size(); ++i){
        entity::Entity2D* entity = m_entities.at(i);
        if (!entity) continue;
        if (!to->getGeodeData()->addDrawable(entity))
            outErrMsg("AddCanvasSeparationCommand: could not add entity to the target");
        if (!from->getGeodeData()->removeDrawable(entity))
            outErrMsg("AddCanvasSeparationCommand: could not remove entity from the source");
    }
    to->updateFrame(0);
    from->updateFrame(0);
}
