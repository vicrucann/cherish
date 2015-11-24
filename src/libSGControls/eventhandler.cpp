#include "eventhandler.h"
#include <iostream>
#include <osgViewer/View>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>

EventHandler::EventHandler(dureu::MOUSE_MODE mode):
    _mode(mode),
    _lastCanvas(0),
    _prevCanvas(0)
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

    std::cout << "  Processing mouse event..." << std::endl;

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (viewer){
        std::cout << "Viewer is read" << std::endl;
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(
                    osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
        osgUtil::IntersectionVisitor iv(intersector);

        osg::Camera* camera = viewer->getCamera();
        if (!camera)
            return false;
        std::cout << "Camera is read" << std::endl;
        camera->accept(iv);
        if (intersector->containsIntersections()){
            std::cout << "Found intersections" << std::endl;
            std::cout << "# of intersections: " << (intersector->getIntersections()).size() << std::endl;
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
    default:
        std::cerr << "Unrecognized event handler, not processed" << std::endl;
        break;
    }
}

// colors when they are deleted?
void EventHandler::doPick(const osgUtil::LineSegmentIntersector::Intersection &result){
    std::cout << "  doPick()" << std::endl;
    if (_lastCanvas.valid()){
        if (_prevCanvas.valid()){
            this->setPrevCanvasColor(dureu::CANVAS_CLR_REST);
            _prevCanvas = NULL;
        }
        // later can include previous color setting
        // but for that need to keep a observer variable on previous node
        this->setLastCanvasColor(dureu::CANVAS_CLR_PREVIOUS);
        _prevCanvas = _lastCanvas;
        _lastCanvas = NULL;
    }
    // now search for parent to retrieve ptr on Canvas
    Canvas* cnv = dynamic_cast<Canvas*>(result.nodePath.at(3));
    if (!cnv){
        std::cerr << "Could not dynamic_cast to Canvas*" << std::endl;
        return;
    }
    std::cout << "assumed canvas with name: " << cnv->getName() << std::endl;
    cnv->setColor(dureu::CANVAS_CLR_CURRENT);
    _lastCanvas = cnv;
}

// check nodepath to see how to go far enough so that to get canvas type
// nodepath; back() returns the very parent
// we need something like back()+c , where c is an arbitrary number of where in SceneRoot
// our canvases are located
void EventHandler::doErase(const osgUtil::LineSegmentIntersector::Intersection &result)
{
    std::cout << "  doErase()" << std::endl;
    std::cout << "node path size: " << result.nodePath.size() << std::endl;
    for (unsigned int i = 0; i < result.nodePath.size(); ++i){
        osg::Node* node = dynamic_cast<osg::Node*>(result.nodePath.at(i));
        std::cout << "#" << i <<", supposed canvas, check name: " << node->getName() << std::endl;
    }
    std::cout << "Trying to delete the canvas" << std::endl;
    osg::Group* parent = dynamic_cast<osg::Group*>(result.nodePath.at(2)); // RootScene
    osg::Node* child = dynamic_cast<osg::Node*>(result.nodePath.at(3)); // Canvas
    if (!parent){
        std::cerr << "Could not retrieve RootScene" << std::endl;
        return;
    }
    bool success = parent->removeChild(child);
    std::cout << "success is " << success << std::endl;
}

void EventHandler::setLastCanvasColor(const osg::Vec4 &color){
    _lastCanvas->setColor(color);
}

void EventHandler::setPrevCanvasColor(const osg::Vec4 &color){
    _prevCanvas->setColor(color);
}
