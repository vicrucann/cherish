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

    osg::Matrix mR = osg::Matrix::identity(); //::rotate(dureu::PI*0.5, rotaxis.x(), rotaxis.y(), rotaxis.z());
    mR.makeRotate(osg::Vec3f(0,0,1), normal); // from default global normal to new normal
    m_canvas->setMatrixRotation(mR);

    osg::Matrix mT = osg::Matrix::translate(center);
    m_canvas->setMatrixTranslation(mT);

    this->setText(QObject::tr("Add canvas %1")
                  .arg(QString(name.c_str())));
}

AddCanvasCommand::~AddCanvasCommand()
{

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

    m_canvas->getGeodeData()->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute() );

    this->setText(QObject::tr("Add photo to %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

AddPhotoCommand::~AddPhotoCommand()
{

}

void AddPhotoCommand::undo()
{
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

    emit m_scene->photoAdded(m_photo->getName());
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
