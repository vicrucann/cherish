#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>

#include "Canvas.h"
#include "Stroke.h"

class Utilities
{
public:
    bool areStrokesProjectable(const std::vector<entity::Stroke*>& strokes,
                               entity::Canvas* source, entity::Canvas* target,
                               const osg::Vec3f& eye);
};

#endif // UTILITIES_H
