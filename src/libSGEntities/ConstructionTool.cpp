#include "ConstructionTool.h"

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#include <osg/StateSet>
#include <osg/LineWidth>
#include <osg/BlendFunc>

#include "Settings.h"

entity::ConstructionTool::ConstructionTool(unsigned int nVerts, osg::Array::Binding colorBind, osg::PrimitiveSet *primitiveSet)
    : osg::Group()
    , m_geodeTool(new osg::Geode)
    , m_geomTool(new osg::Geometry)
{
    osg::Vec3Array* verts = new osg::Vec3Array(nVerts);
    m_geomTool->setVertexArray(verts);
    osg::Vec4Array* clrs = new osg::Vec4Array(nVerts);
    m_geomTool->setColorArray(clrs, colorBind);
    m_geomTool->addPrimitiveSet(primitiveSet);
    m_geodeTool->addDrawable(m_geomTool);
    this->addChild(m_geodeTool);
}

void entity::ConstructionTool::setColor(const osg::Vec4f &color)
{
    osg::Vec4Array* colorTool = static_cast<osg::Vec4Array*>(m_geomTool->getColorArray());
    assert(colorTool->size()>0);
    (*colorTool)[0] = color;
    m_geomTool->dirtyDisplayList();
    m_geomTool->dirtyBound();
}

const osg::Vec4f &entity::ConstructionTool::getColor() const
{
    osg::Vec4Array* colorTool = static_cast<osg::Vec4Array*>(m_geomTool->getColorArray());
    assert(colorTool->size()>0);
    return (*colorTool)[0];
}

entity::ToolNormal::ToolNormal()
    : ConstructionTool(2, osg::Array::BIND_OVERALL,
                       new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2))
{
    this->setName("groupNormal");
    m_geodeTool->setName("geodeNormal");
    m_geomTool->setName("geomNormal");
    this->setColor(dureu::CANVAS_CLR_EDIT);
    this->setVertices(osg::Vec3f(0,0,0), dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0, 0);
}

void entity::ToolNormal::setVertices(const osg::Vec3f &center, float szX, float szY, float , float )
{
    float szN = std::max(szX, szY);
    osg::Vec3Array* vNormal = static_cast<osg::Vec3Array*>(m_geomTool->getVertexArray());
    assert(vNormal->size() == 2);
    (*vNormal)[0] = center;
    (*vNormal)[1] = center + osg::Vec3f(0.f,0.f, szN);
    m_geomTool->dirtyDisplayList();
    m_geomTool->dirtyBound();
}

entity::ToolAxisLocal::ToolAxisLocal()
    : ConstructionTool(4, osg::Array::BIND_PER_VERTEX,
                       new osg::DrawArrays(osg::PrimitiveSet::LINES,0,4))
{
    this->setName("groupAxisLocal");
    m_geodeTool->setName("geodeAxisLocal");
    m_geomTool->setName("geomAxisLocal");
    this->setColor();
    this->setVertices(osg::Vec3f(0,0,0), 0,0,0, dureu::CANVAS_AXIS);
}

void entity::ToolAxisLocal::setColor()
{
    osg::Vec4Array* colorTool = static_cast<osg::Vec4Array*>(m_geomTool->getColorArray());
    assert(colorTool->size() == 4);
    (*colorTool)[0] = solarized::orange;
    (*colorTool)[1] = solarized::orange;
    (*colorTool)[2] = solarized::yellow;
    (*colorTool)[3] = solarized::yellow;
    m_geomTool->dirtyDisplayList();
    m_geomTool->dirtyBound();
}

void entity::ToolAxisLocal::setVertices(const osg::Vec3f &center, float , float ,
                                        float , float szAx)
{
    assert(szAx>=dureu::CANVAS_AXIS);
    osg::Vec3Array* vAxis = static_cast<osg::Vec3Array*>(m_geomTool->getVertexArray());
    assert(vAxis->size() == 4);
    (*vAxis)[0] = center;
    (*vAxis)[1] = center + osg::Vec3(szAx,0.f,0.f);
    (*vAxis)[2] = center;
    (*vAxis)[3] = center + osg::Vec3(0.f, szAx, 0.f);
    m_geomTool->dirtyDisplayList();
    m_geomTool->dirtyBound();
}

entity::ToolFrame::ToolFrame()
    : ConstructionTool(4, osg::Array::BIND_OVERALL,
                       new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4))
    , m_geomPick(new osg::Geometry)
{
    this->setName("groupFrame");
    m_geodeTool->setName("geodeFrame");
    m_geomTool->setName("geomFrame");

    m_geodeTool->addDrawable(m_geomPick);
    m_geomPick->setName("Pickable");
    m_geomPick->setVertexArray(new osg::Vec3Array(4));
    m_geomPick->setColorArray(new osg::Vec4Array(4), osg::Array::BIND_OVERALL);
    m_geomPick->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    this->setColor(dureu::CANVAS_CLR_REST);
    this->setVertices(osg::Vec3f(0,0,0), dureu::CANVAS_MINW, dureu::CANVAS_MINH, dureu::CANVAS_CORNER, 0);
}

