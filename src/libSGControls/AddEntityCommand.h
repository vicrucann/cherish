#ifndef ADDENTITYCOMMAND_H
#define ADDENTITYCOMMAND_H

#include <QUndoCommand>

#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"
#include "Polygon.h"
#include "Bookmarks.h"
#include "SceneState.h"

namespace entity {
class UserScene;
class Bookmarks;
}

/*! \namespace fur
 * \brief Contains QUndoCommand - based classes that are defined within undo/redo framework.
 * All these classes are made friend classes for entity::UserScene (protected) so that to access add and remove entity
 * functionality.
*/
namespace fur {


/*! \class AddCanvasCommand
 * \brief QUndoCommand that performs addition of a canvas to entity::UserScene.
*/
class AddCanvasCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to add to, \param R is the rotation matrix of the canvas, \param T is translation matrix of the canvas,
     * \param name is the Canvas name, \param parent is normally 0. */
    AddCanvasCommand(entity::UserScene* scene, const osg::Matrix& R, const osg::Matrix& T,
                     const std::string& name, QUndoCommand* parent = 0);

    /*! \param scene is the scene graph to add to, \param copy is Canvas to copy from, \param parent is normally 0.  */
    AddCanvasCommand(entity::UserScene* scene, const entity::Canvas& copy,
                     QUndoCommand* parent = 0);

    /*! \param scene is the scene graph to add to, \param normal is a 3D normal of the canvas to create, \param center is the 3D center point of canvas to create,
     * \param name is the canvas name, \param parent is normally 0. */
    AddCanvasCommand(entity::UserScene* scene, const osg::Vec3f& normal, const osg::Vec3f& center,
                     const std::string& name, QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Bookmarks> m_bookmarks;
}; // class AddCanvasCommand

/*! \class AddCanvasSeparationCommand
 * \brief QUndoCommand that performs separation of a canvas and addition of it to entity::UserScene.
*/
class AddCanvasSeparationCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to add to, \param source is the canvas source from which separation will occur, \param copy is the destination canvas parameters, \param parent is normally 0. */
    AddCanvasSeparationCommand(entity::UserScene* scene, entity::Canvas* source, entity::Canvas* copy,
                               QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif // DOXYGEN_SHOULD_SKIP_THIS

private:
    void moveEntities(entity::Canvas* from, entity::Canvas* to);

    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_source;
    osg::ref_ptr<entity::Canvas> m_target;
    std::vector<entity::Entity2D*> m_entities;
    osg::observer_ptr<entity::Bookmarks> m_bookmarks;
};

/*! \class AddPhotoCommand
 * \brief QUndoCommand that performs addition of a photo to entity::UserScene.
*/
class AddPhotoCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to add to, \param fname is path of where the photo is located, \param ename is the photo name within the scene graph, \param parent is normally 0. */
    AddPhotoCommand(entity::UserScene* scene, const std::string& fname, const std::string& ename,
                    QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif // DOXYGEN_SHOULD_SKIP_THIS

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Photo> m_photo;
    osg::observer_ptr<entity::Bookmarks> m_bookmarks;
};

/*! \class AddStrokeCommand
 * \brief QUndoCommand that performs addition of a stroke to UserScene.
*/
class AddStrokeCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to add to, \param stroke is the Stroke to be added to entity::UserScene, \param parent is normally 0. */
    AddStrokeCommand(entity::UserScene* scene, entity::Stroke* stroke, QUndoCommand *parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif // DOXYGEN_SHOULD_SKIP_THIS

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Stroke> m_stroke;
};

/*! \class AddPolygonCommand
 * \brief QUndoCommand that perform addition of a entity::Polygon to entity::UserScene
*/
class AddPolygonCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to add to, \param polygon is the entity::Polygon to be added to entity::UserScene, \param parent is normally 0. */
    AddPolygonCommand(entity::UserScene* scene, entity::Polygon* polygon, QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif // DOXYGEN_SHOULD_SKIP_THIS

private:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Polygon> m_polygon;
};

} // namespace fur

#endif // ADDENTITYCOMMAND_H
