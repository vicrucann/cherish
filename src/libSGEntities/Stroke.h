/* Stroke
 * Ctor creates an empty stroke and initializes the initial
 * parameters.
 * appendPoint() adds a point to the stroke.
 * getLength() is called to check if the stroke is long enough
 * to display and keep it in memory.
 *
 * This class is rather low-level API. It must be used with the
 * defined logic from upper-level class, e.g. from Canvas, or
 * addGeometryCommand(), or from EventHanlder (when changing color)
 * from normal mode to `selected` or `edit`.
 *
 */

#ifndef STROKE
#define STROKE

#include "settings.h"
#include <osg/Geometry>

class Stroke : public osg::Geometry {
public:
    Stroke();
    void appendPoint(const float u, const float v);
    float getLength() const;
    bool isLengthy() const;

    void setVertexData(osg::Vec3Array* vertices);
    osg::Vec3Array* getVertexData() const;

    void setColor(const osg::Vec4f &color);
private:
    osg::DrawArrays* mDrawArrayLines;
    osg::DrawArrays* mDrawArrayPoints;
    osg::Vec3Array* mVertexData;
    osg::Vec4Array* mColors;
};

#endif // STROKE

