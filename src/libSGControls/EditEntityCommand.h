#ifndef EDITENTITYCOMMAND_H
#define EDITENTITYCOMMAND_H

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "Canvas.h"
#include "Photo.h"

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

class EditPhotoMoveCommand : public QUndoCommand
{
public:
    EditPhotoMoveCommand(entity::Canvas* canvas, const double u, const double v, QUndoCommand* parent = 0);
    ~EditPhotoMoveCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::Canvas> m_canvas;
    double m_u0, m_v0, m_u1, m_v1;
};

#endif // EDITENTITYCOMMAND_H
