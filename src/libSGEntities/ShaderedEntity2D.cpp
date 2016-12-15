#include "ShaderedEntity2D.h"

#include <QtGlobal>
#include <QDebug>

#include "Settings.h"
#include "MainWindow.h"

entity::ShaderedEntity2D::ShaderedEntity2D(unsigned int drawing, AttributeBinding binding, const std::string &name, const osg::Vec4f &color)
    : entity::Entity2D()
    , m_lines(new osg::DrawArrays(drawing))
    , m_program(0)
    , m_isShadered(false)
    , m_color(color)
{
    osg::Vec4Array* colors = new osg::Vec4Array;
    osg::Vec3Array* verts = new osg::Vec3Array;

    this->addPrimitiveSet(m_lines.get());
    this->setVertexArray(verts);
    this->setColorArray(colors);
    this->setColorBinding(binding);

    this->setDataVariance(osg::Object::DYNAMIC);
    this->setUseDisplayList(false);
    this->setUseVertexBufferObjects(true);
    this->setName(name);

    qDebug() << "New empty entity created of type: " << QString(this->getName().c_str());
}

entity::ShaderedEntity2D::ShaderedEntity2D(const entity::ShaderedEntity2D &copy, const osg::CopyOp &copyop)
    : entity::Entity2D(copy, copyop)
    , m_lines(copy.m_lines)
    , m_program(copy.m_program)
    , m_isShadered(copy.m_isShadered)
    , m_color(copy.m_color)
{
}

void entity::ShaderedEntity2D::initializeProgram(ProgramEntity2D *p, unsigned int mode)
{
    /* save program pointer to be used later */
    if (!p) throw std::runtime_error("initializeProgram(): Program is NULL");
    m_program = p;

    /* initial values for geometries */
    osg::Vec3Array* points = static_cast<osg::Vec3Array*>(this->getVertexArray());
    if (!points) throw std::runtime_error("initializeProgram(): points are  NULL");
    m_lines->set(mode, 0, points->size());

    /* to disable Stroke shader program, e.g., if it is phantom stroke, override with an empty program
     * The OFF option would not work for the already established program (that is attached to Canvas::m_geodeStrokes), for
     * more details see: http://forum.openscenegraph.org/viewtopic.php?t=11783&view=previous */
    this->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::PROTECTED);
    m_isShadered = false;
}

bool entity::ShaderedEntity2D::copyFrom(const entity::ShaderedEntity2D *copy)
{
    if (!copy || !this->getLines()) return false;
    if (this->getNumPoints() != 0 || copy->getNumPoints() == 0) return false;

    for (int i=0; i<copy->getNumPoints(); i++){
        osg::Vec2f p = copy->getPoint(i);
        this->appendPoint(p.x(), p.y(), copy->getColor());
    }

    this->setProgram(copy->getProgram());
    if (copy->getProgram())
        this->redefineToShape(copy->getProgram()->getTransform());

    return true;
}

void entity::ShaderedEntity2D::appendPoint(const float u, const float v, osg::Vec4f color)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    colors->push_back(color);
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

osg::Vec2f entity::ShaderedEntity2D::getPoint(unsigned int i) const
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

//bool entity::ShaderedEntity2D::redefineToShape(osg::MatrixTransform *t)
//{
//    // if shader is connected, it means this function was already called
//    if (m_isShadered) return true;

//    if (this->redefineToShader(t==0? MainWindow::instance().getCanvasCurrent()->getTransform() : t)){
//        m_isShadered = true;
//        qDebug("Redefined to shader successfully");
//    }
//    else
//    {
//        qCritical("Could not re-define to shader, update graphics drivers so that it supports OpenGL 3.3");
//        m_isShadered = false;
//    }

//    /* if necessary, update the sizing */
//    osg::Vec3Array* finalPts = static_cast<osg::Vec3Array*>(this->getVertexArray());
//    if (finalPts){
//        m_lines->setFirst(0);
//        m_lines->setCount(finalPts->size());
//        finalPts->dirty();
//        osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
//        if (colors){
//            colors->resize(finalPts->size(), m_color);
//            colors->dirty();
//        }
//    }
//    else
//        qCritical("Unable to update geometry correctly");

//    this->dirtyBound();

//    return true;
//}

int entity::ShaderedEntity2D::getNumPoints() const
{
    const osg::Vec3Array* verts = static_cast<const osg::Vec3Array*>(this->getVertexArray());
    Q_CHECK_PTR(verts);

    return static_cast<int>(verts->size());
}

void entity::ShaderedEntity2D::moveDelta(double du, double dv)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i];
        (*verts)[i] = osg::Vec3f(du+vi.x(), dv+vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::ShaderedEntity2D::scale(double scaleX, double scaleY, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(scaleX*vi.x(), scaleY*vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::ShaderedEntity2D::scale(double scale, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(scale*vi.x(), scale*vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::ShaderedEntity2D::rotate(double theta, osg::Vec3f center)
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void entity::ShaderedEntity2D::setLines(osg::DrawArrays *lines)
{
    m_lines = lines;
}

const osg::DrawArrays *entity::ShaderedEntity2D::getLines() const
{
    return m_lines.get();
}

void entity::ShaderedEntity2D::setColor(const osg::Vec4f &color)
{
    m_color = color;
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    if (!colors) throw std::runtime_error("setColors: colors is NULL");

    for (unsigned int i=0; i<colors->size(); ++i){
        (*colors)[i] = m_color;
    }
    colors->dirty();
    this->dirtyDisplayList();
    this->dirtyBound();
}

const osg::Vec4f &entity::ShaderedEntity2D::getColor() const
{
    return m_color;
}

void entity::ShaderedEntity2D::setIsShadered(bool shadered)
{
    m_isShadered = shadered;
}

bool entity::ShaderedEntity2D::getIsShadered() const
{
    return m_isShadered;
}

void entity::ShaderedEntity2D::setProgram(ProgramEntity2D *p)
{
    m_program = p;
}

ProgramEntity2D *entity::ShaderedEntity2D::getProgram() const
{
    return m_program.get();
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
