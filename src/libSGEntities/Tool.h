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

namespace entity {
class Tool : public osg::Group
{
public:
    Tool(int nVerts, osg::Array::Binding colorBind,
         osg::PrimitiveSet* primitiveSet, float linewidth=3.f);

    virtual void setVertices(const std::vector<osg::Vec3f>& source);
    virtual void setColor(const osg::Vec4f& color);
    virtual const osg::Vec4f& getColor() const;

    void setVisibility(bool on);
    bool getVisibility() const;

protected:
    osg::Switch* m_switch;
    osg::AutoTransform* m_AT;
    osg::Geode* m_geode;
    osg::Geometry* m_geometry;
};

class BookmarkTool : public Tool
{
public:
    BookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);
};

}


#endif // TOOL_H
