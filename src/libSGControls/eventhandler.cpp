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

    // Algorithm:
    // use ray-tracking techinique
    // calcualte near and far point in global 3D
    // intersect that segment with plane of canvas - 3D intersection point
    // extract local 3D coords so that to create a stroke (or apprent that point to a current stroke)

    osg::Vec3f nearPoint = osg::Vec3f(x, y, 0.f) * invVPW;
    osg::Vec3f farPoint = osg::Vec3f(x, y, 1.f) * invVPW;

    const osg::Plane plane = _root->getCanvasCurrent()->getPlane();
    const osg::Vec3f center = _root->getCanvasCurrent()->getCenter();
    debugLogVec("doSkech: plane", plane.getNormal().x(), plane.getNormal().y(),plane.getNormal().z());
    debugLogVec("doSketch: center", center.x(), center.y(), center.z());

    assert(plane.valid());
    std::vector<osg::Vec3f> ray(2);
    ray[0] = nearPoint;
    ray[1] = farPoint;
    if (plane.intersect(ray)){ // 1 or -1: no intersection
        std::cerr << "doSketch(): no intersection with the ray" << std::endl;
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
    debugLogVec("doSkecth(): intersect point global 3D", P.x(), P.y(), P.z());

    osg::Matrix M =  _root->getCanvasCurrent()->getTransform()->getMatrix();
    osg::Matrix invM;
    if (!invM.invert(M)){
        std::cerr << "doSketch(): could not invert model matrix" << std::endl;
        return;
    }
    osg::Vec3f p = P * invM;
    debugLogVec("doSketch(): intersect point local 3D", p.x(), p.y(), p.z());
    assert(std::fabs(p.z())<=dureu::EPSILON);

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
    osg::Matrix VPWM = VPW * _root->getCanvasCurrent()->getTransform()->getMatrix();
    osg::Matrix invVPWM;
    assert(invVPWM.invert(VPWM));

    if (!success){
        std::cerr << "doEditOffset(): could not invert View-projection-world matrix for ray casting" << std::endl;
        return;
    }

    // Algorithm:
    // project canvas center (3D local) to the screen
    // how to deal with out of screen projection
    // project normal (3D local) to the screen
    // project mouse coords onto screen normal - get the distance along the normal
    // unproject the obtained distance back to 3D local so that the transform of the canvas changes

    osg::Vec3f C = _root->getCanvasCurrent()->getCenter();
    osg::Vec3f c = C * VPW; // center in screen coords; screen beginning of coord axes: low left corner
    debugLogVec("Center Global", C.x(), C.y(),C.z());
    debugLogVec("Center Screen", c.x(), c.y(), c.z());
    debugLogVec("Center Screen Norm", c.x()/c.z(), c.y()/c.z(), 1);

    osg::Vec3f N = _root->getCanvasCurrent()->getNormal();
    osg::Vec3f Np = C+N;
    osg::Vec3f n = N * VPW;
    osg::Vec3f np = Np * VPW;
    debugLogVec("Normal Global", N.x(), N.y(),N.z());
    debugLogVec("Normal Screen", n.x(), n.y(),n.z());
    debugLogVec("NPoint Screen", np.x(), np.y(),np.z());

    osg::Plane pl = _root->getCanvasCurrent()->getPlane();
    pl.transformProvidingInverse(invVPW);
    debugLogVec("Plane Screen", pl[0], pl[1], pl[2]);
    std::cout << "plane Screen d: " << pl[3] << std::endl;

    if (std::fabs(n.x()) + std::fabs(n.y())<=4.0){
        std::cerr << "The normal is almost perpendicular to the screen, difficult to define the offset from the mouse movements. " <<
                     "To resolve, change the camera position so that to see the normal direction. " << std::endl;
        return;
    }

    double aa=pl[0], bb=pl[1], cc=pl[2], dd=pl[3];
    if (cc == 0){
        std::cerr << "doEditOffset(): Unhandled error: Impossible to obtain z-distance of the screen mouse point." << std::endl;
        return;
    }
    double dz = (-aa*x -bb*y -dd)/cc;
    std::cout << "Z of mouse: " << dz << std::endl;
    osg::Vec3f xyz = osg::Vec3f(x-c.x(), y-c.y(), dz-c.z());
    debugLogVec("xyz vector screen", xyz.x(), xyz.y(), xyz.z());

    //osg::Vec3f xyz = osg::Vec3f(x-c.x(), y-c.y(), c.z());
    float lenS = 0;
    if (n.length()!=0)
        lenS = (xyz * n) / n.length();

    osg::Vec3f p = c + n * lenS; // final screen point which is located on screen normal
    osg::Vec3f P = p * invVPW; // final 3D global point, must be located along the normal

    double len3d = (P-C).length(); // distance in global 3D space where the canvas center will be moved
    std::cout << "global Distance: " << len3d << std::endl;
    osg::Vec3f t = N * len3d; // the translation vector
    debugLogVec("doEditOffset: final translation", t.x(), t.y(), t.z());

    _root->setTransformOffset(t, mouse);
}

Canvas *EventHandler::getCanvas(const osgUtil::LineSegmentIntersector::Intersection &result){
    return dynamic_cast<Canvas*>(result.nodePath.at(_root->getCanvasLevel()));
}
