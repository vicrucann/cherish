#ifndef ADDGROUPCOMMAND_H
#define ADDGROUPCOMMAND_H

#include <QUndoCommand>

#include <osg/Group>
#include <osg/observer_ptr>

#include "rootscene.h"
#include "canvas.h"

class AddGroupCommand : public QUndoCommand
{
public:
    AddGroupCommand(RootScene* scene, Canvas* canvas, QUndoCommand* parent = 0);
    ~AddGroupCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<RootScene> _scene;
    osg::observer_ptr<Canvas> _canvas;
};

#endif // ADDGROUPCOMMAND_H
