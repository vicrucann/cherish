#include "ToolGlobal.h"
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/LineStipple>

#include <assert.h>

#include "Settings.h"


entity::ToolGlobal::ToolGlobal(int nVerts, osg::Array::Binding colorBind, osg::PrimitiveSet *primitiveSet, float linewidth)
    : osg::Group()
    , m_switch(new osg::Switch)
    , m_geode(new osg::Geode)
    , m_geometry(new osg::Geometry)
{
    osg::Vec3Array* verts = new osg::Vec3Array(nVerts);
    m_geometry->setVertexArray(verts);

    osg::Vec4Array* clrs = new osg::Vec4Array(nVerts);
    m_geometry->setColorArray(clrs, colorBind);

    osg::StateSet* ss = new osg::StateSet;
    osg::LineWidth* lw = new osg::LineWidth;
    lw->setWidth(linewidth);
    ss->setAttributeAndModes(lw,osg::StateAttribute::ON);
    ss->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
    ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    m_geometry->setStateSet(ss);
    m_geometry->addPrimitiveSet(primitiveSet);

    m_geode->addDrawable(m_geometry);

    this->addChild(m_switch);
}

void entity::ToolGlobal::setVertices(const std::vector<osg::Vec3f> &source)
{
    osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(m_geometry->getVertexArray());
    assert(vertices->size() == source.size());
    for (size_t i=0; i<vertices->size(); ++i)
        (*vertices)[i] = source[i];
    this->updateGeometry();
}

const osg::Vec3Array *entity::ToolGlobal::getVertices() const
{
    return static_cast<osg::Vec3Array*>(m_geometry->getVertexArray());
}

void entity::ToolGlobal::setColor(const osg::Vec4f &color)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geometry->getColorArray());
    assert(colors->size()>0);
    (*colors)[0] = color;
    this->updateGeometry();
}

const osg::Vec4f &entity::ToolGlobal::getColor() const
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geometry->getColorArray());
    assert(colors->size()>0);
    return (*colors)[0];
}

void entity::ToolGlobal::updateGeometry()
{
    m_geometry->dirtyDisplayList();
    m_geometry->dirtyBound();
}

entity::BookmarkTool::BookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
    : ToolGlobal(12, osg::Array::BIND_OVERALL,
           new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,12))
    , m_AT(new osg::AutoTransform)
{
    this->setColor(dureu::BOOKMARK_CLR);

    std::vector<osg::Vec3f> verts;
    osg::Vec3d dir = center-eye;
    dir.normalize();

    /* since auto transform is activated, we have to move the geometry
     * so that it is located in front on the camera
     * the used autoscale, scales the geometry in relation to the screen */
    osg::Vec3f eye_mod = eye + dir*0.5;

    osg::Vec3d side = dir^up;
    side.normalize();
    osg::Vec3d C = eye_mod + dir * dureu::BOOKMARK_Z;
    osg::Vec3d v1 = C + side * dureu::BOOKMARK_X + up * dureu::BOOKMARK_Y;
    osg::Vec3d v2 = C - side * dureu::BOOKMARK_X + up * dureu::BOOKMARK_Y;
    osg::Vec3d v3 = C - side * dureu::BOOKMARK_X - up * dureu::BOOKMARK_Y;
    osg::Vec3d v4 = C + side * dureu::BOOKMARK_X - up * dureu::BOOKMARK_Y;
    verts.push_back(eye_mod);
    verts.push_back(v1);
    verts.push_back(v2);
    verts.push_back(v3);
    verts.push_back(v4);
    verts.push_back(v1);
    verts.push_back(eye_mod);
    verts.push_back(v2);
    verts.push_back(eye_mod);
    verts.push_back(v3);
    verts.push_back(eye_mod);
    verts.push_back(v4);
    this->setVertices(verts);

    m_AT->setAutoScaleToScreen(true);
    m_AT->setPosition(eye_mod);

    this->initializeSG();
    this->setVisibility(true);
}

void entity::BookmarkTool::initializeSG()
{
    m_switch->addChild(m_AT);
    m_AT->addChild(m_geode);
}

void entity::BookmarkTool::setVisibility(bool on)
{
    m_switch->setChildValue(m_AT, on);
}

bool entity::BookmarkTool::getVisibility() const
{
    return m_switch->getChildValue(m_AT);
}

entity::AxisGlobalTool::AxisGlobalTool()
    : ToolGlobal(6, osg::Array::BIND_PER_VERTEX,
           new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,6))
    , m_AT(new osg::AutoTransform)
    , m_camera(new osg::Camera)
{
    this->setColor( dureu::AXES_CLR_X,  dureu::AXES_CLR_Y,  dureu::AXES_CLR_Z);

    osg::Vec3 corner = osg::Vec3(0.0f,0.0f,0.0f);
    osg::Vec3 xdir = osg::Vec3(dureu::AXES_SIZE,0.0f,0.0f);
    osg::Vec3 ydir = osg::Vec3(0.0f,dureu::AXES_SIZE,0.0f);
    osg::Vec3 zdir = osg::Vec3(0.0f,0.0f,dureu::AXES_SIZE);
    std::vector<osg::Vec3f> verts;
    verts.push_back(corner);
    verts.push_back(corner+xdir);
    verts.push_back(corner);
    verts.push_back(corner+ydir);
    verts.push_back(corner);
    verts.push_back(corner+zdir);
    this->setVertices(verts);

    this->initializeSG();
    this->setVisibility(true);
}

void entity::AxisGlobalTool::initializeSG()
{
    m_AT->setAutoScaleToScreen(true);
    m_AT->setPosition(osg::Vec3d(0,0,0));

    m_camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    m_camera->setRenderOrder(osg::Camera::POST_RENDER);

    m_switch->addChild(m_camera, true);
    m_camera->addChild(m_AT);
    m_AT->addChild(m_geode);
}

