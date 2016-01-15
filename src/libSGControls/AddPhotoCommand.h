#ifndef ADDPHOTOCOMMAND_H
#define ADDPHOTOCOMMAND_H

#include "string"

#include <QUndoCommand>
#include <QObject>

#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Photo.h"

class AddPhotoCommand : public QUndoCommand
{
public:
    AddPhotoCommand(entity::UserScene* scene, const std::string& name, QUndoCommand* parent = 0);
    ~AddPhotoCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Photo> m_photo;
};

#endif // ADDPHOTOCOMMAND_H
