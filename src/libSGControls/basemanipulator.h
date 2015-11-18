#ifndef BASEMANIPULATOR
#define BASEMANIPULATOR

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/TrackballManipulator>

class BaseManipulator : public osgGA::TrackballManipulator {
public:
protected:
    virtual bool performMovementLeftMouseButton (const double eventTimeDelta, const double dx, const double dy);
    virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );
    virtual bool performMovementRightMouseButton (const double eventTimeDelta, const double dx, const double dy);

    virtual bool wrapRotation(const double eventTimeDelta, const double dx, const double dy);
    virtual bool wrapPan(const double eventTimeDelta, const double dx, const double dy);
    virtual bool wrapZoom(const double eventTimeDelta, const double, const double dy);
};

#endif // BASEMANIPULATOR

