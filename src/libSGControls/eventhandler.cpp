#include "eventhandler.h"
#include <iostream>
#include <osgViewer/View>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>

EventHandler::EventHandler(dureu::MOUSE_MODE mode):
    _mode(mode)
{
}

bool EventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (_mode == dureu::MOUSE_ROTATE || _mode == dureu::MOUSE_PAN ||
            _mode == dureu::MOUSE_ZOOM || _mode == dureu::MOUSE_FIXEDVIEW)
        return false;

    // for the moment we proceed only if it is release() and left
    // mouse button was pressed
    if (ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE ||
            ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return false;

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (viewer){
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(
                    osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
        osgUtil::IntersectionVisitor iv(intersector);

        osg::Camera* camera = viewer->getCamera();
        if (!camera)
            return false;
        camera->accept(iv);
        if (intersector->containsIntersections()){
            const osgUtil::LineSegmentIntersector::Intersection& result = *(intersector->getIntersections().begin());
            doOperation(result);
        }
    }
    return false;
}

void EventHandler::setMode(dureu::MOUSE_MODE mode)
{
    _mode = mode;
}

void EventHandler::doOperation(const osgUtil::LineSegmentIntersector::Intersection &intersections)
{
    switch(_mode){
    case dureu::MOUSE_PICK:
        doPick(intersections);
    default:
        std::cerr << "Unrecognized event handler, not processed" << std::endl;
    }
}

void EventHandler::doPick(const osgUtil::LineSegmentIntersector::Intersection &intersections){
}
