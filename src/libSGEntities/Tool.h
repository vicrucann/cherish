#ifndef TOOL_H
#define TOOL_H

#include <vector>
#include <iostream>
#include <stdio.h>

#include <osg/Group>
#include <osg/Switch>
#include <osg/AutoTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Camera>

namespace entity {
class Tool : public osg::Group
{
public:
    Tool(int nVerts, osg::Array::Binding colorBind,
         osg::PrimitiveSet* primitiveSet, float linewidth=3.f);

    /* connect switch to geode: either directly or use other nodes in between */
    virtual void initializeSG() = 0;

    virtual void setVertices(const std::vector<osg::Vec3f>& source);
    virtual void setColor(const osg::Vec4f& color);
    virtual const osg::Vec4f& getColor() const;

    virtual void setVisibility(bool on) = 0;
    virtual bool getVisibility() const = 0;

protected:
    void updateGeometry();

    osg::Switch* m_switch;
    osg::Geode* m_geode;
    osg::Geometry* m_geometry;
};

class BookmarkTool : public Tool
{
public:
    BookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);
    void initializeSG();
    void setVisibility(bool on);
    bool getVisibility() const;

private:
    osg::AutoTransform* m_AT;
};

class AxisGlobalTool : public Tool
{
public:
    AxisGlobalTool();
    void initializeSG();
    void setVisibility(bool on);
    bool getVisibility() const;

    virtual void setColor(const osg::Vec4f c1, const osg::Vec4f c2, const osg::Vec4f c3);

private:
    osg::AutoTransform* m_AT;
    osg::Camera* m_camera;
};

class IntersectionTool : public Tool
{
public:
    IntersectionTool(const osg::Vec3f& P1, const osg::Vec3f& P2,
                     const osg::Vec3f& P3, const osg::Vec3f& P4);
    void initializeSG();
    void setVisibility(bool on);
    bool getVisibility() const;
    void setPoints(const osg::Vec3f& P1, const osg::Vec3f& P2,
                   const osg::Vec3f& P3, const osg::Vec3f& P4);
};

class AxisLocalTool : public Tool
{
public:
    AxisLocalTool();
    void initializeSG();
    void setVisibility(bool on);
    bool getVisibility() const;

    virtual void setColor(const osg::Vec4f c1, const osg::Vec4f c2);
};

}


#endif // TOOL_H
