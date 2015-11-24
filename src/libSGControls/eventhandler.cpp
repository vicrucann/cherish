#include "eventhandler.h"
#include <iostream>
#include <osgViewer/View>

EventHandler::EventHandler(dureu::MOUSE_MODE mode):
    _mode(mode)
{
}

bool EventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (_mode == dureu::MOUSE_NAVIGATE)
        return false;
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (viewer){
        switch(_mode){
        default:
            std::cout << "Unrecognized event handler, not processed" << std::endl;
            return false;
        }
    }
    return false;
}

void EventHandler::setMode(dureu::MOUSE_MODE mode)
{
    _mode = mode;
}
