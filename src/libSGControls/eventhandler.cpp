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
    /*if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH){
        std::cout << "OSG getButton(): " << ea.getButton() << std::endl;
        std::cout << "OSG push button mask: " << ea.getButtonMask() << std::endl;
    }
    if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE){
        std::cout << "OSG getButton(): " << ea.getButton() << std::endl;
        std::cout << "OSG release button mask: " << ea.getButtonMask() << std::endl;
    }
    if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG){
        std::cout << "OSG getButton(): " << ea.getButton() << std::endl;
        std::cout << "OSG drag button mask: " << ea.getButtonMask() << std::endl;
    }*/

    // if it's mouse navigation mode, don't process event
    // it will be processed by mouse navigator
    if (_mode == dureu::MOUSE_ROTATE || _mode == dureu::MOUSE_PAN ||
            _mode == dureu::MOUSE_ZOOM || _mode == dureu::MOUSE_FIXEDVIEW)
        return false;
    if (_mode == dureu::MOUSE_PICK || _mode == dureu::MOUSE_ERASE)
        doByIntersector(ea, aa);
    else if (_mode == dureu::MOUSE_EDIT_MOVE)
        doByHybrid(ea, aa);
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
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            ))
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
        std::cout << "doByOperator(): push button" << std::endl;
        switch(_mode){
        case dureu::MOUSE_SKETCH:
            doSketch(ea.getX(), ea.getY(), camera, 0);
            break;
        case dureu::MOUSE_EDIT_OFFSET:
            doEditOffset(ea.getX(), ea.getY(), camera, 0);
            break;
        case dureu::MOUSE_EDIT_ROTATE:
            doEditRotate(ea.getX(), ea.getY(), camera, 0);
            break;
        default:
            break;
        }
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        std::cout << "doByOperator(): release button" << std::endl;
        switch(_mode){
        case dureu::MOUSE_SKETCH:
            doSketch(ea.getX(), ea.getY(), camera, 2);
            break;
        case dureu::MOUSE_EDIT_OFFSET:
            doEditOffset(ea.getX(), ea.getY(), camera, 2);
            break;
        case dureu::MOUSE_EDIT_ROTATE:
            doEditRotate(ea.getX(), ea.getY(), camera, 2);
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
        case dureu::MOUSE_EDIT_OFFSET:
            doEditOffset(ea.getX(), ea.getY(), camera, 1);
            break;
        case dureu::MOUSE_EDIT_ROTATE:
            doEditRotate(ea.getX(), ea.getY(), camera, 1);
            break;
        default:
            break;
        }
        break;
    default: // scrolling, doubleclick, move, keydown, keyup, resize
        // frame, pen_pressure, pen_..., ...
        break;
    }
}

// First it uses intersector to select a drawable, if there is
// no current drawable already (no drawable selected already);
// Then it uses operator to find intersection with canvas plane manually;
// For intersector, we can set a mask so that it only selects within current canvas,
// or we may switch the current canvas where the selected drawable belongs to;
void EventHandler::doByHybrid(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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

    // Algorithm:
    // use ray-tracking techinique
    // calcualte near and far point in global 3D
    // intersect that segment with plane of canvas - 3D intersection point
    // extract local 3D coords so that to create a stroke (or apprent that point to a current stroke)

    osg::Vec3f nearPoint = osg::Vec3f(x, y, 0.f) * invVPW;
    osg::Vec3f farPoint = osg::Vec3f(x, y, 1.f) * invVPW;

    const osg::Plane plane = _root->getCanvasCurrent()->getPlane();
    const osg::Vec3f center = _root->getCanvasCurrent()->getCenter();

    assert(plane.valid());
    std::vector<osg::Vec3f> ray(2);
    ray[0] = nearPoint;
    ray[1] = farPoint;
    if (plane.intersect(ray)){ // 1 or -1: no intersection
        std::cerr << "doSketch(): no intersection with the ray." << std::endl;
        // finish the stroke if it was started
        if (_root->getCanvasCurrent()->getStrokeCurrent()){
            std::cout << "doSketch(): finishing the current stroke." << std::endl;
            _root->getCanvasCurrent()->finishStrokeCurrent();
        }
        return;
    }
    osg::Vec3f dir = farPoint-nearPoint;
    if (! plane.dotProductNormal(dir)){ // denominator
        std::cerr << "doSketch(): projected line is parallel to the canvas plane" << std::endl;
        return;
    }
    if (! plane.dotProductNormal(center-nearPoint)){
        std::cerr << "doSketch(): plane contains the line, so no single intersection can be defined" << std::endl;
        return;
    }

    double len = plane.dotProductNormal(center-nearPoint) / plane.dotProductNormal(dir);
    osg::Vec3f P = dir * len + nearPoint;
    //debugLogVec("doSkecth(): intersect point global 3D", P.x(), P.y(), P.z());

    osg::Matrix M =  _root->getCanvasCurrent()->getTransform()->getMatrix();
    osg::Matrix invM;
    if (!invM.invert(M)){
        std::cerr << "doSketch(): could not invert model matrix" << std::endl;
        return;
    }
    osg::Vec3f p = P * invM;
    if (std::fabs(p.z())>dureu::EPSILON){
        std::cerr << "doSketch(): error while projecting point from global 3D to local 3D, z-coordinate is not zero" << std::endl;
        debugLogVec("doSketch: p", p.x(), p.y(), p.z());
        return;
    }

    double u=p.x(), v=p.y();

    _root->getCanvasCurrent()->addStroke(u,v, mouse);
}

