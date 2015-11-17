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
    _trackballSize(1.0), // in grad
    _orbit(new osgGA::OrbitManipulator)
{
}

bool BaseHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa){
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view){
        if (view->getCamera()){
            view->getCamera()->getViewMatrixAsLookAt(_eye, _center, _up);
            _orbit->setTransformation(_eye,_center, _up);
        }
        switch (ea.getEventType()){
        case osgGA::GUIEventAdapter::SCROLL:
            if (ea.getScrollingMotion()==osgGA::GUIEventAdapter::SCROLL_UP)
                this->zoomIn();
            else
                this->zoomOut();
            //break;
        case osgGA::GUIEventAdapter::PUSH:
            _dx = -1.f;
            _dy = -1.f;
            _nx = ea.getXnormalized();
            _ny = ea.getYnormalized();
            //break;
        case osgGA::GUIEventAdapter::DRAG:
            if (_dx>0.f && _dy>0.f){
                if (ea.getButtonMask()==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
                    this->rotate(ea.getXnormalized(), ea.getYnormalized());
            }
            _dx = ea.getX();
            _dy = ea.getY();
            _nx = ea.getXnormalized();
            _ny = ea.getYnormalized();
            //break;
        case osgGA::GUIEventAdapter::FRAME:
            if (_orbit)
                _orbit->getTransformation(_eye, _center, _up);
            if (view->getCamera())
                view->getCamera()->setViewMatrixAsLookAt(_eye, _center, _up);
            //break;
        default:
            break;
        }
    }
    return false;
}

void BaseHandler::zoomIn(){
    if (_orbit)
        _orbit->setDistance(_orbit->getDistance()-_wheelZoomFactor);
}

void BaseHandler::zoomOut(){
    if (_orbit)
        _orbit->setDistance(_orbit->getDistance()+_wheelZoomFactor);
}

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

        _orbit->setRotation(osg::Quat(angle0+angle, axis0+axis));
    }
}

void BaseHandler::setWheelZoomFactor(double wzf){
    _wheelZoomFactor = wzf;
}

void BaseHandler::setTrackballSize(double ts){
    _trackballSize = ts;
}

/**
 * Helper trackball method that projects an x,y pair onto a sphere of radius r OR
 * a hyperbolic sheet if we are away from the center of the sphere.
 */
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
