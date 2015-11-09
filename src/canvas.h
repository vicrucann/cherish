#ifndef CANVAS
#define CANVAS

#include "settings.h"

#include <osg/Geode>
#include <osg/BoundingBox>

class Canvas : public osg::Geode {
public:
    Canvas();
    Canvas(osg::Vec3 center, osg::BoundingBox bb, osg::Vec4 color,double boundMargin = dureu::CANVAS_MIN_BOUND_MARGIN);
    ~Canvas(){}

private:
    void addCanvasDrawables();
    osg::Vec3 _center;
    osg::Vec3 _normal;
    osg::Vec4 _color;
    osg::BoundingBox _bb;
    double _boundMargin;
};

#endif // CANVAS

