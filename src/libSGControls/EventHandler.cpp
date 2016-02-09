#include "EventHandler.h"
#include <iostream>
#include <assert.h>
#include <osgViewer/View>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
#include <osg/Viewport>

EventHandler::EventHandler(RootScene* scene, dureu::MOUSE_MODE mode)
    : osgGA::GUIEventHandler()
    , m_mode(mode)
    , m_scene(scene)
    , m_photo(0)
{
}

// handle() has to be re-defined, for more info, check
// OpenSceneGraph beginner's guide or
// OpenSceneGraph 3.0 Cookbook
// and search for custom event handler examples
bool EventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{    
    /* if it's mouse navigation mode, don't process event
     * it will be processed by Manipulator */

    if (m_mode == dureu::MOUSE_ORBIT || m_mode == dureu::MOUSE_PAN ||
            m_mode == dureu::MOUSE_ZOOM || m_mode == dureu::MOUSE_FIXEDVIEW)
        return false;

    if (! m_scene->getCanvasCurrent())
        return false;

    switch (m_mode){
    case dureu::MOUSE_SELECT:
        if (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL)
            this->doSelectStroke<StrokeIntersector::Intersection, StrokeIntersector>(ea, aa);
        else
            this->doSelectEntity<osgUtil::LineSegmentIntersector::Intersection, osgUtil::LineSegmentIntersector>(ea, aa);
        break;
    case dureu::MOUSE_SKETCH:
        this->doSketch(ea, aa);
        break;
    case dureu::MOUSE_DELETE:
        if (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL)
            this->doDeleteEntity(ea, aa);
        else
            this->doDeleteCanvas<osgUtil::LineSegmentIntersector::Intersection, osgUtil::LineSegmentIntersector>(ea, aa);
        break;
    case dureu::MOUSE_CANVAS_OFFSET:
        this->doEditCanvasOffset(ea, aa);
        break;
    case dureu::MOUSE_CANVAS_ROTATE:
        this->doEditCanvasRotate(ea, aa);
        break;
    case dureu::MOUSE_CANVAS_CLONE:
        this->doCanvasClone(ea, aa);
        break;
    case dureu::MOUSE_PHOTO_FLIPH:
        this->doEditPhotoFlip<osgUtil::LineSegmentIntersector::Intersection, osgUtil::LineSegmentIntersector>(ea, aa, true);
        break;
    case dureu::MOUSE_PHOTO_FLIPV:
        this->doEditPhotoFlip<osgUtil::LineSegmentIntersector::Intersection, osgUtil::LineSegmentIntersector>(ea, aa, false);
        break;
    case dureu::MOUSE_PHOTO_MOVE:
        this->doEditPhotoMove<osgUtil::LineSegmentIntersector::Intersection, osgUtil::LineSegmentIntersector>(ea, aa);
        break;
    case dureu::MOUSE_PHOTO_SCALE:
        this->doEditPhotoScale<osgUtil::LineSegmentIntersector::Intersection, osgUtil::LineSegmentIntersector>(ea, aa);
        break;
    case dureu::MOUSE_PHOTO_ROTATE:
        this->doEditPhotoRotate<osgUtil::LineSegmentIntersector::Intersection, osgUtil::LineSegmentIntersector>(ea, aa);
        break;
    default:
        break;
    }
    return false;
}

void EventHandler::setMode(dureu::MOUSE_MODE mode)
{
    m_mode = mode;
    switch (m_mode){
    case dureu::MOUSE_PHOTO_ROTATE:
    case dureu::MOUSE_PHOTO_SCALE:
    case dureu::MOUSE_PHOTO_FLIPH:
    case dureu::MOUSE_PHOTO_FLIPV:
    case dureu::MOUSE_PHOTO_MOVE:
        m_scene->setCanvasesButCurrent(false);
        break;
    default:
        m_scene->setCanvasesButCurrent(true);
        break;
    }
}

dureu::MOUSE_MODE EventHandler::getMode() const
{
    return m_mode;
}

