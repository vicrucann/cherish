#ifndef ZOOMMANIPULATOR
#define ZOOMMANIPULATOR

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

#include "basemanipulator.h"

class ZoomManipulator : public BaseManipulator{
protected:
    virtual bool performMovementLeftMouseButton (const double eventTimeDelta, const double dx, const double dy);
};

#endif // ZOOMMANIPULATOR

