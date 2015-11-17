#include <iostream>
#include <math.h>

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
    _wheelZoomFactor(0.1), // how fast the zoom is
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
        case osgGA::GUIEventAdapter::FRAME:
            if (_orbit)
                _orbit->getTransformation(_eye, _center, _up);
            if (view->getCamera())
                view->getCamera()->setViewMatrixAsLookAt(_eye, _center, _up);
            break;
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

void BaseHandler::setWheelZoomFactor(double wzf){
    _wheelZoomFactor = wzf;
}
