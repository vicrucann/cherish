#include "DeleteStrokeCommand.h"

DeleteStrokeCommand::DeleteStrokeCommand(RootScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
    , mScene(scene)
{

}

void DeleteStrokeCommand::undo()
{

}

void DeleteStrokeCommand::redo()
{

}