// performs offset of the current canvas along its normal
void EventHandler::doEditOffset(int x, int y, const osg::Camera *camera, int mouse)
{
    assert(camera);
    if (!camera->getViewport()){
        std::cerr << "doEditOffset(): could not read viewport" << std::endl;
        return;
    }

    osg::Matrix VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    osg::Matrix invVPW;
    bool success = invVPW.invert(VPW);
    /*osg::Matrix VPWM = VPW * _root->getCanvasCurrent()->getTransform()->getMatrix();
    osg::Matrix invVPWM;
    assert(invVPWM.invert(VPWM));*/

    if (!success){
        std::cerr << "doEditOffset(): could not invert View-projection-world matrix for ray casting" << std::endl;
        return;
    }

    // Algorithm:
    // Cast the ray into 3D space
    // Make sure the ray is not parallel to the normal
    // The new offset point will be located on the projected point
    // between the ray and canvas normal.
    // Ray and normal are skew lines in 3d space, so we only need
    // to extract the projection point of the ray into the normal.

    osg::Vec3f nearPoint = osg::Vec3f(x, y, 0.f) * invVPW;
    osg::Vec3f farPoint = osg::Vec3f(x, y, 1.f) * invVPW;
    osg::Vec3f C = _root->getCanvasCurrent()->getCenter();
    osg::Vec3f N = _root->getCanvasCurrent()->getNormal();

    // algorithm for distance between skew lines:
    //http://www2.washjeff.edu/users/mwoltermann/Dorrie/69.pdf
    // For two points P1 and P2 on skew lines;
    // and d - the direction vector from P1 to P2;
    // u1 and u2 - unit direction vectors for the lines
    osg::Vec3f P1 = C;
    osg::Vec3f P2 = nearPoint;
    osg::Vec3f d = P2 - P1;
    osg::Vec3f u1 = N;
    u1.normalize();
    osg::Vec3f u2 = farPoint - nearPoint;
    u2.normalize();
    osg::Vec3f u3 = u1^u2;

    if (std::fabs(u3.x())<=dureu::EPSILON && std::fabs(u3.y())<=dureu::EPSILON && std::fabs(u3.z())<=dureu::EPSILON){
        std::cerr << "doEditOffset(): cast ray and normal are almost parallel. To resolve, change the camera view." << std::endl;
        return;
    }

    // X1 and X2 are the closest points on lines
    // we want to find X1 (u1 corresponds to normal)
    // solving the linear equation in r1 and r2: Xi = Pi + ri*ui
    // we are only interested in X1 so we only solve for r1.
    float a1 = u1*u1, b1 = u1*u2, c1 = u1*d;
    float a2 = u1*u2, b2 = u2*u2, c2 = u2*d;
    assert((std::fabs(b1) > dureu::EPSILON)); // denominator
    assert(a2!=-1 && a2!=1); // lines are not parallel and we already checked for that
    double r1 = (c2 - b2*c1/b1)/(a2-b2*a1/b1);
    osg::Vec3f X1 = P1 + u1*r1;

    _root->setTransformOffset(X1-C, mouse);
}

void EventHandler::doEditRotate(int x, int y, const osg::Camera *camera, int mouse)
{
    assert(camera);
    if (!camera->getViewport()){
        std::cerr << "doEditOffset(): could not read viewport" << std::endl;
        return;
    }

    osg::Matrix VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    osg::Matrix invVPW;
    bool success = invVPW.invert(VPW);
    if (!success){
        std::cerr << "doEditRotate(): could not invert View-projection-world matrix for ray casting" << std::endl;
        return;
    }

    // Algorithm:
    // Project mouse ray into 3D;
    // Find closest point X2 like in doEditOffset() (we will have to calculate r2 instead);
    // Project vector CX1 so that it is parallel to global X-axis:
    // The projected vector CX1' is a new normal for the canvas.

    osg::Vec3f nearPoint = osg::Vec3f(x, y, 0.f) * invVPW;
    osg::Vec3f farPoint = osg::Vec3f(x, y, 1.f) * invVPW;
    osg::Vec3f C = _root->getCanvasCurrent()->getCenter();
    osg::Vec3f N = _root->getCanvasCurrent()->getNormal();

    osg::Vec3f nearC = nearPoint - C;
    debugLogVec("near-C", nearC.x(), nearC.y(), nearC.z());

    _root->setTransformRotate(N, mouse);
}

Canvas *EventHandler::getCanvas(const osgUtil::LineSegmentIntersector::Intersection &result){
    return dynamic_cast<Canvas*>(result.nodePath.at(_root->getCanvasLevel()));
}
