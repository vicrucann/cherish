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

#include "settings.h"
#include "fixedviewhandler.h"

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(dureu::MOUSE_MODE mode = dureu::MOUSE_PICK);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void resetMode(dureu::MOUSE_MODE mode);
private:
    dureu::MOUSE_MODE _mode;
    osg::ref_ptr<FixedViewHandler> _FVH;
};

#endif // EVENTHANDLER

