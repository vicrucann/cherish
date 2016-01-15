#include "AddPhotoCommand.h"

AddPhotoCommand::AddPhotoCommand(entity::UserScene* scene, const std::string& name, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_photo(new entity::Photo)
{
    m_photo->loadImage(name);
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
}

void AddPhotoCommand::redo()
{
    if (!m_canvas->getGeodeData()->addDrawable(m_photo.get()))
        outErrMsg("Could not add photo to current canvas");
    m_canvas->getGeodeData()->getOrCreateStateSet()->
            setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());
    m_canvas->updateFrame();
}

