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

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(dureu::MOUSE_MODE mode = dureu::MOUSE_PICK);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void setMode(dureu::MOUSE_MODE mode);

    virtual void doOperation(const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doPick(const osgUtil::LineSegmentIntersector::Intersection& result);
    virtual void doErase(const osgUtil::LineSegmentIntersector::Intersection& result);

    void setLastCanvasColor(const osg::Vec4& color);
    void setPrevCanvasColor(const osg::Vec4& color);
protected:
    dureu::MOUSE_MODE _mode;
    osg::observer_ptr<Canvas> _lastCanvas;
    osg::observer_ptr<Canvas> _prevCanvas;
private:
    // test variables
};

#endif // EVENTHANDLER

