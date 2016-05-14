#ifndef MANIPULATOR
#define MANIPULATOR

#include "Settings.h"
#include "RootScene.h"

#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/TrackballManipulator>

/*! \class Manipulator
 * Class description
*/
class Manipulator : public osgGA::TrackballManipulator {
public:
    Manipulator(cher::MOUSE_MODE mode = cher::CAMERA_ORBIT);
    void setMode(cher::MOUSE_MODE mode);
protected:
    virtual bool performMovementLeftMouseButton (const double eventTimeDelta, const double dx, const double dy);
    virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );
    virtual bool performMovementRightMouseButton (const double eventTimeDelta, const double dx, const double dy);
private:
    virtual bool wrapRotation(const double eventTimeDelta, const double dx, const double dy);
    virtual bool wrapPan(const double eventTimeDelta, const double dx, const double dy);
    virtual bool wrapZoom(const double eventTimeDelta, const double, const double dy);

    cher::MOUSE_MODE m_mode;
};

#endif // MANIPULATOR