/* Algorithm:
 * Get closest line segment out of Stroke
 * Pass that line segment to split stroke
 * That line segment will be deleted from stroke, and,
 * depending on its location inside the stroke, it will either
 * split the stroke, or it will erase one of its ends.
 * When stroke is split, need to see if both substrokes are long
 * enough to continue to exist.
*/
void EventHandler::doEraseStroke(entity::Stroke* stroke, int first, int last, dureu::EVENT event)
{
    /* THIS IS A DEBUG VERSION !!!!!!*/
    if (!stroke){
        outErrMsg("doEraseStroke: could not obtain stroke");
        return;
    }
    m_scene->eraseStroke(stroke, first, last, event);
}

void EventHandler::doSketch(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           ))
        return;

    double u=0, v=0;
    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
            return;
        m_scene->addStroke(u, v, dureu::EVENT_PRESSED);
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
            return;
        m_scene->addStroke(u,v, dureu::EVENT_RELEASED);
        break;
    case osgGA::GUIEventAdapter::DRAG:
        if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
            return;
        m_scene->addStroke(u,v,dureu::EVENT_DRAGGED);
        break;
    default:
        break;
    }
}

void EventHandler::doDeleteEntity(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{

}

template <typename T1, typename T2>
void EventHandler::doDeleteCanvas(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE || ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return;
    outLogMsg("doDeleteCanvas called");

    T1* result = new T1;
    bool intersected = this->getLineIntersection<T1, T2>(ea,aa, *result);
    if (!intersected)
        return;

    entity::Canvas* cnv = this->getCanvas(*result);
    if (!cnv){
        std::cerr << "doDeleteCanvas(): could not dynamic_cast<Canvas*>" << std::endl;
        return;
    }
    m_scene->editCanvasDelete(cnv);
}

void EventHandler::doEditCanvasOffset(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           ))
        return;

    osg::Vec3f XC = osg::Vec3f(0.f,0.f,0.f);
    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        if (!this->getRaytraceNormalProjection(ea,aa,XC))
            return;
        m_scene->editCanvasOffset(XC, dureu::EVENT_PRESSED);
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        if (!this->getRaytraceNormalProjection(ea,aa,XC))
            return;
        m_scene->editCanvasOffset(XC, dureu::EVENT_RELEASED);
        break;
    case osgGA::GUIEventAdapter::DRAG:
        if (!this->getRaytraceNormalProjection(ea,aa,XC))
            return;
        m_scene->editCanvasOffset(XC, dureu::EVENT_DRAGGED);
        break;
    default:
        break;
    }
}

void EventHandler::doEditCanvasRotate(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           ))
        return;

    //osg::Vec3f XC = osg::Vec3f(0.f,0.f,0.f);
    osg::Quat rot(dureu::PI/24, osg::Vec3f(0,0,1));

    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        //if (!this->getRaytraceNormalProjection(ea,aa,XC)) return;
        m_scene->editCanvasRotate(rot, dureu::EVENT_PRESSED);
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        //if (!this->getRaytraceNormalProjection(ea,aa,XC)) return;
        m_scene->editCanvasRotate(rot, dureu::EVENT_RELEASED);
        break;
    case osgGA::GUIEventAdapter::DRAG:
        //if (!this->getRaytraceNormalProjection(ea,aa,XC))return;
        m_scene->editCanvasRotate(rot, dureu::EVENT_DRAGGED);
        break;
    default:
        break;
    }
}

void EventHandler::doCanvasClone(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           ))
        return;

    osg::Vec3f XC = osg::Vec3f(0.f,0.f,0.f);
    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        if (!this->getRaytraceNormalProjection(ea,aa,XC)) return;
        m_scene->editCanvasClone(XC, dureu::EVENT_PRESSED);
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        if (!this->getRaytraceNormalProjection(ea,aa,XC))
            this->finishAll();
        m_scene->editCanvasClone(XC, dureu::EVENT_RELEASED);
        break;
    case osgGA::GUIEventAdapter::DRAG:
        if (!this->getRaytraceNormalProjection(ea,aa,XC))
            this->finishAll();
        m_scene->editCanvasClone(XC, dureu::EVENT_DRAGGED);
        break;
    default:
        break;
    }
}

