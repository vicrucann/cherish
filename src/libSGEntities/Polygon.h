#ifndef POLYGON_H
#define POLYGON_H

#include "Settings.h"
#include "Entity2D.h"
#include "ShaderedEntity2D.h"

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

class Polygon : public entity::ShaderedEntity2D
{
public:
    /*! Constructor that creates an empty polygon. */
    Polygon();

    /*! Copy constructor, only used for serialization; not to be used otherwise. */
    Polygon(const Polygon& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, Polygon)

    /*! A method to be used to copy the input polygon's data. It is assumed *this polygon is empty.
     * \param copy is the source polygon to copy from. */
    virtual bool copyFrom(const entity::ShaderedEntity2D* copy);

    /*! A method that overwrites the coordinates of the last point of the polygon. */
    void editLastPoint(float u, float v);

    /*! A method to remove the last point from the polygon. */
    void removeLastPoint();

    /*! A method changes the geometry from line adjacency to polygon type thus allowing opacity of a region. It should be
     * called on completion of polygon draw (e.g., when phantom last point is very clone to its first point) from event handler.
     * \return true upon success. */
    virtual bool redefineToShape(osg::MatrixTransform *t = 0);

    /*! \return whether the geometry is of polygon type */
    bool isPolygon() const;

    virtual void appendPoint(const float u, const float v);

public:

    /*! A re-defined method of entity::Entity2D to obtain entity type.
     * \return entity type */
    cher::ENTITY_TYPE getEntityType() const;

protected:

    bool redefineToShader(osg::MatrixTransform *t);

private:
};
} // namespace entity

#endif // POLYGON_H
