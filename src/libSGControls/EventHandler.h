#ifndef EVENTHANDLER
#define EVENTHANDLER

/* EventHandler
 * It is represented as a switcher between all the other
 * user-defined handlers. Depending on the mouse mode (select, edit,
 * navigate, sketch, etc), it choses the right handle() function of the
 * corresponding event handler. Of course, all the handlers must be
 * initialized in the contstructor. The mode can be reset by a user class.
*/

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "../libGUI/GLWidget.h"
#include "Settings.h"
#include "Canvas.h"
#include "Photo.h"
#include "SVMData.h"
#include "DraggableWire.h"
#include "UserScene.h"
#include "RootScene.h"
#include "StrokeIntersector.h"
#include "LineIntersector.h"
#include "PolyLineIntersector.h"
#include "PointIntersector.h"
#include "VirtualPlaneIntersector.h"
#include "CanvasNormalProjector.h"

class GLWidget;

/*! \class EventHandler
 * \brief An OpenSceneGraph events that were passed from Qt's interface - GLWidget.
 * Event handler is where all the mouse events are processed. Examples: sketching, canvas position editing,
 * polygon drawing, deletion of entities. The EventHandler, after the mouse info extracted, then sends
 * according signals to the UserScene, which, in turn, register the scene changed through the undo/redo
 * commands. Note, some of the keyboard event are processed through the GLWidget and are not passed to the event
 * handler. However, all the mouses events are meant to be passed and processed to the EventHandler.
*/
class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(GLWidget* widget, RootScene* scene, cher::MOUSE_MODE mode = cher::SELECT_ENTITY);

    /*! A method that handles all the events pass from GLWidget. */
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void setMode(cher::MOUSE_MODE mode);
    cher::MOUSE_MODE getMode() const;

