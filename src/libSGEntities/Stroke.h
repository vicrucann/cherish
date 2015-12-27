#ifndef STROKE
#define STROKE

#include "settings.h"
#include <osg/Geometry>

class Stroke : public osg::Geometry {
public:
    Stroke();
    void appendPoint(float u, float v);
    float getLength() const;

    void setColor(osg::Vec4f color);
private:
    osg::DrawArrays* _mDrawArrayLines;
    osg::DrawArrays* _mDrawArrayPoints;
    osg::Vec3Array* _mVertexData;
    osg::Vec4Array* _mColors;
};

#endif // STROKE

