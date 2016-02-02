#ifndef EDITENTITYCOMMAND_H
#define EDITENTITYCOMMAND_H

#include <vector>

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"

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
    osg::observer_ptr<entity::Photo> m_photo;
    double m_u0, m_v0, m_u1, m_v1;
};

class EditPhotoScaleCommand : public QUndoCommand
{
public:
    EditPhotoScaleCommand(entity::Canvas* canvas, const double scale, QUndoCommand* parent = 0);
    ~EditPhotoScaleCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    double m_scale;
};

class EditPhotoFlipCommand : public QUndoCommand
{
public:
    EditPhotoFlipCommand(entity::Canvas* canvas, bool horizontal, QUndoCommand* parent = 0);
    ~EditPhotoFlipCommand(){}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    bool m_horizontal;
};

class EditPhotoRotateCommand : public QUndoCommand
{
public:
    EditPhotoRotateCommand(entity::Canvas* canvas, const double angle, QUndoCommand* parent = 0);
    ~EditPhotoRotateCommand(){}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    double m_angle;
};

class EditStrokesPushCommand : public QUndoCommand
{
public:
    EditStrokesPushCommand(const std::vector<entity::Stroke*>& strokes, entity::Canvas* current, entity::Canvas* target,
                           const osg::Vec3f& eye, QUndoCommand* parent = 0);
    ~EditStrokesPushCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:

    void doPushStrokes(entity::Canvas& source, entity::Canvas& target);

    const std::vector<entity::Stroke*> m_strokes;
    osg::observer_ptr<entity::Canvas> m_canvasCurrent;
    osg::observer_ptr<entity::Canvas> m_canvasTarget;
    osg::Vec3f m_eye;
};

#endif // EDITENTITYCOMMAND_H
