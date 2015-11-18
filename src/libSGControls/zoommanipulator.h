#ifndef ZOOMMANIPULATOR
#define ZOOMMANIPULATOR

#include "basemanipulator.h"

class ZoomManipulator : public BaseManipulator{
protected:
    virtual bool performMovementLeftMouseButton (const double eventTimeDelta, const double dx, const double dy){
        return this->wrapZoom(eventTimeDelta, dx, dy);
    }
};

#endif // ZOOMMANIPULATOR

