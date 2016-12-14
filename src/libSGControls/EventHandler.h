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

#include "Settings.h"
#include "Canvas.h"
#include "Photo.h"
#include "UserScene.h"
#include "RootScene.h"
#include "StrokeIntersector.h"
#include "../libGUI/GLWidget.h"

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

    void doEraseStroke(entity::Stroke *stroke, int first, int last, cher::EVENT event = cher::EVENT_DRAGGED);

    void doSelectEntity(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    template <typename T1, typename T2>
    void doSelectCanvas(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

    /*! Method to process events for stroke sketching: left button push indicates beginning of a stroke, and
     * left button release indicates an end of the stroke. */
    void doSketch(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! Method to process events for polygon drawing: left mouse click indicates a new point, right mouse click indicates
     * polygon completion. */
    void doSketchPolygon(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doDeleteEntity(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doEditCanvas(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditCanvasOffset(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditCanvasRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                            osg::Vec3f alongAxis, osg::Vec3f rotAxis);
    void doCanvasClone(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doCanvasSeparate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    template <typename T1, typename T2>
    void doEditPhotoPush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doEditEntitiesMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditEntitiesScale(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditEntitiesRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
    entity::Stroke*     getStroke(const StrokeIntersector::Intersection& result);
    entity::Canvas*     getCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);
    entity::Photo*      getPhoto(const osgUtil::LineSegmentIntersector::Intersection& result);
    entity::Polygon*    getPolygon(const osgUtil::LineSegmentIntersector::Intersection& result);

    template <typename T>
    cher::MOUSE_MODE getMouseMode(const T& result, cher::MOUSE_MODE mode_default) const;

    template <typename TypeIntersection, typename TypeIntersector>
    bool getIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, unsigned int mask,
                         TypeIntersection& resultIntersection);

    bool getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                 double& u, double& v);
    bool getRaytraceNormalProjection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                     osg::Vec3f &XC);
    bool getRaytracePlaneIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                      const osg::Vec3f& axis,
                                      osg::Vec3f& P);

    /*! A method that defines the mouse mode depending on location of mouse over the canvas frame.
     * Used in entity select, entity move, entity scale, entity rotate, etc.
     * \return true if no need to exit the parent function, false otherwise */
    template <typename TResult, typename TIntersector>
    bool setSubMouseMode(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, cher::MOUSE_MODE modeDefault, bool selected = true);

    void setDrawableColorFromMode(osg::Drawable* draw);

    void finishAll();

    GLWidget* m_glWidget;
    cher::MOUSE_MODE m_mode;
    osg::observer_ptr<RootScene> m_scene;
    osg::observer_ptr<entity::Photo> m_photo;
};

#endif // EVENTHANDLER

