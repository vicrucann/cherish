#include "eventhandler.h"
#include <iostream>

EventHandler::EventHandler(dureu::MOUSE_MODE mode):
    _mode(mode),
    _FVH(new FixedViewHandler)
{
}

bool EventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    switch(_mode){
    case dureu::MOUSE_NAVIGATE_FIXED:
        return _FVH->handle(ea, aa);
    default:
        std::cout << "Unrecognized event handler, not processed" << std::endl;
        return true;
    }
}

void EventHandler::resetMode(dureu::MOUSE_MODE mode)
{
    _mode = mode;
}
