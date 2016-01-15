#include "AddStrokeCommand.h"

#include <QObject>

#include "settings.h"

AddStrokeCommand::AddStrokeCommand(entity::UserScene* scene, entity::Stroke* stroke, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_stroke(stroke)
{
    this->setText(QObject::tr("Add new stroke to canvas %1")
                  .arg(QString( m_canvas->getName().c_str() ) ) );
}

AddStrokeCommand::~AddStrokeCommand()
{
}

void AddStrokeCommand::undo()
{
    if (!m_canvas->getGeodeData()->removeChild(m_stroke))
        outErrMsg("undo(): problem while removing stroke from a canvas");
    // update GLWidgets or set canvas' geometry dirty

}

void AddStrokeCommand::redo()
{
    if (!m_canvas->getGeodeData()->addDrawable(m_stroke))
        outErrMsg("redo(): problem while adding stroke to a canvas");
    // update GLWidgets or set canvas' geometry dirty
}
