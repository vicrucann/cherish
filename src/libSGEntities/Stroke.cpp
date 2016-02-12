#include "Stroke.h"

#include <assert.h>

#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/BlendFunc>
#include <osg/Point>

entity::Stroke::Stroke()
    : osg::Geometry()
    , m_lines(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP))
    , m_color(osg::Vec4f(0.f, 0.f, 0.f, 1.f))
{
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(dureu::STROKE_CLR_NORMAL);
    osg::Vec3Array* verts = new osg::Vec3Array;

    this->addPrimitiveSet(m_lines.get());
    this->setVertexArray(verts);
    this->setColorArray(colors);
    this->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(dureu::STROKE_LINE_WIDTH);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setAttributeAndModes(new osg::Point(dureu::STROKE_LINE_WIDTH));
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    this->setStateSet(stateset);

    this->setDataVariance(osg::Object::DYNAMIC);
    this->setUseDisplayList(false);
    this->setUseVertexBufferObjects(true);
    this->setName("Stroke");

    noticeMsg("New stroke ctor complete");
}

entity::Stroke::Stroke(const entity::Stroke& copy, const osg::CopyOp& copyop)
    : osg::Geometry(copy, copyop)
    , m_lines(copy.m_lines)
    , m_color(copy.m_color)
{
}

void entity::Stroke::setLines(osg::DrawArrays* lines)
{
    m_lines = lines;
}

const osg::DrawArrays*entity::Stroke::getLines() const
{
    return m_lines.get();
}

void entity::Stroke::setColor(const osg::Vec4f &color)
{
    m_color = color;
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    //(*colors)[0] = color;
    colors->front() = m_color;
    colors->dirty();
    this->dirtyDisplayList();
    this->dirtyBound();
}

const osg::Vec4f&entity::Stroke::getColor() const
{
    return m_color;
}

void entity::Stroke::appendPoint(const float u, const float v)
{
    //osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());

    //colors->push_back(dureu::STROKE_CLR_NORMAL);
    //colors->dirty();

    verts->push_back(osg::Vec3f(u,v,0.f));
    unsigned int sz = verts->size();

    m_lines->setFirst(0);
    m_lines->setCount(sz);

    verts->dirty();
    this->dirtyBound();
    // read more: http://forum.openscenegraph.org/viewtopic.php?t=2190&postdays=0&postorder=asc&start=15
}

void entity::Stroke::removePoints(unsigned int index_start, unsigned int index_end)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());

    if (index_start > verts->size()-1 || index_end > verts->size()-1){
        outErrMsg("Stroke remove edge: indices to remove are out of range");
        return;
    }
    if (index_end == verts->size()-1){
        for (unsigned int i = index_start; i<=index_end; ++i)
            verts->pop_back();
    }
    else if (index_start == 0){
        //verts->erase(verts->begin(), verts->begin()+index_end);
        for (unsigned int i = index_start; i<=index_end; ++i){
            verts->pop_back();
            colors->pop_back();
        }

    }
    else{ // to define later - what to do when it is erased in the middle
        return;
    }

    unsigned int sz = verts->size();
    m_lines->setFirst(0);
    m_lines->setCount(sz);

    verts->dirty();
    colors->dirty();
    this->dirtyBound();
}

float entity::Stroke::getLength() const
{
    osg::BoundingBox bb = this->getBoundingBox();
    if (std::fabs(bb.zMax()-bb.zMin()) > dureu::EPSILON ){
        warningMsg("Stroke->getLength(): z coordinates of a stroke are unexpected values");
        warningVal("zMax", bb.zMax());
        warningVal("zMin", bb.zMin());
        return 0;
    }
    return std::max(bb.xMax() - bb.xMin(), bb.yMax() - bb.yMin());
}

bool entity::Stroke::isLengthy() const
{
    return this->getLength()>dureu::STROKE_MINL;
}

void entity::Stroke::moveDelta(double du, double dv)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i];
        (*verts)[i] = osg::Vec3f(du+vi.x(), dv+vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::Stroke::scale(double s)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i];
        (*verts)[i] = osg::Vec3f(s*vi.x(), s*vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::Stroke::rotate(double theta, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i];
        (*verts)[i] = center + osg::Vec3f(vi.x() * std::cos(theta) - vi.y() * std::sin(theta),
                                          -vi.x() * std::sin(theta) + vi.y() * std::cos(theta), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

/* for serialization of stroke type
 * for more info, see OSG beginner's guide, or
 * OSG cookbook. In both, there is a section on serialization.
*/
REGISTER_OBJECT_WRAPPER(Stroke_Wrapper
                        , new entity::Stroke
                        , entity::Stroke
                        , "osg::Object osg::Geometry entity::Stroke")
{
    ADD_OBJECT_SERIALIZER(Lines, osg::DrawArrays, NULL);
    ADD_VEC4F_SERIALIZER(Color, osg::Vec4f());
}
