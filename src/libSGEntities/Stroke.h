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
#include <osg/Camera>
#include <osgDB/ObjectWrapper>

namespace entity {

/*! \class Stroke
 * \brief Geometry class that defined strokes entered by used.
 * The creation and usage of this class must be followed after certain steps. Here's an example
 * of how to create a new stroke when user is drawing right away:
 *
 * \code{.cpp}
 * // create an empty stroke
 * entity::Stroke* original = new entity::Stroke;
 *
 * // as user draws, add mouse coordinates, in an event loop:
 * original->addPoint(u,v);
 *
 * \\ after user is finished drawing, re-define the look
 * original->redefineToCurve();
 * original->redefineToShader(camera);
 * \endcode
 *
 * The below example provides details on how to copy/clone a stroke that is already present on the
 * scene graph:
 * \code{.cpp}
 * // create an empty stroke
 * entity::Stroke* copy = new entity::Stroke;
 *
 * // copy data points from the stroke that is already on the scene (which means it is shadered)
 * copy->copyFrom(original);
 *
 * // as before, we re-define the stroke to be shadered
 * copy->redefineToCurve();
 * copy->redefineToShader(original->getCamera());
 * \endcode
*/
class Stroke : public entity::Entity2D {
public:
    /*! Constructor that creates an empty stroke. */
    Stroke();

    /*! Copy constructor, only used for serialization; not to be used otherwise. */
    Stroke(const Stroke& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, Stroke)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /* functions to be used by OSG serialization only */
    void setLines(osg::DrawArrays* lines);
    const osg::DrawArrays* getLines() const;

    inline void setColor(const osg::Vec4f& color);
    inline const osg::Vec4f& getColor() const;

    bool isCurved() const;
    bool isShadered() const;

    const osg::Program* getProgram() const;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    /*! A method to be used to copy the input stroke's data. It is assumed *this stroke is empty.
     * \param copy is the source stroke to copy from. */
    bool copyFrom(const entity::Stroke* copy);

    /*! A method to add a point to the end of a stroke. It is normally used when constructing a stroke in-motion while sketching.
     * \param u is local U coordinate, \param v is local V coordinate. */
    void appendPoint(const float u, const float v);

    /*! \param i is the point index. \return point coordinates at the specified index. */
    osg::Vec2f getPoint(unsigned int i) const;

    /*! \return observer pointer on camera which helped to create a shadered version.
     * \sa redefineToShader(). */
    osg::Camera* getCamera() const;

    /*! A method that fits the stroke's points to a set of curve using Schneider's algorithm.
     * \return true upon success. */
    bool redefineToCurve(float tolerance = -1.f);

    /*! A method to tune the look of the stroke with smoother connections and thicker linewidth.
     * So that to avoid broken and thin look of the default OpenGL functionality when using GL_LINE_STRIP_ADJACENCY and such. */
    bool redefineToShader(osg::Camera* camera);

    /*! A method to reset the shadered look to the default OpenGL geometry.
     * \return true on success. */
    bool redefineToDefault();

    /*! \return number of vertices. */
    int getNumPoints() const;

    /*! \return length of the stroke, which is measured as a largest dimention of the bounding box around the stroke. */
    float getLength() const;

    /*! \return true if the stroke is longer than allowed threshold, false otherwise. */
    bool isLengthy() const;

    /*! A method to perform translation of the stroke in delta movement.
     * \param du is delta movement in X local axis direction, \param dv is delta movement in Y local axis direction. */
    void moveDelta(double du, double dv);

    /*! A method to scale the stroke around a fixed point.
     * \param scaleX is scaling factor along U local axis, \param scaleY is scaling factor along V local axis,
     * \param center is the local 2D center around which to scale. */
    void scale(double scaleX, double scaleY, osg::Vec3f center);

    /*! A method to uniformely scale the stroke around a fixed point.
     * \param scale is scaling factor, \param center is the local 2D center around which to scale. */
    void scale(double scale, osg::Vec3f center);

    /*! A method to rotate the stroke around a fixed point.
     * \param theta is angle in radians, \param center is the local 2D center around which to rotate. */
    void rotate(double theta, osg::Vec3f center);

    /*! A re-defined method of entity::Entity2D to obtain entity type.
     * \return entity type */
    cher::ENTITY_TYPE getEntityType() const;

protected:
    bool initializeShaderProgram(osg::Camera* camera);
    osg::Vec3Array *interpolateCurves(const osg::Vec3Array *curves, int segments = 11);

private:
    osg::ref_ptr<osg::DrawArrays>   m_lines; // saved to file
    osg::ref_ptr<osg::Program>      m_program;
    osg::observer_ptr<osg::Camera>  m_camera;
    osg::Vec4f                      m_color; // saved to file
    bool                            m_isCurved;
    bool                            m_isShadered;
};
}

#endif // STROKE

