#include "manipulator.h"

Manipulator::Manipulator(dureu::MANIP_MODE mode):
    _mode(mode)
{
}

bool Manipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    switch (_mode){
    case dureu::MANIP_ZOOM:
        return this->wrapZoom(eventTimeDelta, dx, dy);
    case dureu::MANIP_PAN:
        return this->wrapPan(eventTimeDelta, dx, dy);
    case dureu::MANIP_ROTATE:
        return this->wrapRotation(eventTimeDelta, dx, dy);
    case dureu::MANIP_NONE:
        return false;
    default:
        return false;
    }
}

bool Manipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
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


