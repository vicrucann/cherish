#ifndef PROGRAMLINESEGMENT_H
#define PROGRAMLINESEGMENT_H

#include <osg/Program>
#include <osg/StateSet>
#include <osg/observer_ptr>
#include <osg/Camera>
#include <osg/MatrixTransform>

#include "ProgramPolygon.h"

class ProgramLineSegment : public ProgramPolygon
{
public:
    ProgramLineSegment();
};

#endif // PROGRAMLINESEGMENT_H
