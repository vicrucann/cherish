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
#include "canvas.h"
#include "photo.h"
#include "rootscene.h"

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(RootScene* root, dureu::MOUSE_MODE mode = dureu::MOUSE_PICK);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void setMode(dureu::MOUSE_MODE mode);

    virtual void doByLineIntersector(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    virtual void doByRaytrace(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    virtual void doByHybrid(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    //virtual void doOperation(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
    //                         const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doPick(const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doErase(const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doSketch(double u, double v, int mouse = 1);
    virtual void doEditOffset(osg::Vec3f XC, int mouse = 1);
    virtual void doEditRotate(int x, int y, const osg::Camera* camera, int mouse = 1);
    virtual void doEditMove(const osgUtil::LineSegmentIntersector::Intersection& result,
                            double u, double v, int mouse = 1);

protected:
    Canvas* getCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);
    Photo* getPhoto(const osgUtil::LineSegmentIntersector::Intersection& result);

    bool getLineIntersections(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                              osgUtil::LineSegmentIntersector::Intersection& result);
    bool getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                 double& u, double& v);
    bool getRaytraceNormalProjection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                     osg::Vec3f &XC);

    dureu::MOUSE_MODE _mode;
    osg::observer_ptr<RootScene> _root;
private:
    double _x0,_y0; // used for edit and sketch modes
};

#endif // EVENTHANDLER

