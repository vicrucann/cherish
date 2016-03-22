#include "Tool.h"
#include <osg/LineWidth>
#include <osg/BlendFunc>

#include <assert.h>

#include "Settings.h"


entity::Tool::Tool(int nVerts, osg::Array::Binding colorBind, osg::PrimitiveSet *primitiveSet, float linewidth)
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

void entity::Tool::setVertices(const std::vector<osg::Vec3f> &source)
{
    osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(m_geometry->getVertexArray());
    assert(vertices->size() == source.size());
    for (size_t i=0; i<vertices->size(); ++i)
        (*vertices)[i] = source[i];
    this->updateGeometry();
}

void entity::Tool::setColor(const osg::Vec4f &color)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geometry->getColorArray());
    assert(colors->size()>0);
    (*colors)[0] = color;
    this->updateGeometry();
}

const osg::Vec4f &entity::Tool::getColor() const
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geometry->getColorArray());
    assert(colors->size()>0);
    return (*colors)[0];
}

void entity::Tool::updateGeometry()
{
    m_geometry->dirtyDisplayList();
    m_geometry->dirtyBound();
}

entity::BookmarkTool::BookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
    : Tool(12, osg::Array::BIND_OVERALL,
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
    : Tool(6, osg::Array::BIND_PER_VERTEX,
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

entity::IntersectionTool::IntersectionTool(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4)
    : Tool(4, osg::Array::BIND_OVERALL,
           new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0,4))
{
    this->setColor(dureu::CANVAS_CLR_CURRENT);

    std::vector<osg::Vec3f> verts;
    verts.push_back(P1);
    verts.push_back(P2);
    verts.push_back(P3);
    verts.push_back(P4);
    this->setVertices(verts);

    this->initializeSG();
    this->setVisibility(false);
}

void entity::IntersectionTool::initializeSG()
{
    m_switch->addChild(m_geode);
}

void entity::IntersectionTool::setVisibility(bool on)
{
    m_switch->setChildValue(m_geode, on);
}

bool entity::IntersectionTool::getVisibility() const
{
    return m_switch->getChildValue(m_geode);
}

void entity::IntersectionTool::setPoints(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4)
{
    if (P1.isNaN() || P2.isNaN() || P3.isNaN() || P4.isNaN()) return;
    std::vector<osg::Vec3f> verts;
    verts.push_back(P1);
    verts.push_back(P2);
    verts.push_back(P3);
    verts.push_back(P4);
    this->setVertices(verts);
}

entity::AxisLocalTool::AxisLocalTool()
    : Tool(4, osg::Array::BIND_PER_VERTEX,
           new osg::DrawArrays(osg::PrimitiveSet::LINES,0,4), 1)
{
    this->setColor(solarized::orange, solarized::yellow);

    std::vector<osg::Vec3f> verts;
    osg::Vec3f center = osg::Vec3f(0,0,0);
    osg::Vec3f x = osg::Vec3f(dureu::CANVAS_AXIS,0,0);
    osg::Vec3f y = osg::Vec3f(0,dureu::CANVAS_AXIS,0);
    verts.push_back(center);
    verts.push_back(center+x);
    verts.push_back(center);
    verts.push_back(center+y);
    this->setVertices(verts);

    this->initializeSG();
    this->setVisibility(true);
}

void entity::AxisLocalTool::initializeSG()
{
    m_switch->addChild(m_geode);
}

void entity::AxisLocalTool::setVisibility(bool on)
{
    m_switch->setChildValue(m_geode, on);
}

bool entity::AxisLocalTool::getVisibility() const
{
    return m_switch->getChildValue(m_geode);
}

void entity::AxisLocalTool::setColor(const osg::Vec4f c1, const osg::Vec4f c2)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geometry->getColorArray());
    (*colors)[0] = c1;
    (*colors)[1] = c1;
    (*colors)[2] = c2;
    (*colors)[3] = c2;
    this->updateGeometry();
}
