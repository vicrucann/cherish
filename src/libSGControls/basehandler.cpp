#include <osgViewer/View>
#include <osg/Vec3d>
#include <osgGA/OrbitManipulator>
#include <osg/Matrix>

#include "basehandler.h"

BaseHandler::BaseHandler():
    _zoom(-1.0),
    _panX(0.f),
    _panY(0.f),
    _angleH(0.f),
    _angleV(0.f),
    _dx(-1.f),
    _dy(-1.f),
    _eye(osg::Vec3d(0.f,0.f,0.f)),
    _center(osg::Vec3d(0.f,0.f,0.f)),
    _up(osg::Vec3d(0.f,0.f,-1.f)),
    _wheelZoomFactor(0.05) // how fast the scrolling is done
{
}

bool BaseHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa){
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view){
        if (view->getCamera())
            view->getCamera()->getViewMatrixAsLookAt(_eye, _center, _up);
        switch (ea.getEventType()){
        case osgGA::GUIEventAdapter::SCROLL:
            if (ea.getScrollingMotion()==osgGA::GUIEventAdapter::SCROLL_UP)
                this->zoomCameraScroll(-_wheelZoomFactor);
            else
                this->zoomCameraScroll(_wheelZoomFactor);
        case osgGA::GUIEventAdapter::PUSH:
            _dx = -1.f;
            _dy = -1.f;
            break;
        case osgGA::GUIEventAdapter::DRAG:
            if (_dx>0.f && _dy>0.f){
                if (ea.getButtonMask()==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON){
                    this->panCamera(ea.getX(), ea.getY());
                    //this->zoomCameraMouse(ea.getY(), ea.getWindowHeight());
                }
            }
            _dx = ea.getX();
            _dy = ea.getY();
        case osgGA::GUIEventAdapter::FRAME:
            if (view->getCamera()){
                this->adjustCamera(view->getSceneData()->getBound());
                view->getCamera()->setViewMatrixAsLookAt(_eye, _center, _up);
            }
            break;
        default:
            break;
        }
    }
    return false;
}

void BaseHandler::panCamera(float x, float y) {
    _panX += x - _dx;
    _panY += y - _dy;
}

void BaseHandler::zoomCameraMouse(float mouseY, int widnowHeight) {
    float dy = -mouseY + _dy;
    zoomCameraScroll(dy/widnowHeight);
    //_zoom *= 1.0 + dy / widnowHeight;
    //if (_zoom<1.0)
    //    _zoom = 1.0;
}

void BaseHandler::zoomCameraScroll(float dy){
    _zoom *= 1.0 + dy;
    if (_zoom<1.0)
        _zoom = 1.0;
}

void BaseHandler::rotateCamera() {
}

void BaseHandler::setWheelZoomFactor(double wzf){
    _wheelZoomFactor = wzf;
}

void BaseHandler::adjustCamera(osg::BoundingSphere bs){
    // orthogonal basis, see details in Chapter 15 "Working with cameras" in
    // the book "Essentials of Interactive Computer Graphics: Concepts and Implementation"

    // adjust eye from rotation params first?
    osg::Vec3d lookDir = _center - _eye;
    lookDir.normalize();
    osg::Vec3d side = lookDir ^ _up;
    side.normalize();
    if (_zoom<0.0)
        _zoom = bs.radius()*3.0;
    _center = bs.center() - (side*_panX + _up*_panY)*0.01;
    _eye = _center - lookDir*_zoom;
}

