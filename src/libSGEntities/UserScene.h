#ifndef USERSCENE_H
#define USERSCENE_H

#include <string>

#include <QUndoStack>
#include <QObject>
#include <QModelIndex>
#include <QTreeWidgetItem>

#include <osg/Group>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgDB/ObjectWrapper>

#include "Canvas.h"
#include "Stroke.h"
#include "Photo.h"
#include "Bookmarks.h"
#include "../libGUI/ListWidget.h"
#include "../libGUI/TreeWidget.h"

/*! \namespace entity
 * \brief Scene graph entities
*/
namespace entity {

/*! \class UserScene
 * \brief The UserScene class contains scene graph that was created by user, including
 * the construction tools.
 *
 * The children of the UserScene are the canvases. One of the children is the Bookmarks group.
 * The class also contains observer pointers to the current, previous and clone canvases.
 * Normally, the clone canvas is NULL pointer, unless the user is in the process of creating
 * a canvas by clonning or separation procedures.
 *
 * The Bookmarks child can be put at any position as a child, but it's the only child. It is
 * inserted when the user creates the first bookmark of the scene. Here is an example
 * of possible user scene structure:
 *
 *     UserScene -> Canvas0     [previous]
 *        |-> Canvas1
 *        |-> Bookmarks
 *        |-> Canvas2     [current]
 *        |-> ... (other canvases)
 *
 * The UserScene inherits osg::Group and QObject. The osg::Group is necessary for a scene graph
 * representation, while QObject allows for signal and slots functionality. Most of the signal and
 * slots are connected with MainWindow. The inheritance sequence is important: multiple inheritance
 * requires QObject to be first. For more details on this, see:
 * http://www.setnode.com/blog/qt-staticmetaobject-is-not-a-member-of/
*/
class UserScene : public QObject, public osg::Group
{
    Q_OBJECT
public:
    /*! Default constructor. This is the constructor to use within the application. */
    UserScene();

    /*! Copy constructor which is required for OSG serialiazation functionality.
     * It should never be used as-is within the application.
     * \param scene is the scene to copy
     * \param copyop is the copy method, i.e. shallow vs. deep copy */
    UserScene(const UserScene& scene, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, UserScene)

    /*! Setters and getters methods for OSG serialization. The setters methods are never used
     * as-is within the application, and are only for OSG serialization requirement.
*/

    void setBookmarks(entity::Bookmarks* group);
    const entity::Bookmarks* getBookmarks() const;

    /*! \fn getBookmarksModel
     * \return a non-constant Bookmarks group (editable) */
    entity::Bookmarks* getBookmarksModel() const;

    /*! \fn setIdCanvas
     * \param id is setter for the m_idCanvas
     * Must not be used within the application. */
    void setIdCanvas(unsigned int id);
    /*! \fn getIdCanvas
     * \return The maximum ID of canvas, m_idCanvas */
    unsigned int getIdCanvas() const;

    /*! \fn setIdPhoto
     * \param id is setter for the m_idPhoto
     * Must not be used within the application. */
    void setIdPhoto(unsigned int id);
    /*! \fn getIdPhoto
     * \return The maximum ID of photo, m_idPhoto */
    unsigned int getIdPhoto() const;

    /*! \fn setIdBookmark
     * \param id is setter for the m_idBookmark
     * Must not be used within the application. */
    void setIdBookmark(unsigned int id);
    /*! \fn getIdBookmark
     * \return The maximum ID of bookmark, m_idBookmark */
    unsigned int getIdBookmark() const;

    /*! \fn setFilePath
     * \param name is of type std::string to set up a file path variable where
     * the scene graph is saved to. Can be used within the application. */
    void setFilePath(const std::string& name);
    /*! \fn getFilePath
     * \return Name of file path variable where the scene fraph is saved to */
    const std::string& getFilePath() const;
    /*! \fn isSetFilePath
     * \return Bool value indicating whether the file path was set or not */
    bool isSetFilePath() const;


