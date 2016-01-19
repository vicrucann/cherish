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



EditPhotoMoveCommand::EditPhotoMoveCommand(entity::Canvas *canvas, entity::Photo *photo, const double u, const double v, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_canvas(canvas)
    , m_photo(photo)
    , m_u0(photo->getCenter().x())
    , m_v0(photo->getCenter().y())
    , m_u1(u)
    , m_v1(v)
{
    this->setText(QObject::tr("Move photo %1 within canvas %2")
                  .arg(QString(m_photo->getName().c_str()),
                       QString(m_canvas->getName().c_str())));
}

EditPhotoMoveCommand::~EditPhotoMoveCommand()
{

}

void EditPhotoMoveCommand::undo()
{
    m_photo->move(m_u0, m_v0);
    m_canvas->updateFrame();
}

void EditPhotoMoveCommand::redo()
{
    m_photo->move(m_u1, m_v1);
    m_canvas->updateFrame();
}
