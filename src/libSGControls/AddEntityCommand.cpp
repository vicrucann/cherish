#include "AddEntityCommand.h"

#include <QObject>
#include <QDebug>

fur::AddCanvasCommand::AddCanvasCommand(entity::UserScene* scene, const osg::Matrix& R, const osg::Matrix& T, const std::string& name, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(new entity::Canvas)
    , m_bookmarks(scene->getBookmarksModel())
{
    m_canvas->initializeSG();
    m_canvas->setName(name);
    m_canvas->setMatrixRotation(R);
    m_canvas->setMatrixTranslation(T);
    this->setText(QObject::tr("Add canvas %1")
                  .arg(QString(name.c_str())));
}

fur::AddCanvasCommand::AddCanvasCommand(entity::UserScene* scene, const entity::Canvas& copy, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(copy.clone())
    , m_bookmarks(scene->getBookmarksModel())
{
    this->setText(QObject::tr("Clone to canvas %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

fur::AddCanvasCommand::AddCanvasCommand(entity::UserScene *scene, const osg::Vec3f &normal, const osg::Vec3f &center, const std::string &name, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(new entity::Canvas())
    , m_bookmarks(scene->getBookmarksModel())
{
    m_canvas->initializeSG();
    m_canvas->setName(name);

    osg::Matrix mR = osg::Matrix::identity(); //::rotate(cher::PI*0.5, rotaxis.x(), rotaxis.y(), rotaxis.z());
    mR.makeRotate(cher::NORMAL, normal); // from default global normal to new normal
    m_canvas->setMatrixRotation(mR);

    osg::Matrix mT = osg::Matrix::translate(center);
    m_canvas->setMatrixTranslation(mT);

    this->setText(QObject::tr("Add canvas %1")
                  .arg(QString(name.c_str())));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::AddCanvasCommand::undo()
{
    if (!m_scene->removeCanvas(m_canvas.get()))
        qFatal("AddCanvasCommand::undo() - could not remove from scene graph");
}

void fur::AddCanvasCommand::redo()
{
    if (!m_scene->addCanvas(m_canvas))
        qFatal("AddCanvasCommand::redo() - could not add canvas");
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::AddPhotoCommand::AddPhotoCommand(entity::UserScene* scene, const std::string& fname, const std::string &ename, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_photo(new entity::Photo)
    , m_bookmarks(scene->getBookmarksModel())
{
    m_photo->setName(ename);
    m_photo->loadImage(fname);

    m_photo->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());

    this->setText(QObject::tr("Add photo to %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::AddPhotoCommand::undo()
{
    if (!m_scene->removeEntity(m_canvas.get(), m_photo.get()))
        qFatal("AddPhotoCommand::undo() failed");
}

void fur::AddPhotoCommand::redo()
{
    if (!m_scene->addEntity(m_canvas.get(), m_photo.get()))
        qFatal("AddPhotoCommand::redo() failed");
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::AddStrokeCommand::AddStrokeCommand(entity::UserScene* scene, entity::Stroke* stroke, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_stroke(stroke)
{
    this->setText(QObject::tr("Add new stroke to %1")
                  .arg(QString( m_canvas->getName().c_str() ) ) );
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::AddStrokeCommand::undo()
{
    if (!m_scene->removeEntity(m_canvas.get(), m_stroke.get()))
        qCritical("undo(): problem while removing stroke from a canvas");
}

void fur::AddStrokeCommand::redo()
{
    if (!m_scene->addEntity(m_canvas.get(), m_stroke.get()))
        qCritical("redo(): problem while adding stroke to a canvas");
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

fur::AddCanvasSeparationCommand::AddCanvasSeparationCommand(entity::UserScene *scene, entity::Canvas *source, entity::Canvas *copy, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_source(source)
    , m_target(new entity::Canvas)
    , m_entities(0)
    , m_bookmarks(scene->getBookmarksModel())
{
    m_target->initializeSG();
    m_target->setName(copy->getName());
    m_target->setMatrixRotation(copy->getMatrixRotation());
    m_target->setMatrixTranslation(copy->getMatrixTranslation());

    this->setText(QObject::tr("Separate to canvas %1") .arg(QString(m_target->getName().c_str())));
    for (size_t i=0; i<copy->getNumEntities(); ++i){
        entity::Entity2D* ent = copy->getEntity(i);
        if (!ent) continue;
        m_entities.push_back(ent);
    }
    if (m_entities.size() != m_target->getEntitiesSelectedSize())
        qCritical("fur::AddCanvasSeparationCommand: failed to initialize entities");
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void fur::AddCanvasSeparationCommand::undo()
{
    // move entities to source, remove from target
    this->moveEntities(m_target.get(), m_source.get());

    if (!m_scene->removeCanvas(m_target.get()))
        qFatal("AddCanvasSeparationCommand::undo() failed");
}

void fur::AddCanvasSeparationCommand::redo()
{
    // add entities to target, remove from source
    this->moveEntities(m_source.get(), m_target.get());

    if (!m_scene->addCanvas(m_target.get())) qFatal("AddCanvasSeparationCommand::redo() failed");
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

void fur::AddCanvasSeparationCommand::moveEntities(entity::Canvas *from, entity::Canvas *to)
{
    if (!from || !to) return;

    for (size_t i=0; i<m_entities.size(); ++i){
        entity::Entity2D* entity = m_entities.at(i);
        if (!entity) continue;
        if (!m_scene->addEntity(to, entity))
            qCritical("AddCanvasSeparationCommand: could not add entity to the target");
        if (!m_scene->removeEntity(from, entity))
            qCritical("AddCanvasSeparationCommand: could not remove entity from the source");
        if (entity->getEntityType() == cher::ENTITY_STROKE){
            entity::Stroke* s = dynamic_cast<entity::Stroke*>(entity);
            if (s){
                s->redefineToShader(to->getTransform()); // to update shader's transform callback
            }
        }
    }
    to->updateFrame(0);
    from->updateFrame(0);
}
