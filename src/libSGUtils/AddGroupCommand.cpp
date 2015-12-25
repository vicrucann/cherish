#include "AddGroupCommand.h"

#include <QObject>
#include <QString>

AddGroupCommand::AddGroupCommand(RootScene *scene, Canvas* canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , _scene(scene)
    , _canvas(canvas)
{
    QString command((_canvas->getName()).c_str());
    this->setText(QObject::tr("Add %1") .arg(command));
}

AddGroupCommand::~AddGroupCommand()
{
}

void AddGroupCommand::undo()
{
    _scene->deleteCanvas(_canvas.get());
}

void AddGroupCommand::redo()
{
    debugLogMsg("AddGroupCommand(): redo");
    _scene->addCanvas(_canvas.get());
    debugLogMsg("AddGroupCommand(): complete");
}

