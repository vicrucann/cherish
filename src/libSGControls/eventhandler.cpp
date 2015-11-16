#include "eventhandler.h"
#include <iostream>

EventHandler::EventHandler():
    _mode(dureu::MOUSE_PICK),
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
