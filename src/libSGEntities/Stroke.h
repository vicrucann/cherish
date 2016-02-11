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

#include "Settings.h"
#include <osg/Geometry>
#include <osgDB/ObjectWrapper>

namespace entity {
class Stroke : public osg::Geometry {
public:
    Stroke();
    Stroke(const Stroke& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, Stroke)

    void setLines(osg::DrawArrays* lines);
    const osg::DrawArrays* getLines() const;

    inline void setColor(const osg::Vec4f& color);
    inline const osg::Vec4f& getColor() const;

    void appendPoint(const float u, const float v);
    void removePoints(unsigned int index_start, unsigned int index_end);
    float getLength() const;
    bool isLengthy() const;
    void moveDelta(double du, double dv);
    void scale(double s);

private:
    osg::ref_ptr<osg::DrawArrays> m_lines;
    osg::Vec4f m_color;
};
}

#endif // STROKE

