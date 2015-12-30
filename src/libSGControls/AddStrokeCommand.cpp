#include "AddStrokeCommand.h"

#include <QObject>

#include "settings.h"

// allocate memory
AddStrokeCommand::AddStrokeCommand(RootScene *scene, Stroke *stroke, QUndoCommand *parent)
    : QUndoCommand(parent)
    , mScene(scene)
    , mStroke(stroke)
    , mValid(this->checkPointers())
{
    if (!mValid){
        fatalMsg("AddStrokeCommand ctor: pointer were not initialized correctly");
        return;
    }
    this->setText(QObject::tr("Add new stroke to canvas %1")
                  .arg(QString( mScene->getCanvasCurrent()->getName().c_str() ) ) );
}

AddStrokeCommand::~AddStrokeCommand()
{
}

void AddStrokeCommand::undo()
{
    if (mValid){
        if (!mScene->getCanvasCurrent()->getGeodeData()->removeChild(mStroke))
            outErrMsg("undo(): problem while removing stroke from a canvas");
        // update GLWidgets or set canvas' geometry dirty
    }
}

void AddStrokeCommand::redo()
{
    outLogMsg("AddStrokeCommand::redo()");
    if (mValid){
        if (!mScene->getCanvasCurrent()->getGeodeData()->addDrawable(mStroke))
            outErrMsg("redo(): problem while adding stroke to a canvas");
        // update GLWidgets or set canvas' geometry dirty
    }
}

// to make sure there is no NULL pointers
bool AddStrokeCommand::checkPointers() const
{
    if (!mStroke){
        fatalMsg("AddStrokeCommand(): stroke is NULL");
        return false;
    }
    if (!mScene){
        fatalMsg("AddStrokeCommand(): RootScene is NULL");
        return false;
    }
    if (!mScene->getCanvasCurrent()){
        fatalMsg("AddStrokeCommand(): current canvas is NULL");
        return false;
    }
    if (!mScene->getCanvasCurrent()->getGeodeData()){
        fatalMsg("AddStrokeCommand(): geode data is NULL");
        return false;
    }
    return true;
}