void entity::AxisGlobalTool::setVisibility(bool on)
{
    m_switch->setChildValue(m_camera, on);
}

bool entity::AxisGlobalTool::getVisibility() const
{
    return m_switch->getChildValue(m_camera);
}

void entity::AxisGlobalTool::setColor(const osg::Vec4f c1, const osg::Vec4f c2, const osg::Vec4f c3)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geometry->getColorArray());
    (*colors)[0] = c1;
    (*colors)[1] = c1;
    (*colors)[2] = c2;
    (*colors)[3] = c2;
    (*colors)[4] = c3;
    (*colors)[5] = c3;
    this->updateGeometry();
}

entity::FrameTool::FrameTool()
    : ToolGlobal(4, osg::Array::BIND_OVERALL, new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0,4), 1)
    , m_geodePickable(new osg::Geode)
    , m_geodeIntersect(new osg::Geode)
    , m_geomPickable(new osg::Geometry)
    , m_geomIntersect(new osg::Geometry)
{
    this->initializeSG();
    this->setColor(dureu::CANVAS_CLR_REST);
    this->setVisibility(true);
}

void entity::FrameTool::initializeSG()
{
    /* pickable settings */
    m_geomPickable->setVertexArray(new osg::Vec3Array(4));
    m_geomPickable->setColorArray(new osg::Vec4Array(4), osg::Array::BIND_OVERALL);
    m_geomPickable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    /* intersection settings */
    osg::LineStipple* ls = new osg::LineStipple;
    ls->setFactor(1);
    ls->setPattern(0xf00f);

    m_geomIntersect->setVertexArray(new osg::Vec3Array(4));
    m_geomIntersect->setColorArray(new osg::Vec4Array(4), osg::Array::BIND_OVERALL);
    m_geomIntersect->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0,4));
    m_geomIntersect->getOrCreateStateSet()->setAttributeAndModes(ls, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    /* scene graph structure */
    m_geodePickable->addDrawable(m_geomPickable);
    m_geodeIntersect->addDrawable(m_geomIntersect);

    m_switch->addChild(m_geode);
    m_switch->addChild(m_geodePickable);
    m_switch->addChild(m_geodeIntersect);
}

void entity::FrameTool::setVisibility(bool on)
{
    m_switch->setChildValue(m_geode, on);
    m_switch->setChildValue(m_geodePickable, on);
    m_switch->setChildValue(m_geodeIntersect, on);
}

bool entity::FrameTool::getVisibility() const
{
    return m_switch->getChildValue(m_geode);
}

void entity::FrameTool::setVertices(const osg::Vec3f &center, float szX, float szY, float szCr, float szAx)
{
    std::vector<osg::Vec3f> verts;
    verts.push_back(center + osg::Vec3(szX,szY,0.f));
    verts.push_back(center + osg::Vec3(-szX,szY,0.f));
    verts.push_back(center + osg::Vec3(-szX,-szY,0.f));
    verts.push_back(center + osg::Vec3(szX,-szY,0.f));
    ToolGlobal::setVertices(verts);

    osg::Vec3f p0 = verts.at(0);
    this->setPickable(p0, szCr);
}

void entity::FrameTool::setColor(const osg::Vec4f &color, const osg::Vec4f &colorIntersect)
{
    ToolGlobal::setColor(color);
    this->setColorPickable(color);
    this->setColorIntersection(colorIntersect);
}

void entity::FrameTool::setIntersection(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4)
{
    if (P1.isNaN() || P2.isNaN() || P3.isNaN() || P4.isNaN()) return;
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(m_geomIntersect->getVertexArray());
    assert(verts->size() == 4);
    (*verts)[0] = P1;
    (*verts)[1] = P2;
    (*verts)[2] = P3;
    (*verts)[3] = P4;

    m_geomIntersect->dirtyDisplayList();
    m_geomIntersect->dirtyBound();
}

void entity::FrameTool::setColorIntersection(const osg::Vec4f &colorIntersect)
{
    osg::Vec4Array* colorInter = static_cast<osg::Vec4Array*>(m_geomIntersect->getColorArray());
    assert(colorInter->size() > 0);
    (*colorInter)[0] = colorIntersect;
    m_geomIntersect->dirtyDisplayList();
    m_geomIntersect->dirtyBound();
}

void entity::FrameTool::setPickable(const osg::Vec3f &p0, float szCr)
{
    osg::Vec3Array* vPick = static_cast<osg::Vec3Array*>(m_geomPickable->getVertexArray());
    assert(vPick->size() == 4);
    (*vPick)[0] = p0;
    (*vPick)[1] = p0 + osg::Vec3(-szCr, 0.f, 0.f);
    (*vPick)[2] = p0 + osg::Vec3(-szCr, -szCr, 0.f);
    (*vPick)[3] = p0 + osg::Vec3(.0f, -szCr, 0.f);
    this->updateGeometryPickable();
}

void entity::FrameTool::setColorPickable(const osg::Vec4f &color)
{
    osg::Vec4Array* colorPick = static_cast<osg::Vec4Array*>(m_geomPickable->getColorArray());
    assert(colorPick->size() > 0);
    (*colorPick)[0] = color;
    this->updateGeometryPickable();
}

void entity::FrameTool::updateGeometryPickable()
{
    m_geomPickable->dirtyDisplayList();
    m_geomPickable->dirtyBound();
}

void entity::FrameTool::setCenter()
{

}

void entity::FrameTool::setAxis()
{

}

void entity::FrameTool::setNormal()
{

}

void entity::FrameTool::setMovables2D()
{

}
