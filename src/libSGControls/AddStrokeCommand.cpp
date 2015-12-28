#include "AddStrokeCommand.h"

#include <QObject>

// allocate memory
// assign geometry and color data through deep copy operator
AddStrokeCommand::AddStrokeCommand(RootScene *scene)
    : mScene(scene)
    , mStroke(new Stroke)
    , mValid(this->checkPointers())
{
    // update scene first
    this->setText(QObject::tr("Add new stroke to canvas %1")
                  .arg(QString( mScene->getCanvasCurrent()->getName().c_str() ) ) );
}

AddStrokeCommand::~AddStrokeCommand()
{
}

void AddStrokeCommand::appendPoint(const float u, const float v)
{
    mStroke->appendPoint(u, v);
}

void AddStrokeCommand::undo()
{
    if (mValid){
        if (!mScene->getCanvasCurrent()->getGeodeData()->removeChild(mStroke))
            warningMsg("undo(): problem while removing stroke from a canvas");
        // update GLWidgets or set canvas' geometry dirty
    }
}

void AddStrokeCommand::redo()
{
    if (mValid){
        if (!mScene->getCanvasCurrent()->getGeodeData()->addDrawable(mStroke))
            warningMsg("redo(): problem while adding stroke to a canvas");
        // update GLWidgets or set canvas' geometry dirty
    }
}

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
