#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include "Settings.h"
#include "Entity2D.h"
#include "ShaderedEntity2D.h"
#include "libSGControls/ProgramLineSegment.h"

#include "osg/Geometry"
#include "osgDB/ObjectWrapper"

namespace entity {

/*! \class LineSegment
 * \brief Geometry class that represents a line segment in 3D space.
 */
class LineSegment : public entity::ShaderedEntity2D
{
public:
    /*! Constructor creates empty line segment. */
    LineSegment();

    /*! Copy constructor, only used for serialization. */
    LineSegment(const LineSegment& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, LineSegment)

    virtual bool redefineToShape(osg::MatrixTransform *t = 0);

    virtual void appendPoint(float u, float v);

    void editLastPoint(float u, float v);

    osg::Node* getMeshRepresentation() const;

protected:
    virtual bool redefineToShader(osg::MatrixTransform *t);

public:
    virtual cher::ENTITY_TYPE getEntityType() const;
    virtual ProgramLineSegment* getProgram() const;

}; // class LineSegment

} // namespace entity



#endif // LINESEGMENT_H
