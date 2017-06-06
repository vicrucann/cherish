#ifndef SHADEREDENTITY2D_H
#define SHADEREDENTITY2D_H

#include <string>
#include <osg/Geometry>
#include <osg/Program>
#include <osg/MatrixTransform>

#include "Entity2D.h"
#include "libSGControls/ProgramEntity2D.h"
#include "Settings.h"

namespace entity{

/*! \class ShaderedEntity2D
 * \brief Abstract class for all the shaderized entities, i.e., strokes, polygons and line segments.
*/
class ShaderedEntity2D : public entity::Entity2D
{
public:
    /*! Constructor that creates an empty shadered entity */
    ShaderedEntity2D(unsigned int drawing, osg::Geometry::AttributeBinding binding,
                     const std::string& name, const osg::Vec4f& color);

    /*! Copy constructor to be used only for serializatiob purposes. */
    ShaderedEntity2D(const entity::ShaderedEntity2D& copy, const osg::CopyOp& copyop);

    /*! A method tok be called after it is initialized. It initializes the program that entity is going to use
     * for shadering. */
    virtual void initializeProgram(ProgramEntity2D* p, unsigned int mode = GL_LINE_STRIP);

    /*! A method to be used to copy the input geometry data. It is assumed *this is empty.
     * \param copy is the source geometry to copy from. */
    virtual bool copyFrom(const entity::ShaderedEntity2D* copy);

    /*! A method to add a point to the end of the entity. It is normally used when constructing an emtity in-motion while sketching.
     * \param u is local U coordinate, \param v is local V coordinate. */
    virtual void appendPoint(const float u, const float v, osg::Vec4f color);

    /*! \param i is the point index. \return point coordinates at the specified index. */
    virtual osg::Vec2f getPoint(unsigned int i) const;

    /*! A method that changed geometry type, e.g. from polyline to polygon. Is used after the user is
     * finished with sketching and now the entity's look can be re-defined as it will appear on the scene permanately.
     * \param t is the Canvas matrix transform. If none is provided, the transform of the current canvas is taken.
     * \return true upon success. */
    virtual bool redefineToShape(osg::MatrixTransform* t = 0) = 0;

    /*! \return number of vertices. */
    int getNumPoints() const;

protected:
    /*! A method to tune the look of the entity with shader effects. */
    virtual bool redefineToShader(osg::MatrixTransform* t) = 0;

public:
    /*! A method to perform translation of the stroke in delta movement.
     * \param du is delta movement in X local axis direction, \param dv is delta movement in Y local axis direction. */
    virtual void moveDelta(double du, double dv);

    /*! A method to scale the stroke around a fixed point.
     * \param scaleX is scaling factor along U local axis, \param scaleY is scaling factor along V local axis,
     * \param center is the local 2D center around which to scale. */
    virtual void scale(double scaleX, double scaleY, osg::Vec3f center);

    /*! A method to uniformely scale the stroke around a fixed point.
     * \param scale is scaling factor, \param center is the local 2D center around which to scale. */
    virtual void scale(double scale, osg::Vec3f center);

    /*! A method to rotate the stroke around a fixed point.
     * \param theta is angle in radians, \param center is the local 2D center around which to rotate. */
    virtual void rotate(double theta, osg::Vec3f center);

    virtual cher::ENTITY_TYPE getEntityType() const = 0;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void setLines(osg::DrawArrays* lines);
    const osg::DrawArrays* getLines() const;

    void setColor(const osg::Vec4f& color);
    const osg::Vec4f& getColor() const;

    /*! \param alpha is transparency channel. */
    void setSelected(float alpha);

    /*! \param alpha is transparency channel. */
    void setUnselected(float alpha);

    void setIsShadered(bool shadered);
    bool getIsShadered() const;

    virtual void setProgram(ProgramEntity2D* p);
    virtual ProgramEntity2D* getProgram() const;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

protected:
    osg::ref_ptr<osg::DrawArrays>       m_lines;
    osg::observer_ptr<ProgramEntity2D>  m_program;
    bool                                m_isShadered;
    osg::Vec4f                          m_colorNormal, m_colorSelected;

}; // class ShaderedEntity2D

} // namespace entity

#endif // SHADEREDENTITY2D_H
