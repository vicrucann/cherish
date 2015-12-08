#include "eventhandler.h"
#include <iostream>
#include <assert.h>
#include <osgViewer/View>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
#include <osg/Viewport>

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
    if (_mode == dureu::MOUSE_PICK || _mode == dureu::MOUSE_ERASE)
        doByIntersector(ea, aa);
    else
        doByOperator(ea, aa);
    return false;
}

void EventHandler::setMode(dureu::MOUSE_MODE mode)
{
    _mode = mode;
}

// for pick and erase when lineintersector is going to be used
void EventHandler::doByIntersector(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE ||
            ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return;
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        std::cerr << "doByIntersector(): could not retrieve viewer" << std::endl;
        return;
    }
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(
                osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

    osgUtil::IntersectionVisitor iv(intersector);
    osg::Camera* camera = viewer->getCamera();
    if (!camera){
        std::cerr << "doByIntersector(): could not read camera" << std::endl;
        return;
    }
    camera->accept(iv);
    if (!intersector->containsIntersections()){
        std::cerr << "doByIntersector(): no intersections found" << std::endl;
        return;
    }
    const osgUtil::LineSegmentIntersector::Intersection& result = *(intersector->getIntersections().begin());

    switch (_mode) {
    case dureu::MOUSE_PICK:
        doPick(result);
        break;
    case dureu::MOUSE_ERASE:
        doErase(result);
        break;
    default:
        break;
    }
}

void EventHandler::doByOperator(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if ((ea.getEventType() != osgGA::GUIEventAdapter::RELEASE
         || ea.getEventType() != osgGA::GUIEventAdapter::PUSH
         || ea.getEventType() != osgGA::GUIEventAdapter::DRAG )
            && ea.getButtonMask() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return;


    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        std::cerr << "doByOperator(): could not read viewer" << std::endl;
        return;
    }

    osg::Camera* camera = viewer->getCamera();
    if (!camera){
        std::cout << "doByOperator(): could not read camera" << std::endl;
        return;
    }

    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        switch(_mode){
        case dureu::MOUSE_SKETCH:
            doSketch(ea.getX(), ea.getY(), camera, 0);
            break;
        default:
            break;
        }
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        switch(_mode){
        case dureu::MOUSE_SKETCH:
            doSketch(ea.getX(), ea.getY(), camera, 2);
            break;
        default:
            break;
        }
        break;
    case osgGA::GUIEventAdapter::DRAG:
        switch(_mode){
        case dureu::MOUSE_SKETCH:
            doSketch(ea.getX(), ea.getY(), camera, 1);
            break;
        default:
            break;
        }
        break;
    /*case osgGA::GUIEventAdapter::MOVE:
        std::cout << "doByOperator(): move" << std::endl;
        break;
    case osgGA::GUIEventAdapter::FRAME:
        std::cout << "doByOperator(): frame" << std::endl;
        break;*/
    default: // scrolling, doubleclick, move, keydown, keyup, resize
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

// see https://www.opengl.org/sdk/docs/man2/xhtml/gluUnProject.xml
// and https://www.mail-archive.com/osg-users@openscenegraph.net/msg16244.html
// for more details
// mouse is to indicate if the stroke is just created (0), in the process of drawing (1)
// or finished drawing (2)
void EventHandler::doSketch(int x, int y, const osg::Camera *camera, int mouse)
{
    assert(camera);
    if (!camera->getViewport()){
        std::cerr << "doSketch(): could not read viewport" << std::endl;
        return;
    }

    osg::Matrix VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    osg::Matrix invVPW;
    bool success = invVPW.invert(VPW);
    if (!success){
        std::cerr << "doSketch(): could not invert View-projection-world matrix for ray casting" << std::endl;
        return;
    }

    osg::Vec3f nearPoint = osg::Vec3f(x, y, 0.f) * invVPW;
    osg::Vec3f farPoint = osg::Vec3f(x, y, 1.f) * invVPW;

    _root->addStroke(nearPoint,farPoint, mouse);
}

void EventHandler::doEdit(const osgUtil::LineSegmentIntersector::Intersection &result, double x, double y)
{
    std::cout << "doEdit()" << std::endl;
}

Canvas *EventHandler::getCanvas(const osgUtil::LineSegmentIntersector::Intersection &result){
    return dynamic_cast<Canvas*>(result.nodePath.at(_root->getCanvasLevel()));
}
