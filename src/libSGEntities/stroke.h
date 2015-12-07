#ifndef STROKE
#define STROKE

#include "settings.h"
#include <osg/Geometry>

class Stroke : public osg::Geometry {
public:
    Stroke();
    Stroke(const osg::Vec2f& p1, const osg::Vec2f& p2);

    bool addPoint(float u, float v);
    void setColor(osg::Vec4f color);
private:
};

#endif // STROKE

