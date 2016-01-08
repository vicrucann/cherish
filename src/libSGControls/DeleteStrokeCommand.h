#ifndef DELETESTROKECOMMAND_H
#define DELETESTROKECOMMAND_H

#include <QUndoCommand>

#include "rootscene.h"
#include "Stroke.h"

#include <osg/observer_ptr>

class DeleteStrokeCommand : public QUndoCommand
{
public:
    explicit DeleteStrokeCommand(RootScene* scene, QUndoCommand *parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<RootScene> mScene;
    osg::observer_ptr<entity::Stroke> mStroke;

};

#endif // DELETESTROKECOMMAND_H
