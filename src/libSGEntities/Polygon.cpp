#include "Polygon.h"

#include <QDebug>
#include <QtGlobal>

#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/StateSet>
#include <osg/Point>
#include <osg/Texture2D>

entity::Polygon::Polygon()
    : entity::Entity2D()
    , m_lines(new osg::DrawArrays(GL_LINE_STRIP))
    , m_color(cher::POLYGON_CLR_PHANTOM)
{
    osg::Vec4Array* colors = new osg::Vec4Array;
    osg::Vec3Array* verts = new osg::Vec3Array;

    this->addPrimitiveSet(m_lines.get());
    this->setVertexArray(verts);
    this->setColorArray(colors);
    this->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    this->setDataVariance(osg::Object::DYNAMIC);
    this->setUseDisplayList(false);
    this->setUseVertexBufferObjects(true);
    this->setName("Polygon");

    qDebug("New polygon ctor complete");
}

entity::Polygon::Polygon(const entity::Polygon &copy, const osg::CopyOp &copyop)
    : entity::Entity2D(copy, copyop)
    , m_lines(copy.m_lines)
    , m_color(copy.m_color)
{

}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void entity::Polygon::setLines(osg::DrawArrays *lines)
{
    m_lines = lines;
}

const osg::DrawArrays *entity::Polygon::getLines() const
{
    return m_lines.get();
}

void entity::Polygon::setColor(const osg::Vec4f &color)
{
    m_color = color;
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    for (unsigned int i=0; i<colors->size(); ++i){
        (*colors)[i] = m_color;
    }
    colors->dirty();
    this->dirtyDisplayList();
    this->dirtyBound();
}

const osg::Vec4f &entity::Polygon::getColor() const
{
    return m_color;
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

bool entity::Polygon::copyFrom(const entity::Polygon *copy)
{
    qDebug("About to copy polygon");
    if (!copy){
        qWarning("Copy polygon is NULL");
        return false;
    }

    if (this->getColor() != cher::POLYGON_CLR_PHANTOM ||
            !this->getLines() ||
            static_cast<int>(this->getLines()->getMode()) != GL_LINE_STRIP) {
        qWarning("polygon::copyFrom() : polygon parameters check failed");
        return false;
    }

    if (this->getNumPoints() != 0 || copy->getNumPoints() == 0){
        qWarning("polygon::copyFrom() : polygon size check failed");
        return false;
    }

//    if (copy->getIsShadered() && copy->getNumPoints() % 4 != 0){
//        qWarning("Copy stroke vertex number must be divadable to 4");
//        return false;
//    }

    for (int i=0; i<copy->getNumPoints(); i++){
        osg::Vec2f p = copy->getPoint(i);
        this->appendPoint(p.x(), p.y());
    }
//    this->setProgram(copy->getProgram());
//    this->setIsCurved(copy->getIsCurved());
    this->redefineToPolygon();

//    if (copy->getIsCurved() == this->getIsCurved() &&
//            copy->getIsShadered() == this->getIsShadered() &&
//            copy->getNumPoints() != this->getNumPoints()){
//        qWarning("Stroke copy failed : number of points mismatch");
//        return false;
//    }

    return true;
}

void entity::Polygon::appendPoint(const float u, const float v)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    colors->push_back(m_color);
    colors->dirty();

    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    verts->push_back(osg::Vec3f(u,v,0.f));
    unsigned int sz = verts->size();

    m_lines->setFirst(0);
    m_lines->setCount(sz);

    verts->dirty();
    this->dirtyBound();
    // read more: http://forum.openscenegraph.org/viewtopic.php?t=2190&postdays=0&postorder=asc&start=15
}

void entity::Polygon::editLastPoint(float u, float v)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    Q_ASSERT(verts != 0);
    (*verts)[verts->size()-1] = osg::Vec3f(u, v, 0.f);

    verts->dirty();
    this->dirtyBound();
}

osg::Vec2f entity::Polygon::getPoint(unsigned int i) const
{
    const osg::Vec3Array* verts = static_cast<const osg::Vec3Array*>(this->getVertexArray());
    if (!verts){
        qWarning("Stroke vertices are not initialized. Cannot obtain a point.");
        return osg::Vec2f(0.f, 0.f);
    }
    unsigned int sz = verts->size();
    if (i>=sz){
        qWarning("Stroke's index is out of range. Cannot obtain  a point");
        return osg::Vec2f(0.f, 0.f);
    }
    osg::Vec3f p = (*verts)[i];
    if (std::fabs(p.z()) > cher::EPSILON){
        qWarning("Stroke::getPoint : unexpected value of z-coordinate");
        qInfo() << p.z();
        return osg::Vec2f(0.f, 0.f);
    }

    return osg::Vec2f(p.x(), p.y());
}

void entity::Polygon::redefineToPolygon()
{
    if (m_lines->getMode() == GL_POLYGON) return;
    osg::ref_ptr<osg::Vec3Array> points = static_cast<osg::Vec3Array*>(this->getVertexArray());
    Q_ASSERT(points);
    m_lines->set(GL_POLYGON, 0, this->getNumPoints());
    points->dirty();
    this->setColor(solarized::violet); // replace to NORMALFILL
}

int entity::Polygon::getNumPoints() const
{
    const osg::Vec3Array* verts = static_cast<const osg::Vec3Array*>(this->getVertexArray());
    Q_ASSERT(verts);

    return static_cast<int>(verts->size());
}

void entity::Polygon::moveDelta(double du, double dv)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i];
        (*verts)[i] = osg::Vec3f(du+vi.x(), dv+vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::Polygon::scale(double scaleX, double scaleY, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(scaleX*vi.x(), scaleY*vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::Polygon::scale(double scale, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(scale*vi.x(), scale*vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::Polygon::rotate(double theta, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(vi.x() * std::cos(theta) - vi.y() * std::sin(theta),
                                          vi.x() * std::sin(theta) + vi.y() * std::cos(theta), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

cher::ENTITY_TYPE entity::Polygon::getEntityType() const
{
    return cher::ENTITY_POLYGON;
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
