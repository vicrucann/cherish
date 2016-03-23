#ifndef TOOLGLOBAL_H
#define TOOLGLOBAL_H

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
class ToolGlobal : public osg::Group
{
public:
    ToolGlobal(int nVerts, osg::Array::Binding colorBind,
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

class BookmarkTool : public ToolGlobal
{
public:
    BookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);
    void initializeSG();
    void setVisibility(bool on);
    bool getVisibility() const;

private:
    osg::AutoTransform* m_AT;
};

class AxisGlobalTool : public ToolGlobal
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

}


#endif // TOOLGlobal_H
