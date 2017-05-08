#include "LineSegment.h"

#include <QDebug>
#include <QtGlobal>

#include <osg/LineWidth>
#include <osg/StateSet>

#include "MainWindow.h"

const GLenum LINESEMENT_PHANTOM_TYPE = GL_LINE;

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
    m_program->updateTransform(t);
    this->getOrCreateStateSet()->setAttributeAndModes(m_program.get(), osg::StateAttribute::ON);
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
