#include "eventhandler.h"
#include <iostream>
#include <osgViewer/View>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>

EventHandler::EventHandler(RootScene *root, dureu::MOUSE_MODE mode):
    _mode(mode),
    _root(root),
    _x0(-1),
    _y0(-1)
{
}

bool EventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    // if it's mouse navigation mode, don't process event
    // it will be processed by mouse navigator
    if (_mode == dureu::MOUSE_ROTATE || _mode == dureu::MOUSE_PAN ||
            _mode == dureu::MOUSE_ZOOM || _mode == dureu::MOUSE_FIXEDVIEW)
        return false;

    // process only if the event comes from left mouse button
    // otherwise, do nothing
    if (ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return false;

    if (_mode == dureu::MOUSE_SKETCH){ // have to redesin this if-else
        if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
            doSketch(ea.getXnormalized(), ea.getYnormalized());
    }

    else{ // have to redesin this if-else

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (viewer){
        std::cout <<  "handle(): Viewer is read" << std::endl;
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(
                    osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
        osgUtil::IntersectionVisitor iv(intersector);

        osg::Camera* camera = viewer->getCamera();
        if (!camera)
            return false;
        //std::cout <<  "handle(): Camera is read" << std::endl;
        camera->accept(iv);
        if (intersector->containsIntersections()){
            //std::cout << "handle(): # of intersections: " << (intersector->getIntersections()).size() << std::endl;
            const osgUtil::LineSegmentIntersector::Intersection& result = *(intersector->getIntersections().begin());
            doOperation(ea, aa, result);
        }
    }
    } // have to redesin this if-else
    return false;
}

void EventHandler::setMode(dureu::MOUSE_MODE mode)
{
    _mode = mode;
}

void EventHandler::doOperation(const osgGA::GUIEventAdapter &ea,
                               osgGA::GUIActionAdapter &aa,
                               const osgUtil::LineSegmentIntersector::Intersection &result)
{
    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        switch (_mode){
        case dureu::MOUSE_PICK:
            doPick(result);
            break;
        case dureu::MOUSE_ERASE:
            doErase(result);
            break;
        case dureu::MOUSE_SKETCH: // this is only for test purposes, have to redefine behaviour
            doSketch(ea.getX(), ea.getY());
            break;
        default:
            _x0 = -1;
            _y0 = -1;
            break;
        }
    case osgGA::GUIEventAdapter::DRAG:
        if (_x0 > 0 && _y0>0){
            switch (_mode){
            case dureu::MOUSE_EDIT:
                doEdit(result, ea.getX(), ea.getY());
                _x0 = ea.getX();
                _y0 = ea.getY();
                break;
            case dureu::MOUSE_SKETCH:
                //doSketch(result, ea.getX(), ea.getY()); // redefine
                _x0 = ea.getX();
                _y0 = ea.getY();
                break;
            default:
                _x0 = ea.getX();
                _y0 = ea.getY();
                break;
            }
        }
        break;
    default:
        // scrolling, doubleclick, move, keydown, keyup, resize
        // frame, pen_pressure, pen_..., ...
        break;
    }
}

void EventHandler::doPick(const osgUtil::LineSegmentIntersector::Intersection &result){
    Canvas* cnv = getCanvas(result);
    if (!cnv){
        std::cerr << "doPick(): could not dynamic_cast<Canvas*>" << std::endl;
        return;
    }
    std::cout << "doPick(): assumed canvas with name: " << cnv->getName() << std::endl;
    _root->setCanvasCurrent(cnv);
}

// check nodepath to see how to go far enough so that to get canvas type
void EventHandler::doErase(const osgUtil::LineSegmentIntersector::Intersection &result)
{
    Canvas* cnv = getCanvas(result);
    if (!cnv){
        std::cerr << "doErase(): could not dynamic_cast<Canvas*>" << std::endl;
        return;
    }
    std::cout << "doErase(): assumed canvas with name: " << cnv->getName() << std::endl;
    bool success = _root->deleteCanvas(cnv);
    std::cout << "doErase(): success is " << success << std::endl;
}

void EventHandler::doSketch(double x, double y)
{
    std::cout << "doSketch()" << std::endl;
    _root->addStroke(x,y);
}

void EventHandler::doEdit(const osgUtil::LineSegmentIntersector::Intersection &result, double x, double y)
{
    std::cout << "doEdit()" << std::endl;
}

Canvas *EventHandler::getCanvas(const osgUtil::LineSegmentIntersector::Intersection &result){
    return dynamic_cast<Canvas*>(result.nodePath.at(_root->getCanvasLevel()));
}
