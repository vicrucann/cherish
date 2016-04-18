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

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(GLWidget* widget, RootScene* scene, dureu::MOUSE_MODE mode = dureu::SELECT_ENTITY);

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void setMode(dureu::MOUSE_MODE mode);
    dureu::MOUSE_MODE getMode() const;

    void doEraseStroke(entity::Stroke *stroke, int first, int last, dureu::EVENT event = dureu::EVENT_DRAGGED);

    void doSelectEntity(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    template <typename T1, typename T2>
    void doSelectCanvas(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

    void doSketch(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doDeleteEntity(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doEditCanvas(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditCanvasOffset(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditCanvasRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, bool alongV = true);
    void doCanvasClone(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    template <typename T1, typename T2>
    void doEditPhotoPush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doEditEntitiesMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditEntitiesScale(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditEntitiesRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
    entity::Stroke* getStroke(const StrokeIntersector::Intersection& result);
    entity::Canvas* getCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);
    entity::Photo* getPhoto(const osgUtil::LineSegmentIntersector::Intersection& result);

    template <typename T>
    dureu::MOUSE_MODE getMouseMode(const T& result, dureu::MOUSE_MODE mode_default) const;

    template <typename T1, typename T2>
    bool getLineIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, unsigned int mask, T1& result);

    bool getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                 double& u, double& v);
    bool getRaytraceNormalProjection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                     osg::Vec3f &XC);
    bool getRaytracePlaneIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                      const osg::Vec3f& axis,
                                      osg::Vec3f& P);

    template <typename TResult, typename TIntersector>
    bool setSubMouseMode(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, dureu::MOUSE_MODE modeDefault, bool selected = true);

    void setDrawableColorFromMode(osg::Drawable* draw);

    void finishAll();

    GLWidget* m_glWidget;
    dureu::MOUSE_MODE m_mode;
    osg::observer_ptr<RootScene> m_scene;
    osg::observer_ptr<entity::Photo> m_photo;
};

#endif // EVENTHANDLER