void entity::ToolFrame::setVertices(const osg::Vec3f &center, float szX, float szY, float szCr, float)
{
    assert(szX>=dureu::CANVAS_MINW && szY>=dureu::CANVAS_MINH);
    osg::Vec3Array* vFrame = static_cast<osg::Vec3Array*>(m_geomTool->getVertexArray());
    assert(vFrame->size() == 4);
    (*vFrame)[0] = center + osg::Vec3(szX,szY,0.f);
    (*vFrame)[1] = center + osg::Vec3(-szX,szY,0.f);
    (*vFrame)[2] = center + osg::Vec3(-szX,-szY,0.f);
    (*vFrame)[3] = center + osg::Vec3(szX,-szY,0.f);
    m_geomTool->dirtyDisplayList();
    m_geomTool->dirtyBound();

    assert(szCr >= dureu::CANVAS_CORNER);
    osg::Vec3 p0 = (*vFrame)[0];
    osg::Vec3Array* vPick = static_cast<osg::Vec3Array*>(m_geomPick->getVertexArray());
    assert(vPick->size() == 4);
    (*vPick)[0] = p0;
    (*vPick)[1] = p0 + osg::Vec3(-szCr, 0.f, 0.f);
    (*vPick)[2] = p0 + osg::Vec3(-szCr, -szCr, 0.f);
    (*vPick)[3] = p0 + osg::Vec3(.0f, -szCr, 0.f);
    m_geomPick->dirtyDisplayList();
    m_geomPick->dirtyBound();
}

const osg::Vec3Array *entity::ToolFrame::getVertices() const
{
    return static_cast<osg::Vec3Array*>(m_geomTool->getVertexArray());
}

void entity::ToolFrame::setColor(const osg::Vec4f &color)
{
    ConstructionTool::setColor(color);

    osg::Vec4Array* colorPick = static_cast<osg::Vec4Array*>(m_geomPick->getColorArray());
    assert(colorPick->size() > 0);
    (*colorPick)[0] = color;
    m_geomPick->dirtyDisplayList();
    m_geomPick->dirtyBound();
}

entity::ToolBookmark::ToolBookmark(const osg::Vec3d &eye, const osg::Vec3d& center, const osg::Vec3d& up)
    : ConstructionTool(12, osg::Array::BIND_OVERALL,
                       new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,12))
{
    this->setName("groupBookmark");
    m_geodeTool->setName("geodeBookmark");
    m_geomTool->setName("geomBookmark");
    this->setColor(dureu::BOOKMARK_CLR);
    this->setVertices(eye, center, up);

    osg::StateSet* ss = new osg::StateSet;
    osg::LineWidth* lw = new osg::LineWidth;
    lw->setWidth(3.f);
    ss->setAttributeAndModes(lw,osg::StateAttribute::ON);
    ss->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
    ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    m_geomTool->setStateSet(ss);
}

void entity::ToolBookmark::setVertices(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
{
    osg::Vec3d dir = center-eye;
    dir.normalize();
    osg::Vec3d side = dir^up;
    side.normalize();

    osg::Vec3d C = eye + dir * dureu::BOOKMARK_Z;
    osg::Vec3d v1 = C + side * dureu::BOOKMARK_X + up * dureu::BOOKMARK_Y;
    osg::Vec3d v2 = C - side * dureu::BOOKMARK_X + up * dureu::BOOKMARK_Y;
    osg::Vec3d v3 = C - side * dureu::BOOKMARK_X - up * dureu::BOOKMARK_Y;
    osg::Vec3d v4 = C + side * dureu::BOOKMARK_X - up * dureu::BOOKMARK_Y;

    osg::Vec3Array* vFrame = static_cast<osg::Vec3Array*>(m_geomTool->getVertexArray());
    assert(vFrame->size() == 12);
    (*vFrame)[0] = eye;
    (*vFrame)[1] = v1;
    (*vFrame)[2] = v2;
    (*vFrame)[3] = v3;
    (*vFrame)[4] = v4;
    (*vFrame)[5] = v1;
    (*vFrame)[6] = eye;
    (*vFrame)[7] = v2;
    (*vFrame)[8] = eye;
    (*vFrame)[9] = v3;
    (*vFrame)[10] = eye;
    (*vFrame)[11] = v4;
    m_geomTool->dirtyDisplayList();
    m_geomTool->dirtyBound();
}

void entity::ToolBookmark::setVertices(const osg::Vec3f &, float, float, float, float)
{
}

entity::ToolIntersectionLine::ToolIntersectionLine(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4)
    : ConstructionTool(4, osg::Array::BIND_OVERALL,
                       new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,4))
{
    this->setName("groupIntersectionLine");
    m_geodeTool->setName("geodeIntersectionLine");
    m_geomTool->setName("geomIntersectionLine");
    this->setColor(dureu::CANVAS_CLR_INTERSECTION);
    this->setVertices(P1,P2,P3,P4);

    osg::StateSet* ss = new osg::StateSet;
    osg::LineWidth* lw = new osg::LineWidth;
    lw->setWidth(dureu::CANVAS_LINE_WIDTH);
    ss->setAttributeAndModes(lw,osg::StateAttribute::ON);
    ss->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
    ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    m_geomTool->setStateSet(ss);
}

void entity::ToolIntersectionLine::setVertices(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4)
{
    osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(m_geomTool->getVertexArray());
    assert(vertices->size() == 4);
    (*vertices)[0] = P1;
    (*vertices)[1] = P2;
    (*vertices)[2] = P3;
    (*vertices)[3] = P4;
    m_geomTool->dirtyDisplayList();
    m_geomTool->dirtyBound();
}
