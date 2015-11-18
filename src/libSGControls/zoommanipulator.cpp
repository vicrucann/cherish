#include "zoommanipulator.h"

bool ZoomManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return this->wrapZoom(eventTimeDelta, dx, dy);
}
