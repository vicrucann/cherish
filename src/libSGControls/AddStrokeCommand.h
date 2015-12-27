#ifndef ADDSTROKECOMMAND_H
#define ADDSTROKECOMMAND_H

#include <QUndoCommand>

#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "rootscene.h"
#include "canvas.h"
#include "Stroke.h"

class AddStrokeCommand : public QUndoCommand
{
public:
    AddStrokeCommand(RootScene* scene, Stroke* stroke);
    ~AddStrokeCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    bool checkPointers() const;

    osg::observer_ptr<RootScene> mScene;
    osg::ref_ptr<Stroke> mStroke;
    bool mValid;
};

#endif // ADDSTROKECOMMAND_H
