#include "Tool.h"
#include <osg/LineWidth>
#include <osg/BlendFunc>

#include <assert.h>

#include "Settings.h"


entity::Tool::Tool(int nVerts, osg::Array::Binding colorBind, osg::PrimitiveSet *primitiveSet, float linewidth)
    : osg::Group()
    , m_switch(new osg::Switch)
    , m_AT(new osg::AutoTransform)
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
    m_switch->addChild(m_AT);
    m_AT->addChild(m_geode);

    this->setVisibility(true);
}

void entity::Tool::setVertices(const std::vector<osg::Vec3f> &source)
{
    osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(m_geometry->getVertexArray());
    assert(vertices->size() == source.size());
    for (size_t i=0; i<vertices->size(); ++i)
        (*vertices)[i] = source[i];
    m_geometry->dirtyDisplayList();
    m_geometry->dirtyBound();
}

void entity::Tool::setColor(const osg::Vec4f &color)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geometry->getColorArray());
    assert(colors->size()>0);
    (*colors)[0] = color;
    m_geometry->dirtyDisplayList();
    m_geometry->dirtyBound();
}

const osg::Vec4f &entity::Tool::getColor() const
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geometry->getColorArray());
    assert(colors->size()>0);
    return (*colors)[0];
}

void entity::Tool::setVisibility(bool on)
{
    m_switch->setChildValue(m_AT, on);
}

bool entity::Tool::getVisibility() const
{
    return m_switch->getChildValue(m_AT);
}

entity::BookmarkTool::BookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
    : Tool(12, osg::Array::BIND_OVERALL,
           new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,12))
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
}
