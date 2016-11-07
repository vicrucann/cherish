#include "Polygon.h"

#include <QDebug>
#include <QtGlobal>

#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/StateSet>
#include <osg/Point>
#include <osg/Texture2D>

#include "MainWindow.h"

const GLenum POLYGON_PHANTOM_TYPE = GL_LINE_STRIP;

entity::Polygon::Polygon()
    : entity::ShaderedEntity2D(POLYGON_PHANTOM_TYPE, osg::Geometry::BIND_PER_VERTEX, "Polygon", cher::POLYGON_CLR_PHANTOM)
{
}

entity::Polygon::Polygon(const entity::Polygon &copy, const osg::CopyOp &copyop)
    : entity::ShaderedEntity2D(copy, copyop)
{
}

bool entity::Polygon::copyFrom(const ShaderedEntity2D *copy)
{
    const entity::Polygon* poly = dynamic_cast<const entity::Polygon*>(copy);
    if (!poly){
        qWarning("Could not dynamic cast to Polygon, no copy will be done");
        return false;
    }

    if (!entity::ShaderedEntity2D::copyFrom(copy))
        qCritical("Polygon copy has failed");

    return true;
}

void entity::Polygon::editLastPoint(float u, float v)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    Q_ASSERT(verts != 0);
    (*verts)[verts->size()-1] = osg::Vec3f(u, v, 0.f);

    verts->dirty();
    this->dirtyBound();
}

void entity::Polygon::removeLastPoint()
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    Q_ASSERT(verts != 0);
    verts->pop_back();
    verts->dirty();

    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    Q_ASSERT(colors);
    colors->pop_back();
    colors->dirty();

    m_lines->setFirst(0);
    m_lines->setCount(verts->size());

    this->dirtyBound();
}

bool entity::Polygon::redefineToShape(osg::MatrixTransform *t)
{
    if (m_isShadered) return true;

    if (this->redefineToShader(t==0? MainWindow::instance().getCanvasCurrent()->getTransform() : t))
    {
        m_isShadered = true;
    }

    osg::ref_ptr<osg::Vec3Array> points = static_cast<osg::Vec3Array*>(this->getVertexArray());
    Q_ASSERT(points);
    points->dirty();
    osg::Vec4f color = MainWindow::instance().getCurrentColor();
    this->setColor(color);

    return true;
}

bool entity::Polygon::isPolygon() const
{
    return m_lines->getMode() == GL_POLYGON;
}

void entity::Polygon::appendPoint(const float u, const float v)
{
    entity::ShaderedEntity2D::appendPoint(u,v, cher::POLYGON_CLR_PHANTOM);
}

cher::ENTITY_TYPE entity::Polygon::getEntityType() const
{
    return cher::ENTITY_POLYGON;
}

ProgramPolygon *entity::Polygon::getProgram() const
{
    return dynamic_cast<ProgramPolygon*>(m_program.get());
}

bool entity::Polygon::redefineToShader(osg::MatrixTransform *t)
{
    if (!m_program) return false;
    if (m_program.get())
        m_program->updateTransform(t);

    /* The used shader requires that each line segment is represented as GL_LINES_AJACENCY_EXT */
    osg::ref_ptr<osg::Vec3Array> points = static_cast<osg::Vec3Array*>(this->getVertexArray());
    if (!points) return false;

    /* reset the primitive type */
    m_lines->set(GL_POLYGON, 0, this->getNumPoints());

    /* set shader attributes */
    this->setVertexAttribArray(0, points, osg::Array::BIND_PER_VERTEX);
    osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(this->getColorArray());
    Q_ASSERT(colors);
    this->setVertexAttribArray(1, colors, osg::Array::BIND_PER_VERTEX);

    /* apply shader to the state set */
    Q_ASSERT(this->getOrCreateStateSet());
    this->getOrCreateStateSet()->setAttributeAndModes(m_program.get(), osg::StateAttribute::ON);

    m_isShadered = true;
    return true;
}


/* for serialization of entity::Polygon class
 * for more info, see OSG beginner's guide, or
 * OSG cookbook. In both, there is a section on serialization.
*/
REGISTER_OBJECT_WRAPPER(Polygon_Wrapper
                        , new entity::Polygon
                        , entity::Polygon
                        , "osg::Object osg::Geometry entity::Polygon" )
{
    ADD_OBJECT_SERIALIZER(Lines, osg::DrawArrays, NULL);
    ADD_VEC4F_SERIALIZER(Color, osg::Vec4f());
}
