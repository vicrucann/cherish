#include "EditEntityCommand.h"

#include <QObject>

EditCanvasOffsetCommand::EditCanvasOffsetCommand(entity::Canvas *canvas, const osg::Vec3f &translate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_canvas(canvas)
    , m_translate(translate)
{
    this->setText(QObject::tr("Offset %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

EditCanvasOffsetCommand::~EditCanvasOffsetCommand()
{
}

void EditCanvasOffsetCommand::undo()
{
    m_canvas->translate(osg::Matrix::translate(-m_translate.x(), -m_translate.y(), -m_translate.z()));
}

void EditCanvasOffsetCommand::redo()
{
    m_canvas->translate(osg::Matrix::translate(m_translate.x(), m_translate.y(), m_translate.z()));
}

/* =====================*/

EditCanvasRotateCommand::EditCanvasRotateCommand(entity::Canvas *canvas, const osg::Quat &rotate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_canvas(canvas)
    , m_rotate(rotate)
{
    this->setText(QObject::tr("Rotate %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

EditCanvasRotateCommand::~EditCanvasRotateCommand()
{

}

void EditCanvasRotateCommand::undo()
{
    double angle;
    osg::Vec3d axis;
    m_rotate.getRotate(angle, axis);
    m_canvas->rotate(osg::Matrix::rotate(-angle, axis));
}

void EditCanvasRotateCommand::redo()
{
    m_canvas->rotate(osg::Matrix::rotate(m_rotate));
}


