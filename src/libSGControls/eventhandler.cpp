#include "eventhandler.h"
#include <iostream>

EventHandler::EventHandler(dureu::MOUSE_MODE mode):
    _mode(mode)
{
}

bool EventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    switch(_mode){
    case dureu::MOUSE_NAVIGATE:
        return true;
    default:
        std::cout << "Unrecognized event handler, not processed" << std::endl;
        return true;
    }
}

void EventHandler::setMode(dureu::MOUSE_MODE mode)
{
    _mode = mode;
}