protected:
    /*! This method must not used. It is in development. */
    void doEraseStroke(entity::Stroke *stroke, int first, int last, cher::EVENT event = cher::EVENT_DRAGGED);

    /*! A method to perform a selection  of an entity or a group of entities within a current canvas. */
    void doSelectEntity(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A method to perform canvas selection by using their pickables. */
    template <typename T1, typename T2>
    void doSelectCanvas(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

    /*! Method to process events for stroke sketching: left button push indicates beginning of a stroke, and
     * left button release indicates an end of the stroke. */
    void doSketch(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! Method to process events for polygon drawing: left mouse click indicates a new point, right mouse click
     * or double click indicates polygon completion. */
    void doSketchPolygon(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A method to delete an entity from a current canvas by performing a drag within the entity's proximity. */
    void doDeleteEntity(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A method to perform canvas editing: offset or rotation. */
    void doEditCanvas(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A sub-method for canvas editing to perform offset. */
    void doEditCanvasOffset(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /* A sub-method for canvas editing to perform rotation. */
    void doEditCanvasRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                            osg::Vec3f alongAxis, osg::Vec3f rotAxis);

    /*! A method to create a new canvas by clonning operation. The clonning is done by dragging and locating
     * the new position along a canvas normal. The cloned content does not include entity::Photo.
     * \sa doCanvasSeparate();. */
    void doCanvasClone(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A method to create a new canvas which has the same rotation matrix as the original, but in contrary
     * to the clonning operation, it does not copy any canvas content.
     * \sa doCanvasClone(); */
    void doCanvasSeparate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! This method is in developemt. */
    template <typename T1, typename T2>
    void doEditPhotoPush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A method to move a set of entities within a canvas. */
    void doEditEntitiesMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A method to scale a set of entities within a canvas. */
    void doEditEntitiesScale(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A method to rotate a set of entities within a canvas. */
    void doEditEntitiesRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /* A method for idle mode of SVM (Single View Metrology) related activity. */
    void doIdleMouse(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa);

    /*! A method for hover mode over entity::SVMData. */
    void doHoverWire(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa);

    /*! A method for point hovering over entity::SVMData */
    void doHoverPoint(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa);

    /*! A method for point dragging related to entity::SVMData. */
    void doDragPoint(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa);

protected:
    /*! A method to obtain entity::Stroke type from intersection result. */
    entity::Stroke*     getStroke(const StrokeIntersector::Intersection& result);

    /*! A method to obtain entity::Canvas type from intersection result. */
    entity::Canvas*     getCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);

    /*! A method to obtain entity::Photo type from intersection result. */
    entity::Photo*      getPhoto(const osgUtil::LineSegmentIntersector::Intersection& result);

    /*! A method to obtain entity::Polygon type from intersection result. */
    entity::Polygon*    getPolygon(const osgUtil::LineSegmentIntersector::Intersection& result);

    /*! A method to obtain entity::DraggableWire type from intersection result. */
    entity::DraggableWire* getDraggableWire(const LineIntersector::Intersection& result);

    /*! A method to obtain a selected point index of an entity::DraggableWire type from intersection result. */
    int getSelectedPoint(const PointIntersector::Intersection& result);

    /*! A method to update a mouse mode based on type of geometry caught in an intersection.
     * This method is mainly used when we deal with a entity::SelectedGroup within a canvas. I.e.,
     * a group of geomtries that were selected by a user, and that group is now modifyable by methods of
     * rotation within the canvas, scaling within the canvas, moving within the canvas, etc.
     * \param result is the intersection result, e.g. frame for scaling or geometry for rotation, etc.
     * \param mode_default is the default mode which is returned in case if the intersection results do not
     * contain any known componenents, or if they are empty.
     * \return new mouse mode to set. */
    template <typename T>
    cher::MOUSE_MODE getMouseModeFromName(const T& result, cher::MOUSE_MODE mode_default) const;

    /*! A method to update a provided mouse mode based on the current mouse event. Normally, it is used
     * to determine whether a drag event is happening or not.
     * \param mode is the input mouse mode
     * \return the updated mouse mode (bitwise operator used to update corresponding flags). */
    cher::MOUSE_MODE getMouseModeFromEvent(cher::MOUSE_MODE mode, const osgGA::GUIEventAdapter& ea);

    /*! A template method to obtain intersection given an intersector type as a template.
     * \param mask is used when only certain parts of scene graph should be taken into account by an intersector.
     * For example, when doing a selection of geometries within a canvas, all the other canvas geometries
     * are excluded from the intersector search.
     * \param resultIntersection is the intersection result.
     * \return true if all the steps of intersectors passed successfully. */
    template <typename TypeIntersection, typename TypeIntersector>
    bool getIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, unsigned int mask,
                         TypeIntersection& resultIntersection);

    /*! A convinience method to calculate intersection point between a raytrace and a current canvas
     * virtual plane. The result intersection is returned in canvas local coordinates. As an example,
     * this method is used for sketching on a entity::Canvas surface. The result intersection point
     * (u, v) is passed to a entity::UserScene::addStroke() in order to draw a stroke.
     * \param u is the result local U coordinate,
     * \param v is the result local V coordinate. */
    bool getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                 double& u, double& v);

    /*! A convinience method to obtain a 3D point-intersection between a raytrace and a canvas' normal.
     * \param XC is the returned 3D point
     * \return true if the point XC was found. */
    bool getRaytraceNormalProjection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                     osg::Vec3f &XC);

    /*! A convinience method to obtain a 3D intersection between a raycast and a plane which passes through a given
     * axis. The center of a plane is considered to be a 3D center of a current canvas.
     * \param axis is a vector through which the plane passes.
     * \param P is the returned 3D location.
     * \return true if point P was found successfully. */
    bool getRaytracePlaneIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                      const osg::Vec3f& axis, osg::Vec3f& P);

    /*! A method that defines the mouse mode depending on location of mouse over a canvas frame.
     * Used in entity select, entity move, entity scale, entity rotate, etc. modes.
     * \return true if no need to exit the parent function, false otherwise */
    template <typename TResult, typename TIntersector>
    bool setCanvasMouseMode(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, cher::MOUSE_MODE modeDefault, bool selected = true);

    template <typename TResult, typename TIntersector>
    std::tuple<bool, TResult> setSVMMouseMode(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, cher::MOUSE_MODE modeDefault);

    /*! A method to set colors of canvas frame drawables to:
     * * gray color when mouse is not hovering anything
     * * cyan color over the drawable which gets the hovering. */
    void setDrawableColorFromMode(osg::Drawable* draw);

    void updateWireGeometry(const PolyLineIntersector::Intersection& intersection);
    void updatePointGeometry(const PointIntersector::Intersection& intersection);
    void updateDragPointGeometry(const std::tuple<double, double, bool>& intersection, const osgGA::GUIEventAdapter& ea);

    /*! A method which is called whenever it is necessary to shut the current mode's operation.
     * E.g., for sketching, it would mean forcing a mouse release imitation and completion of a current stroke.
     * This might be necessary for various reasons. The most common one is when the raytracer cannot obtain
     * a 3D intersection with a virtual plane because the current camera view is located too sharp towards the
     * canvas plane where the sketching is going on. Most of the modes that rely on mouse click - drag - release,
     * most likely use this method. */
    void finishAll();

protected:
    GLWidget*                           m_glWidget;     /*!< A pointer on a current GLWidget; used to make sure the mouse mode is set up within GUI (i.e., cursor change). */
    cher::MOUSE_MODE                    m_mode;         /*!< A current mouse mode which dictates what actions are performed based on mouse events. */
    osg::observer_ptr<RootScene>        m_scene;        /*!< A pointer on a scene; ofter used to extract scene elements such as current canvas. */

    osg::observer_ptr<entity::DraggableWire> m_selection;
};

#endif // EVENTHANDLER