    /*! Creates and adds a Canvas as a child to the UserScene through undo/redo framework by assigning
     * it an automatic name.
     * \param stack is the undo/redo stack where the AddCanvasCommand will be pushed to
     * \param R is the rotation matrix of the new canvas
     * \param T is the transtaltion matrix of the new canvas */
    void addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T);

    /*! Creates and adds a Canvas as a child to the UserScene through undo/redo framework by asigning
     * it an automatic name.
     * \param stack is the undo/redo stack where the AddCanvasCommand will be pushed to
     * \param normal is the normal of the new canvas
     * \param center is the center location of the new canvas */
    void addCanvas(QUndoStack* stack, const osg::Vec3f& normal, const osg::Vec3f& center);

    /*! Creates and adds a Canvas as a child to the UserScene through undo/redo framework.
     * This method is normally called by UserScene::addCanvas(QUndoStack*, const osg::Matrix&, const osg::Matrix&).
     *
     * \param stack is the undo/redo stack where the AddCanvasCommand will be pushed to
     * \param R is the rotation matrix of the new canvas
     * \param T is the transtaltion matrix of the new canvas
     * \param name is the canvas name */
    void addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T, const std::string& name);


    /*! Adds a point to a current stroke of the current canvas through undo/redo framework.
     * If there is no current stroke exists, it creates it.
     * \param stack  is the undo/redo stack where the AddStrokeCommand will be pushed to
     * \param u is the local canvas U-coordinate of the stroke point [u, v] to append to Canvas::m_strokeCurrent
     * \param v is the local canvas V-coordinate of the stroke point [u, v] to append to Canvas::m_strokeCurrent
     * \param event is the current mouse state: click, drag or release */
    void addStroke(QUndoStack* stack, float u, float v, cher::EVENT event);

    /*! Creates and adds a photo to  a current canvas through undo/redo framework by
     * asigning to it an automatic name.
     * \param stack is the undo/redo stack where the AddPhotoCommand will be pushed to
     * \param fname is the photo file path from where it is loaded */
    void addPhoto(QUndoStack* stack, const std::string& fname);


    /*! Creates and adds a bookmark to the Bookmarks group and the corresponding BookmarkWidget
     * by asigning to it an automatic name.
     * \param widget is the BookmarkWidget which is connected to the Bookmarks data structure
     * \param eye is the vector of camera eye to add as a bookmark
     * \param center is the vector of camera center to add as a bookmark
     * \param up is the vector of camera up to add as a bookmark */
    void addBookmark(BookmarkWidget *widget, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

    /*! Updates the screenshot of the bookmark within the BookmarkWidget.
     * \param widget is the BookmarkWidget which is connected to the Bookmarks data structure
     * \param row is the id number of the bookmark in a bookmark widget and Bookmarks data structure */
    void updateBookmark(BookmarkWidget *widget, int row);

    /*! Deletes the bookmark from Bookmarks data strucure as well as BookmarkWidget.
     * \param widget is the BookmarkWidget  which is connected to the Bookmarks data structure
     * \param index is the bookmark id number */
    void deleteBookmark(BookmarkWidget *widget, const QModelIndex& index);


    /*! This method is in development and must not be used as it is now (v0.4.0).
     * The main idea is to erase some points from a stroke (eraser functionality)
     * \param stack
     * \param stroke is the stroke from which to erase
     * \param first is the first index of interval of points to remove
     * \param last is the last index of interval of points to remove
     * \param event is event for pressed, dragged or released */
    void eraseStroke(QUndoStack* stack, entity::Stroke* stroke, int first, int last, cher::EVENT event);


    /*! Gets a pointer to a Canvas based on UserScene child index. This method is useful
     * when, for example, we need to iterate through all the canvases of UserScene.
     * \code
     *     for (int i=0; i<scene->getNumChildren(); ++i){
     *         entity::Canvas* canvas = scene->getCanvas(i);
     *         if (!canvas) continue; // it can be NULL if the child is Bookmarks structure
     *         // do stuff with canvas
     *     }
     * \endcode
     * \param id is the UserScene child index_sequence
     * \return a pointer on Canvas under the given index */
    entity::Canvas* getCanvas(unsigned int id);

    /*! Gets a pointer to a Canvas based on name match. Use with caution: in case if there
     * are two or more canvases with the same name, it will return the first found child which
     * name matches the given string.
     * \param name is the name to match
     * \return a pointer on a first found child with the matched name, or NULL if no such child
     * is found. */
    entity::Canvas* getCanvas(const std::string& name);


    /*! Returns a level of Stroke within the whole scene graph, starting from the RootScene.
     * It is used from EventHandler when using intersectors. The return value is defined manually,
     * therefore it is important to keep them updated in case if scene graph is dramatically
     * re-designed. The level for the Stroke is defined:
     *
     *     RootScene -> UserScene -> Canvas -> Canvas::m_transform -> Canvas::m_switch -> Canvas::m_geodeData -> Stroke
     *
     *        [1]        [2]          [3]            [4]                   [5]                     [6]              [7]
     *
     * To simplify, a relative deepness is used, i.e., \code UserScene::getCanvasLevel() + 4 \endcode.
     * \return a deepness level of Stroke from the RootScene. */
    int getStrokeLevel() const;

    /*! Returns a level of Canvas within the whole scene graph, starting from the RootScene.
     * It is used from EventHandler when using intersectors. The return value is defined manually,
     * therefore it is important to keep them updated in case if scene graph is dramatically
     * re-designed. The level for the Canvas is defined:
     *
     *     RootScene -> UserScene -> Canvas
     *
     *        [1]        [2]          [3]
     *
     * \return a deepness level of Canvas from the RootScene. */
    int getCanvasLevel() const;

    /*! Returns a level of Photo within the whole scene graph, starting from the RootScene.
     * It is used from EventHandler when using intersectors. The return value is defined manually,
     * therefore it is important to keep them updated in case if scene graph is dramatically
     * re-designed. The level for the Photo is defined:
     *
     *     RootScene -> UserScene -> Canvas -> Canvas::m_transform -> Canvas::m_switch -> Canvas::m_geodeData -> Photo
     *
     *        [1]        [2]          [3]            [4]                   [5]                     [6]              [7]
     *
     * To simplify, a relative deepness is used, i.e., \code UserScene::getCanvasLevel() + 4 \endcode.
     * \return a deepness level of Photo from the RootScene. */
    int getPhotoLevel() const;


    /*! Sets given Canvas as a current. This sets the variable UserScene::m_canvasCurrent
     * to pointer of given Canvas. The current status also means change of Canvas
     * frame color. When the frame color changes, the function emits a signal which
     * is connected to CanvasWidget to mark the corresponding Canvas as selected in
     * widget UI.
     * Most of the time this method is called by either CanvasWidget, EventHandler
     * methods or undo/redo framework classes (e.g. Add*Command and Edit*Command).
     * \param cnv is the Canvas to set as current
     * \return The bool parameter which indicates the success of current status change (also checks for validity of
     * pointers). */
    bool setCanvasCurrent(entity::Canvas* cnv);

    /*! Sets given Canvas as previous. This sets the variable UserScene::m_canvasPrevious
     * to pointer of given Canvas. The previous status also means change of Canvas
     * frame color. When the frame color changes, the function emits a signal which
     * is connected to CanvasWidget to mark the corresponding Canvas as selected in
     * widget UI.
     * Most of the time this method will be called automatically by UserScene::setCanvasCurrent(),
     * although there are few cases when it is called by other means, e.g., from CanvasWidget
     * when user sets previous status by right mouse click.
     * \param cnv is the Canvas to set as previous
     * \return The bool parameter which indicates the success of current status change (also checks for validity of
     * pointers). */
    bool setCanvasPrevious(entity::Canvas* cnv);

    /*! Sets the traversal mask so that all the canvases (except current) are either traversible or not,
     * depending on the input parameter. This function would be called from EventHandler when we want
     * to exclude all the canvases but current from ray casting and intersection search (or enable
     * the traversal mask back).
     *
     * An example for turning the traversal off: selection of 2D entities within the current canvas (in this case
     * we do not want the contents of other canvases to mix in with the intersection results).
     * An example for turning the traversal masks on: 3D selection of Canvas by clicking on its pickable.
     * \param enabled is a boolean variable which indicates whether we want to set the traversal to on (true)
     * or off (false).
     * \sa EventHandler::setMode()
*/
    void setCanvasesButCurrent(bool enabled);

    /*! \return The state of traversal masks of all the canvases except the current. True indicates the
     * traversal masks are on (traversal is performed), false indicated the traversal masks are off.
     * \sa setCanvasesButCurrent(),
     * \sa EventHandler::setMode() */
    bool getCanvasesButCurrent() const;

    /*! \return A pointer on current canvas, editable. */
    entity::Canvas* getCanvasCurrent() const;

    /*! \return A pointer on previous canvas, editable. */
    entity::Canvas* getCanvasPrevious() const;


    /*! A method to return a sequential index of Canvas pointer. Normally, the index is aligned with
     * child index of UserScene, however, it is not always the case since one of the children is
     * Bookmarks group. The method is frequently used when we need to obtain the index number to deal with
     * CanvasWidget entries. Example of methods that use it are: setting canvas as current / previous;
     * model resetting, adding new canvas and photo addition and removal.
     * \param canvas is the pointer on Canvas
     * \return index of the Canvas as is in CanvasWidget
     * \sa getCanvasFromIndex() */
    int getCanvasIndex(entity::Canvas* canvas) const;

    /*! Similar to getCanvasIndex(), the method is to return a sequential index of a photo as it is in
     * CanvasWidget. It iterates through all the children of Canvas and compares them to the Photo pointer.
     * Whenever the child is of type Photo, the index value increases. When the pointers match, the result
     * index is returned.
     * \param photo is a Photo of which index we want to obtain
     * \param canvas is a Canvas that contains the photo
     * \return index of the given Photo within the given Canvas as is in CanvasWidget.
     * \sa getPhotoFromIndex() */
    int getPhotoIndex(entity::Photo* photo, entity::Canvas* canvas) const;

    /*! A method to return a Canvas pointer given sequential index within CanvasWidget.
     * Returns a child of UserScene as a Canvas with given sequential index, taking Bookmarks group
     * into consideration.
     * \param row is the sequential index as is in CanvasWidget
     * \return Canvas pointer of the index.
     * \sa getCanvasIndex() */
    entity::Canvas* getCanvasFromIndex(int row);

    /*! A method to return a Photo pointer given sequential index within a Canvas.
     * Returns a child of Canvas with given sequential index.
     * \param canvas is the Canvas which is a parent of requested Photo
     * \param row is the sequential index as is in CanvasWidget
     * \return Photo pointer of the index.
     * \sa getPhotoIndex() */
    entity::Photo* getPhotoFromIndex(entity::Canvas* canvas, int row);

    /*! \return the number of canvases on the scene */
    int getNumCanvases() const;

    /*! \return the number of photos within the given Canvas */
    int getNumPhotos(entity::Canvas* canvas) const;

    /*! The method is to edit the canvas 3D position, offset in particular. The offset is defined as a
     * non-zero distance along the canvas normal in either positive or negative direction.
     * \param stack is the command stack for undo/redo framework
     * \param translate is the 3D difference vector which indicates translation along the canvas normal
     * from previous canvas position
     * \param event is to indicate a mouse state: clicked, dragged or released
     * \sa canvasOffsetStart(), canvasOffsetAppend(), canvasOffsetFinish() */
    void editCanvasOffset(QUndoStack* stack, const osg::Vec3f& translate, cher::EVENT event);

    /*! The method is to edit the canvas 3D position, by means of rotation. The rotation is defined as a
     * quat which was performed along either local U or local V Canvas axis.
     * \param stack is the command stack for undo/redo framework
     * \param rotation is the quat which indicates rotation from the previous canvas position
     * \param center3d is a 3D global point along which the rotation is performed (as of v0.4.0 this value
     * is represented by a Canvas::m_center and is fixed)
     * \param event is to indicate a mouse state: clicked, dragged or released
     * \sa canvasRotateStart(), canvasRotateAppend(), canvasRotateFinish() */
    void editCanvasRotate(QUndoStack* stack, const osg::Quat& rotation, const osg::Vec3f& center3d, cher::EVENT event);

    /*! The method is to create a new canvas by clonning a current canvas and then translating the newly
     * created canvas along its normal. While in the clonning mode,
     * three mouse modes make different states of creation:
     *     * click performs an actual clone
     *     * drag allows user to chose the new position of the cloned canvas
     *     * release fixes the position and pushes the cloned canvas to undo/redo stack
     *
     * \param stack is the command stack for undo/redo framework
     * \param translate  is the 3D difference vector which indicates translation along the canvas normal
     * from previous canvas position
     * \param event is to indicate a mouse state: clicked, dragged or released
     * \sa canvasCloneStart(), canvasCloneAppend(), canvasCloneFinish()
     * \sa editCanvasSeparate() */
    void editCanvasClone(QUndoStack* stack, const osg::Vec3f& translate, cher::EVENT event);

    /*! Similar to editCanvasClone(), the method creates a new canvas by performing separation (not clonning). By separation
     * it is meant a cut and paste of selected strokes into the new Canvas. While in the separation mode, three mouse modes
     * make different states of separation:
     *     * click to perform an actual separation
     *     * drag to position the newly created canvas in 3D space
     *     * release to fix the canvas position and to push the separation operation into undo/redo stack
     *
     * \param stack is the command stack for undo/redo framework
     * \param translate  is the 3D difference vector which indicates translation along the canvas normal
     * \param event is to indicate a mouse state: clicked, dragged or released
     * \sa canvasSeparateStart(), canvasSeparateAppend(), canvasSeparateFinish()
     * \sa editCanvasClone() */
    void editCanvasSeparate(QUndoStack* stack, const osg::Vec3f& translate, cher::EVENT event);

    /*! \param stack  is the command stack for undo/redo framework
     * \param canvas is the canvas to delete from UserScene */
    void editCanvasDelete(QUndoStack* stack, entity::Canvas* canvas);

    /*! \param stack  is the command stack for undo/redo framework
     * \param photo is the photo to delete
     * \param canvas is the canvas from which the photo will be deleted */
    void editPhotoDelete(QUndoStack* stack, entity::Photo* photo, entity::Canvas* canvas);

    /*! The method is to transfer a photo from one canvas to another. As of v0.4.0 it is in development and advised not to be used
     * for time being; have to fix the syncronization with CanvasWidget
     * \param stack  is the command stack for undo/redo framework
     * \param photo is the photo to move
     * \param source is the canvas from where the photo will be removed
     * \param destination is the canvas to where to photo will be moved */
    void editPhotoPush(QUndoStack* stack, entity::Photo* photo, entity::Canvas* source,
                       entity::Canvas* destination);

    /*! The method is to transfer a set of strokes from current canvas to previous. The transfer is projective, i.e. under
     * given camera view the whole sketching image does not change, although the set of strokes now belongs to the
     * previous canvas.
     * \param stack  is the command stack for undo/redo framework
     * \param camera is the camera pointer that contains current camera parameters (eye, center, up) */
    void editStrokesPush(QUndoStack* stack, osg::Camera* camera);


    /*! The method is to perform a movement of a set of strokes within a canvas. That is, it can be considered as a 2D local move.
     * \param stack  is the command stack for undo/redo framework
     * \param u is the local U coordinate where the center of strokes [u, v] will be moved to
     * \param v is the local V coordinate where the center of strokes [u, v] will be moved to
     * \param event is to indicate a mouse state: clicked, dragged or released
     * \sa editStrokesRotate(), editStrokesScale() */
    void editStrokesMove(QUndoStack* stack, double u, double v, cher::EVENT event);

    /*! The method is to perform a uniform scaling of a set of strokes within a canvas.
     * \param stack  is the command stack for undo/redo framework
     * \param u is the local U coordinate where the bounding box point [u, v] will be moved to
     * \param v is the local V coordinate where the bounding box point [u, v] will be moved to
     * \param event is to indicate a mouse state: clicked, dragged or released
     * \sa editStrokesMove(), \sa editStrokesRotate() */
    void editStrokesScale(QUndoStack* stack, double u, double v, cher::EVENT event);

    /*! The method is to perform a rotation of a set of strokes within a canvas. That is, it can be
     * considered as a 2D local rotation.
     * \param stack  is the command stack for undo/redo framework
     * \param u is the local U coordinate where the local axis [u, v] will be rotated to
     * \param v is the local V coordinate where local axis [u, v] will be rotated to
     * \param event is to indicate a mouse state: clicked, dragged or released
     * \sa editStrokesMove(), editStrokesScale()  */
    void editStrokesRotate(QUndoStack* stack, double u, double v, cher::EVENT event);

    /*! The method is to delete a specified stroke.
     * \param stack  is the command stack for undo/redo framework
     * \param stroke is a stroke to delete from scene */
    void editStrokeDelete(QUndoStack* stack, entity::Stroke* stroke);


    /*! \return A boolean value telling whether the scene is empty (true) or not (false) */
    bool isEmptyScene() const;

    /*! A method to clear all the user data from scene graph: canvases, bookmarks and their contents.
     * \return Whether the operation was successfull (true) or not (false). */
    bool clearUserData();

    /*! A tester method which is run when a scene is loaded from file.
     * \return Whether the test was successfull (true) or not (false). */
    bool printScene();

    /*! A method which connects UserScene with MainWindow (and/or GLWidget). Its main purpose is to request for
     * GLWidget::update() method to renew the viewport. */
    void updateWidgets();

    /*! A method to reset Canvas and Photo data within the corresponding widget, as well as its model. It updates the
     * widget with the new UserScene. This method is called when a file was open. The main principle is that it traverses
     * the scene graph and adds the found Canvas entities to the CanvasPhotoWidget.
     * \param widget is the CanvasPhotoWidget to update */
    void resetModel(CanvasPhotoWidget* widget);

signals:
    /*! A signal which is connected with MainWindow::recievedRequestUpdate() to request for GLWidget update. */
    void sendRequestUpdate();

    /*! A signal which is connected with  CanvasPhotoWidget::onCanvasAdded() to request to add a canvas item to the widget */
    void canvasAdded(const std::string& name);

    /*! A signal which is connected with  CanvasPhotoWidget::onPhotoAdded() to request to add a photo item to the widget */
    void photoAdded(const std::string& name, int idxCanvas);

    /*! A signal which is connected with  CanvasPhotoWidget::onCanvasRemoved() to request to remove a canvas item from the widget */
    void canvasRemoved(int row);

    /*! A signal which is connected with  CanvasPhotoWidget::onPhotoRemoved() to request to remove a photo item from the widget */
    void photoRemoved(int rowCanvas, int rowPhoto);

    /*! A signal which is connected with  CanvasPhotoWidget::onCanvasSelectedColor() to request to update color indications for a
     * certain canvas, e.g., when switching the canvas status from normal to current, or from current to previous. */
    void canvasSelectedColor(int row, int color);

    /*! A signal which is connected with CanvasPhotoWidget and is called from RootScene when the scene is loaded from file.
     * It is to provide a back feedback from scene to the widget and it triggers the setting of the visibility icon
     * ("open" or "closed" eye) which is related to the corresponding canvas.
     * \param row is the sequential canvas index as it is in the canvas-photo widget
     * \param visibility is the boolean variable indicating whether the canvas is visible on the scene (true) or not (false).
     * This value also corresponds to Canvas::getVisibilityData(). */
    void canvasVisibilitySet(int row, bool visibility);

public slots:
    /*! A slot which is called when the canvas name was edited from CanvasPhotoWidget. Based on the edited item, the slot
     * edits the name of the corresponding canvas. This slot is connected to CanvasPhotoWidget::itemChanged() */
    void onItemChanged(QTreeWidgetItem* item, int column);

    /*! A slot which is called when the user performed a canvas left-click selection from a tree widget. Based on what item was selected,
     * the slots changes the corresponding canvas status to current. This slot is connected with CanvasPhotoWidget::clicked(). */
    void onClicked(const QModelIndex& index);

    /*! A slot which is called when the user performed a canvas right-click selection from a tree widget. Based on what item was selected,
     * the slots changes the corresponding canvas status to previous. This slot is connected with CanvasPhotoWidget::rightClicked(). */
    void onRightClicked(const QModelIndex& index);

protected:
    ~UserScene();

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    std::string getCanvasName();
    std::string getPhotoName();
    std::string getBookmarkName();
    std::string getEntityName(const std::string& name, unsigned int id) const;

    void strokeStart();
    void strokeAppend(float u, float v);
    void strokeFinish(QUndoStack* stack);
    bool strokeValid() const;

    void entitiesMoveStart(double u, double v);
    void entitiesMoveAppend(double u, double v);
    void entitiesMoveFinish(QUndoStack* stack);
    bool entitiesSelectedValid() const;

    void entitiesScaleStart(double u, double v);
    void entitiesScaleAppend(double u, double v);
    void entitiesScaleFinish(QUndoStack* stack);

    void entitiesRotateStart(double u, double v);
    void entitiesRotateAppend(double u, double v);
    void entitiesRotateFinish(QUndoStack* stack);

    void eraseStart(entity::Stroke* stroke, osg::Vec3d& hit);
    void eraseAppend(entity::Stroke* stroke, osg::Vec3d& hit);
    void eraseFinish(QUndoStack* stack, entity::Stroke* stroke);
    bool eraseValid(entity::Stroke* stroke) const;

    void canvasOffsetStart();
    void canvasOffsetAppend(const osg::Vec3f& t);
    void canvasOffsetFinish(QUndoStack* stack);
    bool canvasEditValid() const;

    void canvasCloneStart();
    void canvasCloneAppend(const osg::Vec3f& t);
    void canvasCloneFinish(QUndoStack* stack);
    bool canvasCloneValid() const;

    void canvasSeparateStart();
    void canvasSeparateAppend(const osg::Vec3f& t);
    void canvasSeparateFinish(QUndoStack* stack);

    void canvasRotateStart();
    void canvasRotateAppend(const osg::Quat& r, const osg::Vec3f& center3d);
    void canvasRotateFinish(QUndoStack* stack);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

private:
    osg::ref_ptr<entity::Bookmarks> m_bookmarks; /*!< Pointer on Bookmarks data structure, it is one of the direct children of UserScene. */
    osg::observer_ptr<entity::Canvas> m_canvasCurrent; /*!< Observer pointer on current canvas. */
    osg::observer_ptr<entity::Canvas> m_canvasPrevious; /*!< Observer pointer on previous canvas. */
    osg::ref_ptr<entity::Canvas> m_canvasClone; /*!< Smart pointer for clone and separation canvas operations. */

    osg::Vec3f m_deltaT;    /*!< Temporarly variable for edit operations: translate canvas. */
    osg::Quat m_deltaR;     /*!< Temporarly variable for for edit operation: rotate canvas. */
    double m_u              /*!< Temporarly variable for edit operations: move, scale, rotate of 2D entities. */
        ,  m_v;             /*!< Temporarly variable for edit operations: move, scale, rotate of 2D entities. */
    bool m_inits;           /*!< Temporarly variable for defining if initialization of operations was performed or not. */
    double m_du             /*!< Temporarly variable for edit operations: move, scale, rotate of 2D entities. */
        ,  m_dv;            /*!< Temporarly variable for edit operations: move, scale, rotate of 2D entities. */
    double m_scaleX         /*!< Temporarly variable for edit operations: scale of 2D entities. */
        ,  m_scaleY;        /*!< Temporarly variable for edit operations: scale of 2D entities. */
    double m_rotate;        /*!< Temporarly variable for edit operations: rotate of 2D entities. */
    unsigned int m_idCanvas;    /*!< Naming convention identification number for canvases. */
    unsigned int m_idPhoto;     /*!< Naming convention identification number for photos. */
    unsigned int m_idBookmark;  /*!< Naming convention identification number for bookmarks. */
    std::string m_filePath;     /*!< File path where the scene is saved to. */
};

}

#endif // USERSCENE_H