/* Pick photo, obtain canvas that contains that photo;
 * Make that canvas current;
 * Change photo coordinates to [x, y];
 * Change photo colors when pushed (edit color) and released (no color)
*/
template <typename T1, typename T2>
void EventHandler::doEditPhotoMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           ))
        return;

    /* disable non-current canvases, set their traversals to none */

    T1* result = new T1;
    bool intersects = this->getLineIntersection<T1, T2>(ea,aa, *result);

    /* pick photo to track */
    if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH){
        if (!intersects)
            return;
        m_photo = this->getPhoto(*result);
    }

    if (!m_photo){
        std::cerr << "doEditPhotoMove(): no selected photo" << std::endl;
        return;
    }

    double u=0, v=0;
    if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
        return;

    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        std::cout << "edit photo move: push button" << std::endl;
        m_scene->editPhotoMove(m_photo.get(), u, v, dureu::EVENT_PRESSED);
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        std::cout << "edit photo move: release button" << std::endl;
        m_scene->editPhotoMove(m_photo.get(), u, v, dureu::EVENT_RELEASED);
        this->finishAll();
        break;
    case osgGA::GUIEventAdapter::DRAG:
        m_scene->editPhotoMove(m_photo.get(), u, v, dureu::EVENT_DRAGGED);
        break;
    default:
        break;
    }
}

template <typename T1, typename T2>
void EventHandler::doEditPhotoFlip(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, bool horizontal)
{
    if (ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE || ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return;

    T1* result = new T1;
    if (!this->getLineIntersection<T1, T2>(ea,aa, *result))
        return;

    entity::Photo* photo = getPhoto(*result);
    if (!photo){
        std::cerr << "doEditPhotoMove(): could not dynamic_cast<Photo*>" << std::endl;
        return;
    }

    entity::Canvas* cnv = getCanvas(*result);
    if (!cnv){
        std::cerr << "doEditPhotoMove(): could not dynamic_cast<Canvas*>" << std::endl;
        return;
    }

    m_scene->setCanvasCurrent(cnv); /* subjec to change : only track photos within current canvas */
    m_scene->editPhotoFlip(photo, horizontal);
}

entity::Stroke *EventHandler::getStroke(const StrokeIntersector::Intersection &result)
{
    return dynamic_cast<entity::Stroke*>(result.drawable.get());
}

entity::Canvas *EventHandler::getCanvas(const osgUtil::LineSegmentIntersector::Intersection &result){
    if (result.drawable->getName() != "Pickable")
        return NULL;
    return dynamic_cast<entity::Canvas*>(result.nodePath.at(m_scene->getCanvasLevel()));
}

entity::Photo *EventHandler::getPhoto(const osgUtil::LineSegmentIntersector::Intersection &result)
{
    return dynamic_cast<entity::Photo*>(result.drawable.get());
}

template <typename T1, typename T2>
bool EventHandler::getLineIntersection(const osgGA::GUIEventAdapter &ea,
                                        osgGA::GUIActionAdapter &aa,
                                        T1& result)
{
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        outErrMsg("getLineIntersection(): could not retrieve viewer");
        return false;
    }
    osg::ref_ptr<T2> intersector = new T2(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

    osgUtil::IntersectionVisitor iv(intersector);
    iv.setTraversalMask(~0x1);
    osg::Camera* cam = viewer->getCamera();
    if (!cam){
        std::cerr << "getLineIntersection(): could not read camera" << std::endl;
        return false;
    }
    cam->accept(iv);
    if (!intersector->containsIntersections()){
        outLogMsg("getLineIntersection(): no intersections found");
        return false;
    }
    result = *(intersector->getIntersections().begin());
    return true;
}

bool EventHandler::getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                                 double &u, double &v)
{
    double x = ea.getX();
    double y = ea.getY();

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        std::cerr << "getRaytraceCanvasIntersection(): could not read viewer" << std::endl;
        return false;
    }

    osg::Camera* camera = viewer->getCamera();
    if (!camera){
        std::cout << "getRaytraceCanvasIntersection(): could not read camera" << std::endl;
        return false;
    }

    if (!camera->getViewport()){
        std::cerr << "getRaytraceIntersection(): could not read viewport" << std::endl;
        return false;
    }

    osg::Matrix VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    osg::Matrix invVPW;
    if (!invVPW.invert(VPW)){
        std::cerr << "getRaytraceIntersection(): could not invert View-projection-world matrix for ray casting" << std::endl;
        return false;
    }

    // Algorithm:
    // use ray-tracking techinique
    // calcualte near and far point in global 3D
    // intersect that segment with plane of canvas - 3D intersection point
    // extract local 3D coords so that to create a stroke (or apprent that point to a current stroke)
    osg::Vec3f nearPoint = osg::Vec3f(x, y, 0.f) * invVPW;
    osg::Vec3f farPoint = osg::Vec3f(x, y, 1.f) * invVPW;

    const osg::Plane plane = m_scene->getCanvasCurrent()->getPlane();
    const osg::Vec3f center = m_scene->getCanvasCurrent()->getCenter();

    assert(plane.valid());
    std::vector<osg::Vec3f> ray(2);
    ray[0] = nearPoint;
    ray[1] = farPoint;
    if (plane.intersect(ray)){ // 1 or -1: no intersection
        std::cerr << "getRaytraceIntersection(): no intersection with the ray." << std::endl;
        // finish the stroke if it was started
        // this should be replaced by a function finishAll()
        // which checks what are the current modes (sketch, photo move, etc) that are not finished
        // and finishes each which is current
        this->finishAll();
        return false;
    }
    osg::Vec3f dir = farPoint-nearPoint;
    if (! plane.dotProductNormal(dir)){ // denominator
        std::cerr << "getRaytraceIntersection(): projected line is parallel to the canvas plane" << std::endl;
        return false;
    }
    if (! plane.dotProductNormal(center-nearPoint)){
        std::cerr << "getRaytraceIntersection(): plane contains the line, so no single intersection can be defined" << std::endl;
        return false;
    }

    double len = plane.dotProductNormal(center-nearPoint) / plane.dotProductNormal(dir);
    osg::Vec3f P = dir * len + nearPoint;
    //outLogVec("getRaytraceIntersection(): intersect point global 3D", P.x(), P.y(), P.z());

    osg::Matrix M =  m_scene->getCanvasCurrent()->getTransform()->getMatrix();
    osg::Matrix invM;
    if (!invM.invert(M)){
        std::cerr << "getRaytraceIntersection(): could not invert model matrix" << std::endl;
        return false;
    }
    osg::Vec3f p = P * invM;
    if (std::fabs(p.z())>dureu::EPSILON){
        std::cerr << "getRaytraceIntersection(): error while projecting point from global 3D to local 3D, z-coordinate is not zero" << std::endl;
        outLogVec("p", p.x(), p.y(), p.z());
        outLogVec("P", P.x(), P.y(), P.z());
        outLogVec("Normal", plane.getNormal().x(), plane.getNormal().y(), plane.getNormal().z());
        outLogVec("Center", center.x(), center.y(), center.z());
        return false;
    }

    u=p.x();
    v=p.y();
    return true;
}

