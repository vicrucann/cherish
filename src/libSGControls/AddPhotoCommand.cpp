#include "AddPhotoCommand.h"

AddPhotoCommand::AddPhotoCommand(RootScene* scene, const std::string& name, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_root(scene)
    , m_photo(new entity::Photo)
{
    m_photo->loadImage(name);
    this->setText(QObject::tr("Add photo to %1")
                  .arg(QString(m_root->getCanvasCurrent()->getName().c_str())));
}

AddPhotoCommand::~AddPhotoCommand()
{

}

void AddPhotoCommand::undo()
{
    if (!m_root->getCanvasCurrent()->getGeodeData()->removeChild(m_photo.get()))
        outErrMsg("Could not remove photo from current canvas");
}

void AddPhotoCommand::redo()
{
    if (!m_root->getCanvasCurrent()->getGeodeData()->addDrawable(m_photo.get()))
        outErrMsg("Could not add photo to current canvas");
    m_root->getCanvasCurrent()->getGeodeData()->
            getOrCreateStateSet()->
            setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());
    m_root->getCanvasCurrent()->updateFrame();
}

