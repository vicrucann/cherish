#ifndef ADDPHOTOCOMMAND_H
#define ADDPHOTOCOMMAND_H

#include "string"

#include <QUndoCommand>
#include <QObject>

#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "rootscene.h"
#include "UserScene.h"
#include "Photo.h"

class AddPhotoCommand : public QUndoCommand
{
public:
    AddPhotoCommand(entity::UserScene* scene, const std::string& name, QUndoCommand* parent = 0);
    ~AddPhotoCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<RootScene> m_root;
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Photo> m_photo;
};

#endif // ADDPHOTOCOMMAND_H
