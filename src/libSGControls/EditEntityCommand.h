#ifndef EDITENTITYCOMMAND_H
#define EDITENTITYCOMMAND_H

#include <vector>

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"

class EditCanvasOffsetCommand : public QUndoCommand
{
public:
    EditCanvasOffsetCommand(entity::UserScene* scene, const osg::Vec3f& translate, QUndoCommand* parent = 0);
    ~EditCanvasOffsetCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Vec3f m_translate;
};

class EditCanvasRotateCommand : public QUndoCommand
{
public:
    EditCanvasRotateCommand(entity::UserScene* scene, const osg::Quat& rotate, QUndoCommand* parent = 0);
    ~EditCanvasRotateCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Quat m_rotate;
};

class EditCanvasDeleteCommand : public QUndoCommand
{
public:
    EditCanvasDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, QUndoCommand* parent = 0);
    ~EditCanvasDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Canvas> m_canvas;
};

class EditPhotoMoveCommand : public QUndoCommand
{
public:
    EditPhotoMoveCommand(entity::UserScene* scene, const double u, const double v, QUndoCommand* parent = 0);
    ~EditPhotoMoveCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    double m_u0, m_v0, m_u1, m_v1;
};

class EditPhotoScaleCommand : public QUndoCommand
{
public:
    EditPhotoScaleCommand(entity::UserScene* scene, const double scale, QUndoCommand* parent = 0);
    ~EditPhotoScaleCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    double m_scale;
};

class EditPhotoFlipCommand : public QUndoCommand
{
public:
    EditPhotoFlipCommand(entity::UserScene* scene, bool horizontal, QUndoCommand* parent = 0);
    ~EditPhotoFlipCommand(){}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    bool m_horizontal;
};

class EditPhotoRotateCommand : public QUndoCommand
{
public:
    EditPhotoRotateCommand(entity::UserScene* scene, const double angle, QUndoCommand* parent = 0);
    ~EditPhotoRotateCommand(){}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    double m_angle;
};

class EditPhotoDeleteCommand : public QUndoCommand
{
public:
    EditPhotoDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, entity::Photo* photo, QUndoCommand* parent = 0);
    ~EditPhotoDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Photo> m_photo;
};

class EditStrokesPushCommand : public QUndoCommand
{
public:
    EditStrokesPushCommand(entity::UserScene* scene, const std::vector<entity::Stroke*>& strokes, entity::Canvas* current, entity::Canvas* target,
                           const osg::Vec3f& eye, QUndoCommand* parent = 0);
    ~EditStrokesPushCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:

    void doPushStrokes(entity::Canvas& source, entity::Canvas& target);

    osg::observer_ptr<entity::UserScene> m_scene;
    const std::vector<entity::Stroke*> m_strokes;
    osg::observer_ptr<entity::Canvas> m_canvasCurrent;
    osg::observer_ptr<entity::Canvas> m_canvasTarget;
    osg::Vec3f m_eye;
};

class EditStrokesMoveCommand : public QUndoCommand
{
public:
    EditStrokesMoveCommand(entity::UserScene* scene, const std::vector<entity::Stroke*>& strokes, entity::Canvas* canvas,
                           double du, double dv, QUndoCommand* parent = 0);
    ~EditStrokesMoveCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    std::vector<entity::Stroke*> m_strokes;
    osg::observer_ptr<entity::Canvas> m_canvas;
    double m_du;
    double m_dv;
};

class EditStrokeDeleteCommand : public QUndoCommand
{
public:
    EditStrokeDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, entity::Stroke* stroke, QUndoCommand* parent = 0);
    ~EditStrokeDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Stroke> m_stroke;
};

#endif // EDITENTITYCOMMAND_H
