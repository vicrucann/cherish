#ifndef STROKE
#define STROKE

#include "settings.h"
#include <osg/Geometry>

class Stroke : public osg::Geometry {
public:
    Stroke();
    void appendPoint(float u, float v);

    void setColor(osg::Vec4f color);
private:
    osg::DrawArrays* _mDrawArrayLines;
    osg::Vec3Array* _mVertexData;
    osg::Vec4Array* _mColors;
};

#endif // STROKE

