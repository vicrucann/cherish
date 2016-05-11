#ifndef ADDENTITYCOMMAND_H
#define ADDENTITYCOMMAND_H

#include <QUndoCommand>

#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"

/*! \class AddCanvasCommand
 * Class description
*/
class AddCanvasCommand : public QUndoCommand
{
public:
    AddCanvasCommand(entity::UserScene* scene, const osg::Matrix& R, const osg::Matrix& T,
                     const std::string& name, QUndoCommand* parent = 0);
    AddCanvasCommand(entity::UserScene* scene, const entity::Canvas& copy,
                     QUndoCommand* parent = 0);
    AddCanvasCommand(entity::UserScene* scene, const osg::Vec3f& normal, const osg::Vec3f& center,
                     const std::string& name, QUndoCommand* parent = 0);

    /*! \fn undo
     * Performs an undo of the command */
    void undo() Q_DECL_OVERRIDE;

    /*! \fn redo
     * Performs a redo of the command */
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Canvas> m_canvas;
};

/*! \class AddCanvasSeparationCommand
 * Class to perform canvas separate within undo/redo framework
*/
class AddCanvasSeparationCommand : public QUndoCommand
{
public:
    AddCanvasSeparationCommand(entity::UserScene* scene, entity::Canvas* source, entity::Canvas* copy,
                               QUndoCommand* parent = 0);

    /*! \fn undo
     * Performs an undo of the command */
    void undo() Q_DECL_OVERRIDE;

    /*! \fn redo
     * Performs a redo of the command */
    void redo() Q_DECL_OVERRIDE;

private:
    void moveEntities(entity::Canvas* from, entity::Canvas* to);

    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_source;
    osg::ref_ptr<entity::Canvas> m_target;
    std::vector<entity::Entity2D*> m_entities;
};

/*! \class AddPhotoCommand
 * Class description
*/
class AddPhotoCommand : public QUndoCommand
{
public:
    AddPhotoCommand(entity::UserScene* scene, const std::string& fname, const std::string& ename,
                    QUndoCommand* parent = 0);

    /*! \fn undo
     * Performs an undo of the command */
    void undo() Q_DECL_OVERRIDE;

    /*! \fn redo
     * Performs a redo of the command */
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Photo> m_photo;
};

/*! \class AddStrokeCommand
 * Class description
*/
class AddStrokeCommand : public QUndoCommand
{
public:
    AddStrokeCommand(entity::UserScene* scene, entity::Stroke* stroke, QUndoCommand *parent = 0);
    ~AddStrokeCommand();

    /*! \fn undo
     * Performs an undo of the command */
    void undo() Q_DECL_OVERRIDE;

    /*! \fn redo
     * Performs a redo of the command */
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Stroke> m_stroke;
};

#endif // ADDENTITYCOMMAND_H
