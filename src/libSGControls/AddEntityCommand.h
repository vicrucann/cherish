#ifndef ADDENTITYCOMMAND_H
#define ADDENTITYCOMMAND_H

#include <QUndoCommand>

#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"

class AddCanvasCommand : public QUndoCommand
{
public:
    AddCanvasCommand(entity::UserScene* scene, const osg::Matrix& R, const osg::Matrix& T,
                     const std::string& name, QUndoCommand* parent = 0);
    AddCanvasCommand(entity::UserScene* scene, const entity::Canvas& copy,
                     QUndoCommand* parent = 0);
    ~AddCanvasCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Canvas> m_canvas;
};

class AddPhotoCommand : public QUndoCommand
{
public:
    AddPhotoCommand(entity::UserScene* scene, const std::string& fname, const std::string& ename,
                    QUndoCommand* parent = 0);
    ~AddPhotoCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Photo> m_photo;
};

class AddStrokeCommand : public QUndoCommand
{
public:
    AddStrokeCommand(entity::UserScene* scene, entity::Stroke* stroke, QUndoCommand *parent = 0);
    ~AddStrokeCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Stroke> m_stroke;
};

#endif // ADDENTITYCOMMAND_H
