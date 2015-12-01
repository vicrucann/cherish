#include "eventhandler.h"
#include <iostream>
#include <osgViewer/View>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>

EventHandler::EventHandler(RootScene *root, dureu::MOUSE_MODE mode):
    _mode(mode),
    _root(root)
{
}

bool EventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (_mode == dureu::MOUSE_ROTATE || _mode == dureu::MOUSE_PAN ||
            _mode == dureu::MOUSE_ZOOM || _mode == dureu::MOUSE_FIXEDVIEW)
        return false;

    // for the moment we proceed only if it is release() and left
    // mouse button was pressed
    if (ea.getEventType()!=osgGA::GUIEventAdapter::PUSH ||
            ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return false;

    std::cout << "handle(): Processing mouse event..." << std::endl;

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (viewer){
        std::cout <<  "handle(): Viewer is read" << std::endl;
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(
                    osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
        osgUtil::IntersectionVisitor iv(intersector);

        osg::Camera* camera = viewer->getCamera();
        if (!camera)
            return false;
        std::cout <<  "handle(): Camera is read" << std::endl;
        camera->accept(iv);
        if (intersector->containsIntersections()){
            std::cout << "handle(): # of intersections: " << (intersector->getIntersections()).size() << std::endl;
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

void EventHandler::doOperation(const osgUtil::LineSegmentIntersector::Intersection &result)
{
    switch(_mode){
    case dureu::MOUSE_PICK:
        doPick(result);
        break;
    case dureu::MOUSE_ERASE:
        doErase(result);
        break;
    case dureu::MOUSE_SKETCH:
        doSketch(result);
        break;
    default:
        std::cerr << "doOperation(): unrecognized event handler, not processed" << std::endl;
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

void EventHandler::doSketch(const osgUtil::LineSegmentIntersector::Intersection &result)
{
    std::cout << "  doSketch()" << std::endl;
}

Canvas *EventHandler::getCanvas(const osgUtil::LineSegmentIntersector::Intersection &result){
    return dynamic_cast<Canvas*>(result.nodePath.at(_root->getCanvasLevel()));
}
