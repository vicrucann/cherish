#include "Manipulator.h"

Manipulator::Manipulator(cher::MOUSE_MODE mode)
    : osgGA::TrackballManipulator()
    , m_mode(mode)
{
}

void Manipulator::setMode(cher::MOUSE_MODE mode)
{
    m_mode = mode;
}

bool Manipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    switch (m_mode){
    case cher::CAMERA_ZOOM:
        return this->wrapZoom(eventTimeDelta, dx, dy);
    case cher::CAMERA_PAN:
        return this->wrapPan(eventTimeDelta, dx, dy);
    case cher::CAMERA_ORBIT:
        return this->wrapRotation(eventTimeDelta, dx, dy);
    case cher::CAMERA_FIXEDVIEW:
        return this->wrapPan(eventTimeDelta, dx, dy);
    default:
        return false;
    }
}

bool Manipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    if (m_mode == cher::CAMERA_FIXEDVIEW)
        return false;
    else
        return this->wrapRotation(eventTimeDelta, dx, dy);
}

bool Manipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return this->wrapPan(eventTimeDelta, dx, dy);
}

bool Manipulator::wrapRotation(const double eventTimeDelta, const double dx, const double dy)
{
    // rotate camera
    if( getVerticalAxisFixed() )
        rotateWithFixedVertical( dx, dy );
    else
        rotateTrackball( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
                         _ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
                         getThrowScale( eventTimeDelta ) );
    return true;
}

bool Manipulator::wrapPan(const double eventTimeDelta, const double dx, const double dy)
{
    // pan model
    float scale = -0.3f * _distance * getThrowScale( eventTimeDelta );
    panModel( dx*scale, dy*scale );
    return true;
}

bool Manipulator::wrapZoom(const double eventTimeDelta, const double, const double dy)
{
    zoomModel( dy * getThrowScale( eventTimeDelta ), true );
    return true;
}


