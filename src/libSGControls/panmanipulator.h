#ifndef PANMANIPULATOR
#define PANMANIPULATOR

#include "basehandler.h"

class PanManipulator : public BaseManipulator{
protected:
    virtual bool performMovementLeftMouseButton (const double eventTimeDelta, const double dx, const double dy){
        return this->wrapPan(eventTimeDelta, dx, dy);
    }
};

#endif // PANMANIPULATOR

