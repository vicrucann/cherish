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
#include <osg/MatrixTransform>
#include <osgDB/ObjectWrapper>

#include "libSGControls/ProgramStroke.h"
#include "ShaderedEntity2D.h"

namespace entity {

/*! \class Stroke
 * \brief Geometry class that defined strokes entered by a user.
 * The creation and usage of this class must be followed after certain steps. Here's an example
 * of how to create a new stroke when user is drawing right away:
 *
 * \code{.cpp}
 * // create an empty stroke
 * entity::Stroke* original = new entity::Stroke;
 *
 * // initialize the shader program
 * original->initializeProgram(p); // e.g. Canvas::getProgramStroke()
 *
 * // as user draws, add mouse coordinates, in an event loop:
 * original->addPoint(u,v);
 *
 * \\ after user is finished drawing, re-define the look (shaderize as well)
 * original->redefineToShape();
 * \endcode
 *
 * The below example provides details on how to copy/clone a stroke that is already present on the
 * scene graph:
 * \code{.cpp}
 * // create an empty stroke
 * entity::Stroke* copy = new entity::Stroke;
 *
 * // copy data points from the stroke that is already on the scene (which means it is shadered)
 * // it also copies shader data
 * copy->copyFrom(original);
 *
 * // as before, we re-define the stroke to be shadered
 * copy->redefineToShape();
 * \endcode
*/
class Stroke : public entity::ShaderedEntity2D {
public:
    /*! Constructor that creates an empty stroke. */
    Stroke();

    /*! Copy constructor, only used for serialization; not to be used otherwise. */
    Stroke(const Stroke& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, Stroke)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /* functions to be used by OSG serialization only */
    void setIsCurved(bool curved);
    bool getIsCurved() const;   
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    virtual bool copyFrom(const entity::ShaderedEntity2D* copy);

    /*! A method that fits the stroke's points to a set of curve using Schneider's algorithm.
     * \param t is the Canvas matrix transform. If none is provided, the transform of the current canvas is taken.
     * \param tolerance is the threshold fitting.
     * \return true upon success. */
    virtual bool redefineToShape(osg::MatrixTransform* t = 0);

    /*! A method that generates mesh representation of the stroke using Parallel Transport Algorithm.
     * \return pointer on the cretated mesh structure. The structure is not attached to the scene graph. */
    osg::Node* getMeshRepresentation() const;

protected:
    /*! A method to tune the look of the stroke with smoother connections and thicker linewidth.
     * So that to avoid broken and thin look of the default OpenGL functionality when using GL_LINE_STRIP_ADJACENCY and such. */
    bool redefineToShader(osg::MatrixTransform* t);

public:
    /*! \return length of the stroke, which is measured as a largest dimention of the bounding box around the stroke. */
    float getLength() const;

    /*! \return true if the stroke is longer than allowed threshold, false otherwise. */
    bool isLengthy() const;

    /*! A re-defined method of entity::Entity2D to obtain entity type.
     * \return entity type */
    cher::ENTITY_TYPE getEntityType() const;

    virtual ProgramStroke* getProgram() const;

    /*! A method to add a point to the end of the entity. It is normally used when constructing an emtity in-motion while sketching.
     * \param u is local U coordinate, \param v is local V coordinate. */
    virtual void appendPoint(const float u, const float v);

protected:

    /*! \return Sampled points from provided set of bezier control points. */
    osg::Vec3Array* getCurvePoints(const osg::Vec3Array* bezierPts) const;

    /*! A method to make sure the curve is not too small, neither too large for a fitter tolerance level.
     * Nomalization should be applied before the fitting algorithm, and then the result coordinates must get
     * denrmalized back to their true size.
     * The nornalization algorithm is as follows:
     * 1. Find center of the points.
     * 2. For each curve coordinate, substract a center coordinate from it.
     * 3. The scale factor equals to all the squared point deviations devided by total number of the points.
     * 4. Scale each coordinate by scale factor.
     * \param path is the point array to normalize,
     * \param center is local 2d center (e.g., bounding box center).
     * \return scaling factor. */
    double normalize(osg::Vec3Array* path, const osg::Vec3f& center);

    /*! A method to denormalize the curve coordinates. Should be used after the curve fitting algorithm.
     * \sa normalize(). */
    void denormalize(osg::Vec3Array* path, const osg::Vec3f& center, double scale);

private:
    bool                                m_isCurved; // saved to file
};
}

#endif // STROKE

