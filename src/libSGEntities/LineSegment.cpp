#include "LineSegment.h"

#include <QDebug>
#include <QtGlobal>

#include <osg/LineWidth>
#include <osg/StateSet>

#include "MainWindow.h"

const GLenum LINESEMENT_PHANTOM_TYPE = GL_LINES;

entity::LineSegment::LineSegment()
    : entity::ShaderedEntity2D(LINESEMENT_PHANTOM_TYPE,
                               osg::Geometry::BIND_PER_VERTEX,
                               "LineSegment",
                               cher::STROKE_CLR_NORMAL)
{
}

entity::LineSegment::LineSegment(const entity::LineSegment &copy, const osg::CopyOp &copyop)
    : entity::ShaderedEntity2D(copy, copyop)
{

}

bool entity::LineSegment::redefineToShape(osg::MatrixTransform *t)
{
    if (m_isShadered)
        return true;

    if (this->redefineToShader(t==0? MainWindow::instance().getCanvasCurrent()->getTransform() : t))
    {
        m_isShadered = true;
    }

    osg::ref_ptr<osg::Vec3Array> points = static_cast<osg::Vec3Array*>(this->getVertexArray());
    Q_CHECK_PTR(points);
    points->dirty();

    return true;
}

void entity::LineSegment::appendPoint(float u, float v)
{
    entity::ShaderedEntity2D::appendPoint(u,v, cher::STROKE_CLR_NORMAL);
}

void entity::LineSegment::editLastPoint(float u, float v)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    Q_CHECK_PTR(verts);
    (*verts)[verts->size()-1] = osg::Vec3f(u, v, 0.f);

    verts->dirty();
    this->dirtyBound();
}

bool entity::LineSegment::redefineToShader(osg::MatrixTransform *t)
{
    if (!m_program) return false;
    m_program->updateTransform(t);

    // make sure the primitive type is line segment
    m_lines->set(GL_LINES, 0, this->getNumPoints());

    // set shader attributes
    osg::ref_ptr<osg::Vec3Array> points = static_cast<osg::Vec3Array*>(this->getVertexArray());
    if (!points) return false;
    this->setVertexAttribArray(0, points, osg::Array::BIND_PER_VERTEX);
    osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(this->getColorArray());
    Q_CHECK_PTR(colors);
    this->setVertexAttribArray(1, colors, osg::Array::BIND_PER_VERTEX);

    /* apply shader to the state set */
    Q_ASSERT(this->getOrCreateStateSet());
    this->getOrCreateStateSet()->setAttributeAndModes(m_program.get(),
                                                      osg::StateAttribute::ON);

    m_isShadered = true;
    return true;
}

cher::ENTITY_TYPE entity::LineSegment::getEntityType() const
{
    return cher::ENTITY_LINESEGMENT;
}

ProgramLineSegment *entity::LineSegment::getProgram() const
{
    return dynamic_cast<ProgramLineSegment*>(m_program.get());
}

REGISTER_OBJECT_WRAPPER(LineSegment_Wrapper
                        , new entity::LineSegment
                        , entity::LineSegment
                        , "osg::Object osg::Geometry entity::LineSegment")
{
    ADD_OBJECT_SERIALIZER(Lines, osg::DrawArrays, NULL);
    ADD_VEC4F_SERIALIZER(Color, osg::Vec4f());
}
