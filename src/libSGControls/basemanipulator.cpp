#include "basemanipulator.h"

bool BaseManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return false;
}

bool BaseManipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return this->wrapRotation(eventTimeDelta, dx, dy);
}

bool BaseManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return this->wrapPan(eventTimeDelta, dx, dy);
}

bool BaseManipulator::wrapRotation(const double eventTimeDelta, const double dx, const double dy){
    // rotate camera
    if( getVerticalAxisFixed() )
        rotateWithFixedVertical( dx, dy );
    else
        rotateTrackball( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
                         _ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
                         getThrowScale( eventTimeDelta ) );
    return true;
}

bool BaseManipulator::wrapPan(const double eventTimeDelta, const double dx, const double dy){
    // pan model
    float scale = -0.3f * _distance * getThrowScale( eventTimeDelta );
    panModel( dx*scale, dy*scale );
    return true;
}

bool BaseManipulator::wrapZoom(const double eventTimeDelta, const double /*dx*/, const double dy){
    zoomModel( dy * getThrowScale( eventTimeDelta ), true );
    return true;
}