bool EventHandler::getRaytraceNormalProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Vec3f& XC)
{
    double x = ea.getX();
    double y = ea.getY();

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        std::cerr << "getRaytraceNormalProjection(): could not read viewer" << std::endl;
        return false;
    }

    osg::Camera* camera = viewer->getCamera();
    if (!camera){
        std::cout << "getRaytraceNormalProjection(): could not read camera" << std::endl;
        return false;
    }

    if (!camera->getViewport()){
        std::cerr << "getRaytraceNormalProjection(): could not read viewport" << std::endl;
        return false;
    }

    osg::Matrix VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    osg::Matrix invVPW;
    if (!invVPW.invert(VPW)){
        std::cerr << "getRaytraceNormalProjection(): could not invert View-projection-world matrix for ray casting" << std::endl;
        return false;
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
    osg::Vec3f C = m_scene->getCanvasCurrent()->getCenter();
    osg::Vec3f N = m_scene->getCanvasCurrent()->getNormal();

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
        std::cerr << "getRaytraceNormalProjection(): cast ray and normal are almost parallel. To resolve, change the camera view." << std::endl;
        this->finishAll();
        return false;
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
    XC = X1 - C;
    return true;
}

/* If any entity is in edit mode,
 * finish the edit command.
 * For example, if photo was in edit mode, set it to normal mode
 * This function should be called only when an intersector fails to
 * find an intersection. It can happen when, for example, the mouse goes
 * off-plane mode, or the mouse movement is too fast and the photo is not
 * tracked under the cursor.
 */
