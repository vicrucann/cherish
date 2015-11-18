#include <iostream>
#include <math.h>
#include <assert.h>

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
    _nx(0),
    _ny(0),
    _eye(osg::Vec3d(0.f,0.f,0.f)),
    _center(osg::Vec3d(0.f,0.f,0.f)),
    _up(osg::Vec3d(0.f,0.f,-1.f)),
    _wheelZoomFactor(0.1), // how fast the zoom is
    _trackballSize(0.8), // in grad
    _orbit(new osgGA::OrbitManipulator)
{
}

bool BaseHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa){
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view){
        if (view->getCamera()){
            view->getCamera()->getViewMatrixAsLookAt(_eye, _center, _up);
            //_rotation.set(view->getCamera()->getViewMatrix());
        }
        switch (ea.getEventType()){
        case osgGA::GUIEventAdapter::SCROLL:
            if (ea.getScrollingMotion()==osgGA::GUIEventAdapter::SCROLL_UP)
                this->zoomScroll(-_wheelZoomFactor);
            else
                this->zoomScroll(_wheelZoomFactor);
        case osgGA::GUIEventAdapter::PUSH:
            _dx = -1.f;
            _dy = -1.f;
            _nx = ea.getXnormalized();
            _ny = ea.getYnormalized();
            break;
        case osgGA::GUIEventAdapter::DRAG:
            if (_dx>0.f && _dy>0.f){
                if (ea.getButtonMask()==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON){
                    this->pan(ea.getX(), ea.getY());
                    //this->zoomCameraMouse(ea.getY(), ea.getWindowHeight());
                }
                else if (ea.getButtonMask()==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON){
                    this->rotate(ea.getXnormalized(), ea.getYnormalized());
                }
            }
            _dx = ea.getX();
            _dy = ea.getY();
            _nx = ea.getXnormalized();
            _ny = ea.getYnormalized();
            break;
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

void BaseHandler::pan(float x, float y) {
    _panX += x - _dx;
    _panY += y - _dy;
}

void BaseHandler::zoomMouse(float mouseY, int widnowHeight) {
    float dy = -mouseY + _dy;
    zoomScroll(dy/widnowHeight);
}

void BaseHandler::zoomScroll(float dy){
    _zoom *= 1.0 + dy;
    if (_zoom<1.0)
        _zoom = 1.0;
}

// 1. project the points on trackball (p1 and p2)
// 2. calculate axis of rotation: cross(P1.P2, O.P1)
// Input arguments are normalized [-1.0... 1.0]
void BaseHandler::rotate(float x, float y) {
    osg::Matrixd viewmat;
    viewmat.makeLookAt(_eye, _center, _up);
    osg::Quat rotq;
    rotq.set(viewmat);

    trackball(rotq, x, y, x+0.2, y+0.2);
    viewmat.setRotate(rotq);

    viewmat.getLookAt(_eye, _center, _up);
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

void BaseHandler::trackball(osg::Quat& rotq, float x, float y, float dx, float dy){
    if (x==dx && y==dy) // zero roation
        return;

    osg::Matrixd rotation_matrix(rotq);
    osg::Vec3d uv = osg::Vec3d(0.0f,1.0f,0.0f)*rotation_matrix;
    osg::Vec3d sv = osg::Vec3d(1.0f,0.0f,0.0f)*rotation_matrix;
    osg::Vec3d lv = osg::Vec3d(0.0f,0.0f,-1.0f)*rotation_matrix;


    osg::Vec3d p1 = sv*x + uv*y - lv*tb_project_to_sphere(_trackballSize, x, y);
    //(x,y,tb_project_to_sphere(_trackballSize, x,y));
    osg::Vec3d p2 = sv*_nx + uv*_ny - lv*tb_project_to_sphere(_trackballSize, _nx, _ny);
    //(dx,dy,tb_project_to_sphere(_trackballSize, dx,dy));

    osg::Vec3d axis(p2^p1);
    axis.normalize();

    float t = (p2-p1).length() / (2.0*_trackballSize);
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    float angle = osg::inRadians(std::asin(t));

    rotq *= osg::Quat(angle, axis); // combine two rotations
}

/*
void BaseHandler::rotate(float mouseXNorm, float mouseYNorm){
    if (_orbit){
        osg::Quat rotation_quat = _orbit->getRotation();
        osg::Matrixd rotation_matrix(rotation_quat);
        osg::Vec3d axis0;
        double angle0;
        rotation_quat.getRotate(angle0, axis0);
        std::cout << "angle before: " << angle0 << std::endl;
        std::cout << "axis before: " << axis0[0] << " " << axis0[1] << " " << axis0[2] << std::endl;

        osg::Vec3d uv = osg::Vec3d(0.0f,1.0f,0.0f)*rotation_matrix;
        osg::Vec3d sv = osg::Vec3d(1.0f,0.0f,0.0f)*rotation_matrix;
        osg::Vec3d lv = osg::Vec3d(0.0f,0.0f,-1.0f)*rotation_matrix;

        float p1x = mouseXNorm;
        float p1y = mouseYNorm;
        float p2x = _nx;
        float p2y = _ny;

        assert(p1x>=-1.0 && p1x<=1.0);
        assert(p1y>=-1.0 && p1x<=1.0);
        assert(p2x>=-1.0 && p1x<=1.0);
        assert(p2y>=-1.0 && p1x<=1.0);

        osg::Vec3d p1 = sv*p1x + uv*p1y - lv*tb_project_to_sphere(_trackballSize, p1x, p1y);
        osg::Vec3d p2 = sv*p2x + uv*p2y - lv*tb_project_to_sphere(_trackballSize, p2x, p2y);

        osg::Vec3d axis = p2^p1;
        axis.normalize();

        float t = (p2 - p1).length() / (2.0 * _trackballSize);
        if (t > 1.0) t = 1.0;
        if (t < -1.0) t = -1.0;
        float angle = osg::inRadians(std::asin(t));


        std::cout << "angle after: " << angle << std::endl;
        std::cout << "axis after: " << axis[0] << " " << axis[1] << " " << axis[2] << std::endl << std::endl;

        _orbit->setRotation(osg::Quat(angle, axis) * rotation_quat);
    }
} */

void BaseHandler::setWheelZoomFactor(double wzf){
    _wheelZoomFactor = wzf;
}

void BaseHandler::setTrackballSize(double ts){
    _trackballSize = ts;
}

float BaseHandler::tb_project_to_sphere(float r, float x, float y) {
    float d, t, z;
    d = std::sqrt(x*x + y*y);
    if (d < r * 0.70710678118654752440) /* Inside sphere */
        z = std::sqrt(r*r - d*d);
    else{  /* On hyperbola */
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
}
