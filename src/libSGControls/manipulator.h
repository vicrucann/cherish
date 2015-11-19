#ifndef MANIPULATOR
#define MANIPULATOR

#include "settings.h"

#include <osg/ref_ptr>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/TrackballManipulator>

class Manipulator : public osgGA::TrackballManipulator {
public:
    Manipulator(dureu::MANIP_MODE mode = dureu::MANIP_NONE);
    dureu::MANIP_MODE getMode();
    void setMode(dureu::MANIP_MODE mode);
protected:
    virtual bool performMovementLeftMouseButton (const double eventTimeDelta, const double dx, const double dy);
    virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );
    virtual bool performMovementRightMouseButton (const double eventTimeDelta, const double dx, const double dy);
private:
    virtual bool wrapRotation(const double eventTimeDelta, const double dx, const double dy);
    virtual bool wrapPan(const double eventTimeDelta, const double dx, const double dy);
    virtual bool wrapZoom(const double eventTimeDelta, const double, const double dy);

    int _mode;
};

#endif // MANIPULATOR

