#ifndef EDITCANVASCOMMAND_H
#define EDITCANVASCOMMAND_H

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "Canvas.h"

class EditCanvasOffsetCommand : public QUndoCommand
{
public:
    EditCanvasOffsetCommand(entity::Canvas* canvas, const osg::Vec3f& translate, QUndoCommand* parent = 0);
    ~EditCanvasOffsetCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Vec3f m_translate;
};

class EditCanvasRotateCommand : public QUndoCommand
{
public:
    EditCanvasRotateCommand(entity::Canvas* canvas, const osg::Quat& rotate, QUndoCommand* parent = 0);
    ~EditCanvasRotateCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Quat m_rotate;
};

#endif // EDITCANVASCOMMAND_H
