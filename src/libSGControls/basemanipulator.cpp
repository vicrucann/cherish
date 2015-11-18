#include "basemanipulator.h"

BaseManipulator::BaseManipulator()
{

}

bool BaseManipulator::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    switch( ea.getEventType() )
    {

    case osgGA::GUIEventAdapter::FRAME:
        return handleFrame( ea, us );

    case osgGA::GUIEventAdapter::RESIZE:
        return handleResize( ea, us );

    default:
        break;
    }

    if( ea.getHandled() )
        return false;

    switch( ea.getEventType() )
    {
    case osgGA::GUIEventAdapter::MOVE:
        return handleMouseMove( ea, us );

    case osgGA::GUIEventAdapter::DRAG:
        return handleMouseDrag( ea, us );

    case osgGA::GUIEventAdapter::PUSH:
        return handleMousePush( ea, us );

    case osgGA::GUIEventAdapter::RELEASE:
        return handleMouseRelease( ea, us );

    case osgGA::GUIEventAdapter::KEYDOWN:
        return handleKeyDown( ea, us );

    case osgGA::GUIEventAdapter::KEYUP:
        return handleKeyUp( ea, us );

    case osgGA::GUIEventAdapter::SCROLL:
        if( _flags & PROCESS_MOUSE_WHEEL )
            return handleMouseWheel( ea, us );
        else
            return false;

    default:
        return false;
    }
}

BaseManipulator::~BaseManipulator()
{

}

bool BaseManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return false;
}

bool BaseManipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return this->wrapRotation(eventTimeDelta, dx, dy);
}

bool BaseManipulator::wrapRotation(const double eventTimeDelta, const double dx, const double dy)
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
