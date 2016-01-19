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

#include "settings.h"
#include "Canvas.h"
#include "Photo.h"
#include "UserScene.h"
#include "RootScene.h"

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(RootScene* scene, dureu::MOUSE_MODE mode = dureu::MOUSE_PICK);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void setMode(dureu::MOUSE_MODE mode);

    virtual void doByLineIntersector(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    virtual void doByRaytrace(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    virtual void doByHybrid(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    virtual void doPick(const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doDelete(const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doErase(double u, double v, int mouse = 1);
    virtual void doSketch(double u, double v, dureu::EVENT event);
    virtual void doEditCanvasOffset(osg::Vec3f XC, int mouse = 1);
    virtual void doEditCanvasRotate(int x, int y, int mouse = 1);
    virtual void doEditPhotoMove(const osgUtil::LineSegmentIntersector::Intersection& result,
                            double u, double v, int mouse = 1);

protected:
    entity::Canvas* getCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);
    entity::Photo* getPhoto(const osgUtil::LineSegmentIntersector::Intersection& result);

    bool getLineIntersections(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                              osgUtil::LineSegmentIntersector::Intersection& result);
    bool getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                 double& u, double& v);
    bool getRaytraceNormalProjection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                     osg::Vec3f &XC);

    dureu::MOUSE_MODE mMode;
    osg::observer_ptr<RootScene> m_scene;

private:
};

#endif // EVENTHANDLER