void EventHandler::finishAll()
{
    switch (m_mode)
    {
    case dureu::MOUSE_SKETCH:
        m_scene->addStroke(0,0, dureu::EVENT_OFF);
        break;
    case dureu::MOUSE_CANVAS_OFFSET:
        m_scene->editCanvasOffset(osg::Vec3f(0,0,0), dureu::EVENT_OFF);
        break;
    case dureu::MOUSE_CANVAS_ROTATE:
        m_scene->editCanvasOffset(osg::Vec3f(0,0,0), dureu::EVENT_OFF);
        break;
    case dureu::MOUSE_CANVAS_CLONE:
        m_scene->editCanvasClone(osg::Vec3f(0,0,0), dureu::EVENT_OFF);
        break;
    case dureu::MOUSE_PHOTO_MOVE:
        m_scene->editPhotoMove(0,0,0, dureu::EVENT_OFF);
        break;
    case dureu::MOUSE_PHOTO_SCALE:
        m_scene->editPhotoScale(0,0,0, dureu::EVENT_OFF);
        break;
    case dureu::MOUSE_PHOTO_ROTATE:
        m_scene->editPhotoRotate(0,0,0, dureu::EVENT_OFF);
        break;
    default:
        break;
    }
    m_photo = 0;
}

template <typename T1, typename T2>
void EventHandler::doEditPhotoScale(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           ))
        return;

    T1* result = new T1;
    bool intersects = this->getLineIntersection<T1, T2>(ea,aa, *result);

    /* pick photo to track */
    if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH){
        if (!intersects)
            return;
        m_photo = this->getPhoto(*result);
    }

    if (!m_photo){
        std::cerr << "doEditPhotoScale(): no selected photo" << std::endl;
        return;
    }

    double u=0, v=0;
    if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
        return;

    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        std::cout << "scale: push button" << std::endl;
        m_scene->editPhotoScale(m_photo.get(), u, v, dureu::EVENT_PRESSED);
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        std::cout << "scale: release button" << std::endl;
        m_scene->editPhotoScale(m_photo.get(), u, v, dureu::EVENT_RELEASED);
        this->finishAll();
        break;
    case osgGA::GUIEventAdapter::DRAG:
        m_scene->editPhotoScale(m_photo.get(), u, v, dureu::EVENT_DRAGGED);
        break;
    default:
        break;
    }
}

template <typename T1, typename T2>
void EventHandler::doEditPhotoRotate(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           ))
        return;

    T1* result = new T1;
    bool intersects = this->getLineIntersection<T1, T2>(ea,aa, *result);

    /* pick photo to track */
    if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH){
        if (!intersects)
            return;
        m_photo = this->getPhoto(*result);
    }

    double u=0, v=0;
    if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
        return;

    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        std::cout << "scale: push button" << std::endl;
        m_scene->editPhotoRotate(m_photo.get(), u, v, dureu::EVENT_PRESSED);
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        std::cout << "scale: release button" << std::endl;
        m_scene->editPhotoRotate(m_photo.get(), u, v, dureu::EVENT_RELEASED);
        this->finishAll();
        break;
    case osgGA::GUIEventAdapter::DRAG:
        m_scene->editPhotoRotate(m_photo.get(), u, v, dureu::EVENT_DRAGGED);
        break;
    default:
        break;
    }

}

template <typename T1, typename T2>
void EventHandler::doSelectStroke(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
        m_scene->getCanvasCurrent()->resetStrokesSelected();

    if (ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE || ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return;

    T1* result = new T1;
    bool intersected = this->getLineIntersection<T1, T2>(ea,aa, *result);
    if (!intersected)
        return;

    entity::Stroke* stroke = this->getStroke(*result);
    if (!stroke){
        std::cerr << "doPickStroke(): could not dynamic_cast<Stroke*>" << std::endl;
        return;
    }
    m_scene->getCanvasCurrent()->addStrokesSelected(stroke);
}

template <typename T1, typename T2>
void EventHandler::doSelectEntity(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE || ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return;

    T1* result = new T1;
    bool intersected = this->getLineIntersection<T1, T2>(ea,aa, *result);
    if (!intersected)
        return;

    entity::Canvas* cnv = this->getCanvas(*result);
    if (!cnv){
        std::cerr << "doPickCanvas(): could not dynamic_cast<Canvas*>" << std::endl;
        return;
    }
    std::cout << "doPickCanvas(): assumed canvas with name: " << cnv->getName() << std::endl;
    m_scene->setCanvasCurrent(cnv);
}
