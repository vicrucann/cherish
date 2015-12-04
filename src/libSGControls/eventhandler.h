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
#include "rootscene.h"

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(RootScene* root, dureu::MOUSE_MODE mode = dureu::MOUSE_PICK);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void setMode(dureu::MOUSE_MODE mode);

    virtual void doOperation(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                             const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doPick(const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doErase(const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doSketch(double x, double y);
    virtual void doEdit(const osgUtil::LineSegmentIntersector::Intersection& result,
                        double x, double y);

protected:
    Canvas* getCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);

    dureu::MOUSE_MODE _mode;
    osg::observer_ptr<RootScene> _root;
private:
    double _x0,_y0; // used for edit and sketch modes
};

#endif // EVENTHANDLER

