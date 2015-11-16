#ifndef EVENTHANDLER
#define EVENTHANDLER

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/ref_ptr>

#include "settings.h"
#include "fixedviewhandler.h"

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler();
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
private:
    dureu::MOUSE_MODE _mode;
    osg::ref_ptr<FixedViewHandler> _FVH;
};

#endif // EVENTHANDLER

