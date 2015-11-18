#include "manipulator.h"

Manipulator::Manipulator(dureu::MANIP_MODE mode):
    _mode(mode),
    _RM(new RotateManipulator),
    _PM(new PanManipulator),
    _ZM(new ZoomManipulator)
{
}
