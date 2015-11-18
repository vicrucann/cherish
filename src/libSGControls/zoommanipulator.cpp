#include "zoommanipulator.h"

ZoomManipulator::ZoomManipulator()
{

}

bool ZoomManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return this->wrapZoom(eventTimeDelta, dx, dy);
}
