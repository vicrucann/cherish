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
#include "Entity2D.h"
#include <osg/Geometry>
#include <osgDB/ObjectWrapper>

namespace entity {

/*! \class Stroke
 * Class description
*/
class Stroke : public entity::Entity2D {
public:
    /*! */
    Stroke();

    /*! */
    Stroke(const Stroke& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, Stroke)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /* functions to be used by OSG serialization only */
    void setLines(osg::DrawArrays* lines);
    const osg::DrawArrays* getLines() const;

    inline void setColor(const osg::Vec4f& color);
    inline const osg::Vec4f& getColor() const;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    /*! A method to add a point to the end of a stroke. */
    void appendPoint(const float u, const float v);

    /*! \return length of the stroke */
    float getLength() const;

    /*! \return true if the stroke is longer than allowed threshold, false otherwise. */
    bool isLengthy() const;

    /*! \param du is delta movement in X local axis direction, \param dv is delta movement in Y local axis direction. */
    void moveDelta(double du, double dv);

    /*! \param scaleX is scaling factor along U local axis, \param scaleY is scaling factor along V local axis,
     * \param center is the local 2D center around which to scale. */
    void scale(double scaleX, double scaleY, osg::Vec3f center);

    /*! \param scale is scaling factor, \param center is the local 2D center around which to scale. */
    void scale(double scale, osg::Vec3f center);

    /*! \param theta is angle in radians, \param center is the local 2D center around which to rotate. */
    void rotate(double theta, osg::Vec3f center);

    /*! \return entity type */
    cher::ENTITY_TYPE getEntityType() const;

private:
    osg::ref_ptr<osg::DrawArrays> m_lines;
    osg::ref_ptr<osg::Program> m_program;
    osg::Vec4f m_color;
};
}

#endif // STROKE

