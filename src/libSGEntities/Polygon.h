#ifndef POLYGON_H
#define POLYGON_H

#include "Settings.h"
#include "Entity2D.h"

#include <osg/Geometry>
#include <osgDB/ObjectWrapper>

namespace entity {

/*! \class Polygon
 * \brief Geometry class that defines opacity polygons drawn by a user.
 * The creation and usage of this class must be followed after certain steps. Usually, this class API is created with event
 * handler in mind, i.e., it is assumed the user first dtaws the polygon borders (thus a polygon phantom is created), and then
 * the phantom must be re-defined as am opaque polygon. For example:
 *
 * \code{.cpp}
 * // start from creating an empty Polygon, always
 * entity::Polygon* phantom = new entity::Polygon;
 *
 * // initialize the shader program
 * phantom->initializeProgram(p); // e.g., Canvas::getProgramPolygon
 *
 * // as user draws, add mouse coordinates, in an event loop:
 * phantom->addPoint(u,v);
 *
 * // after user is finished, re-define the outer path to the opaque polygon type:
 * phantom->redefineToPolygon();
 * \endcode
 *
 * The below example provides details on how to copy/clone a polygon that is already present on the scene graph:
 * \code{.cpp}
 * // create empty polygon
 * entity::Polygon* copy = new entity::Polygon;
 *
 * // copy data points from another polygon
 * copy->copyFrom(phantom);
 *
 * // re-define to the polygon type
 * copy->redefineToPolygon();
 * \endcode
*/

class Polygon : public entity::Entity2D
{
public:
    /*! Constructor that creates an empty polygon. */
    Polygon();

    /*! Copy constructor, only used for serialization; not to be used otherwise. */
    Polygon(const Polygon& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, Polygon)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /* functions to be used by OSG serialization only (though getters might be used) */
    void setLines(osg::DrawArrays* lines);
    const osg::DrawArrays* getLines() const;

    void setColor(const osg::Vec4f& color);
    const osg::Vec4f& getColor() const;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    /*! A method to be used to copy the input polygon's data. It is assumed *this polygon is empty.
     * \param copy is the source polygon to copy from. */
    bool copyFrom(const entity::Polygon* copy);

    /*! A method to add a point to the end of a polygon. It is normally used when constructing a polygon in-motion while sketching.
     * \param u is local U coordinate, \param v is local V coordinate. */
    void appendPoint(const float u, const float v);

    /*! A method that overwrites the coordinates of the last point of the polygon. */
    void editLastPoint(float u, float v);

    /*! \param i is the point index. \return point coordinates at the specified index. */
    osg::Vec2f getPoint(unsigned int i) const;

    /*! A method changes the geometry from line adjacency to polygon type thus allowing opacity of a region. It should be
     * called on completion of polygon draw (e.g., when phantom last point is very clone to its first point) from event handler.
     * \return true upon success. */
    void redefineToPolygon();

    /*! \return number of vertices. */
    int getNumPoints() const;

public:
    /*! A method to perform translation of the polygon in delta movement.
     * \param du is delta movement in X local axis direction, \param dv is delta movement in Y local axis direction. */
    void moveDelta(double du, double dv);

    /*! A method to scale the polygon around a fixed point.
     * \param scaleX is scaling factor along U local axis, \param scaleY is scaling factor along V local axis,
     * \param center is the local 2D center around which to scale. */
    void scale(double scaleX, double scaleY, osg::Vec3f center);

    /*! A method to uniformely scale the polygon around a fixed point.
     * \param scale is scaling factor, \param center is the local 2D center around which to scale. */
    void scale(double scale, osg::Vec3f center);

    /*! A method to rotate the polygon around a fixed point.
     * \param theta is angle in radians, \param center is the local 2D center around which to rotate. */
    void rotate(double theta, osg::Vec3f center);

    /*! A re-defined method of entity::Entity2D to obtain entity type.
     * \return entity type */
    cher::ENTITY_TYPE getEntityType() const;

private:
    osg::ref_ptr<osg::DrawArrays>       m_lines; // saved to file
    osg::Vec4f                          m_color; // saved to file
};
} // namespace entity

#endif // POLYGON_H
