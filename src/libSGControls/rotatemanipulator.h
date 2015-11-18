#ifndef ROTATEMANIPULATOR
#define ROTATEMANIPULATOR

#include "basemanipulator.h"

class RotateManipulator : public BaseManipulator {
protected:
    virtual bool performMovementLeftMouseButton (const double eventTimeDelta, const double dx, const double dy){
        return this->wrapRotation(eventTimeDelta, dx, dy);
    }
};

#endif // ROTATEMANIPULATOR

