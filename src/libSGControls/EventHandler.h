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

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(RootScene* scene, dureu::MOUSE_MODE mode = dureu::MOUSE_SELECT);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void setMode(dureu::MOUSE_MODE mode);

    template <typename T1, typename T2>
    void doByLineIntersector(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doByRaytrace(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doByHybrid(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doPickStroke(const StrokeIntersector::Intersection& result);
    void doPickCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);

    void doDelete(const osgUtil::LineSegmentIntersector::Intersection& result);

    void doEraseStroke(const StrokeIntersector::Intersection& result, dureu::EVENT event = dureu::EVENT_DRAGGED);

    void doSketch(double u, double v, dureu::EVENT event);

    void doEditCanvasOffset(osg::Vec3f XC, dureu::EVENT event);
    void doEditCanvasRotate(int x, int y, dureu::EVENT event);
    void doEditPhotoMove(const osgUtil::LineSegmentIntersector::Intersection& result,
                            double u, double v, dureu::EVENT event);

protected:
    entity::Stroke* getStroke(const StrokeIntersector::Intersection& result);
    entity::Canvas* getCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);
    entity::Photo* getPhoto(const osgUtil::LineSegmentIntersector::Intersection& result);

    template <typename T1, typename T2>
    bool getLineIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, T1& result);

    bool getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                 double& u, double& v);
    bool getRaytraceNormalProjection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                     osg::Vec3f &XC);

    void finishAll();

    dureu::MOUSE_MODE m_mode;
    osg::observer_ptr<RootScene> m_scene;
};

#endif // EVENTHANDLER

