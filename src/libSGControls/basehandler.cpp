#include <osgViewer/View>
#include <osg/Vec3d>
#include <osgGA/OrbitManipulator>
#include <osg/Matrix>

#include "basehandler.h"

BaseHandler::BaseHandler():
    _center(osg::Vec3(0.f, 0.f, 0.f)),
    _rotation(osg::Quat(0,0,0,0)),
    _distance(1.),
    _trackballSize(0.8),
    _wheelZoomFactor(0.1),
    _minimumDistance(0.05)
{
}

bool BaseHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa){
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view && view->getCamera()){
        osg::Vec3d eye, center, up;
        view->getCamera()->getViewMatrixAsLookAt( eye, center, up );
        this->setTransformation(eye,center,up);
        switch (ea.getEventType()){
        case osgGA::GUIEventAdapter::SCROLL:
            zoomModel(_wheelZoomFactor);
        case osgGA::GUIEventAdapter::SCROLL_DOWN:
            zoomModel(-_wheelZoomFactor);
        case osgGA::GUIEventAdapter::FRAME:
                if (view->getCamera()){
                    this->getTransformation(eye, center, up);
                    view->getCamera()->setViewMatrixAsLookAt(eye, center, up);
                }
        default:
            return false; // unhandled events
        }
    }
    return false;
}

void BaseHandler::setTransformation(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
{
    osg::Vec3d lv( center - eye );

    osg::Vec3d f( lv );
    f.normalize();
    osg::Vec3d s( f^up );
    s.normalize();
    osg::Vec3d u( s^f );
    u.normalize();

    osg::Matrixd rotation_matrix( s[0], u[0], -f[0], 0.0f,
                            s[1], u[1], -f[1], 0.0f,
                            s[2], u[2], -f[2], 0.0f,
                            0.0f, 0.0f,  0.0f, 1.0f );

    _center = center;
    _distance = lv.length();
    _rotation = rotation_matrix.getRotate().inverse();

    // fix current rotation
    //if( getVerticalAxisFixed() )
    //    fixVerticalAxis( _center, _rotation, true );
}

void BaseHandler::getTransformation(osg::Vec3d &eye, osg::Vec3d &center, osg::Vec3d &up) const
{
    center = _center;
    eye = _center + _rotation * osg::Vec3d( 0., 0., _distance );
    up = _rotation * osg::Vec3d( 0., 1., 0. );
}

void BaseHandler::zoomModel(const float dy, bool pushForwardIfNeeded)
{
    float scale = 1.0f + dy;
    float minDist = _minimumDistance;
    if( _distance*scale > minDist )
        _distance *= scale;
    else {
        float scale = -_distance;
        osg::Matrixd rotation_matrix( _rotation );
        osg::Vec3d dv = (osg::Vec3d( 0.0f, 0.0f, -1.0f ) * rotation_matrix) * (dy * scale);
        _center += dv;
    }
}

