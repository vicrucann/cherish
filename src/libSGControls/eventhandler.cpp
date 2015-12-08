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

    // process only if the event comes from left mouse button
    // otherwise, do nothing
    if (ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return false;

    if (_mode == dureu::MOUSE_SKETCH){ // have to redesin this if-else
        if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
            doSketch(ea, aa);
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
            //doSketch(ea.getX(), ea.getY());
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

// see http://www.mvps.org/directx/articles/rayproj.htm
// for algorithmnic details
void EventHandler::doSketch(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    std::cout << "doSketch()" << std::endl;

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        std::cout << "doSketch(): could not read viewer" << std::endl;
        return;
    }

    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(
                osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
    osgUtil::IntersectionVisitor iv(intersector);

    osg::Camera* camera = viewer->getCamera();
    if (!camera){
        std::cout << "doSketch(): could not read camera" << std::endl;
        return;
    }

    camera->accept(iv);
    if (!intersector->containsIntersections()){
        std::cout << "doSketch(): no intersections found" << std::endl;
        return;
    }

    const osgUtil::LineSegmentIntersector::Intersection& result = *(intersector->getIntersections().begin());
    // check against the ground truth
    osg::Vec3f wpGT = result.getWorldIntersectPoint();
    osg::Vec3f wnGT = result.getWorldIntersectNormal();
    osg::Vec3f lpGT = result.getLocalIntersectPoint();
    osg::Vec3f lnGT = result.getLocalIntersectNormal();
    std::cout << "World point: " << wpGT.x() << " " << wpGT.y() << " " << wpGT.z() << std::endl;
    std::cout << "World normal: " << wnGT.x() << " " << wnGT.y() << " " << wnGT.z() << std::endl;
    std::cout << "Local point: " << lpGT.x() << " " << lpGT.y() << " " << lpGT.z() << std::endl;
    std::cout << "Local normal: " << lnGT.x() << " " << lnGT.y() << " " << lnGT.z() << std::endl;

    osg::Matrix VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    osg::Matrix MVPW =  _root->getCanvasCurrent()->getTransform()->getMatrix() * VPW;
    osg::Matrix invMVPW;
    osg::Matrix invVPW;
    assert(invMVPW.invert(MVPW));
    assert(invVPW.invert(VPW));

    // the coords are already normalized
    double dx = ea.getX();
    double dy = ea.getY();
    std::cout << "coords: " << dx << " " << dy << std::endl;

    // end points of ray
    //dx = 1.1f;
    //dy = 0.9f;
    osg::Vec3f nearPoint = osg::Vec3f(dx, dy, 0.f)*invVPW;
    osg::Vec3f farPoint = osg::Vec3f(dx, dy, 1.f)*invVPW;
    osg::Vec3f norm = farPoint-nearPoint;
    norm.normalize();
    std::cout << "nearPoint: " << nearPoint[0] << " " << nearPoint[1] << " " << nearPoint[2] << std::endl;
    std::cout << "farPoint: " << farPoint[0] << " " << farPoint[1] << " " << farPoint[2] << std::endl;
    std::cout << "normal: " << norm[0] << " " << norm[1] << " " << norm[2] << std::endl;

    osg::Vec3f eye,center,up;
    camera->getViewMatrixAsLookAt(eye, center, up);
    std::cout << "center: " << center[0] << " " << center[1] << " " << center[2] << std::endl;
    osg::Vec3f dir = center - eye;
    dir.normalize();
    std::cout << "dir: " << dir[0] << " " << dir[1] << " " << dir[2] << std::endl;
    osg::Vec3f side = dir^up;
    side.normalize();
    std::cout << "side: " << side[0] << " " << side[1] << " " << side[2] << std::endl;
    up.normalize();
    std::cout << "up: " << up[0] << " " << up[1] << " " << up[2] << std::endl;

    // eye position of where the mouse is clicked
    /*double distance = eye.length();
    std::cout << "distance: " << distance << std::endl;
    double scaleX = std::tan(dureu::PI/6)*distance; // tan(30 deg), subject to replace, depends on project matrix
    double scaleY = std::tan(dureu::PI/6)*distance;
    osg::Vec3f eye_new = eye + side * ea.getXnormalized() * scaleX + up * ea.getYnormalized() * scaleY;
    std::cout << "old eye: " << eye[0] << " " << eye[1] << " " << eye[2] << std::endl;
    std::cout << "new eye: " << eye_new[0] << " " << eye_new[1] << " " << eye_new[2] << std::endl;
    std::cout << "direction: " << dir.x() << " " << dir.y() << " " << dir.z() << std::endl;*/

    _root->addStroke(nearPoint,farPoint);
}

void EventHandler::doEdit(const osgUtil::LineSegmentIntersector::Intersection &result, double x, double y)
{
    std::cout << "doEdit()" << std::endl;
}

Canvas *EventHandler::getCanvas(const osgUtil::LineSegmentIntersector::Intersection &result){
    return dynamic_cast<Canvas*>(result.nodePath.at(_root->getCanvasLevel()));
}
