#include "AddStrokeCommand.h"

#include <QObject>

#include "settings.h"

// allocate memory
AddStrokeCommand::AddStrokeCommand(RootScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
    , mScene(scene)
    , mStroke(new Stroke)
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

// change stroke's geometry dynamically
void AddStrokeCommand::appendPoint(const float u, const float v)
{
    if (!mStroke.get()){
        outErrMsg("stroke pointer is NULL");
        return;
    }
    mStroke->appendPoint(u, v);
}

// is the stroke long enough to keep it?
bool AddStrokeCommand::isLengthy() const
{
    return mStroke->isLengthy();
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
