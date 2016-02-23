#ifndef CONSTRUCTIONTOOL_H
#define CONSTRUCTIONTOOL_H

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"

namespace entity {
class ConstructionTool : public osg::Group
{
public:
    ConstructionTool(unsigned int nVerts,
                     osg::Array::Binding colorBind,
                     osg::PrimitiveSet* primitiveSet);

    virtual void setVertices(const osg::Vec3f &center, float szX, float szY,
                             float szCr, float szAx) = 0;

    virtual void setColor(const osg::Vec4f& color);

protected:
    osg::Geode* m_geodeTool;
    osg::Geometry* m_geomTool;
};

class ToolNormal : public ConstructionTool
{
public:
    ToolNormal();

    void setVertices(const osg::Vec3f &center, float szX, float szY, float, float);
};

class ToolAxisLocal : public ConstructionTool
{
public:
    ToolAxisLocal();
    void setColor();
    void setVertices(const osg::Vec3f &center, float, float, float, float szAx);
};

class ToolFrame : public ConstructionTool
{
public:
    ToolFrame();
    void setVertices(const osg::Vec3f &center, float szX, float szY,
                     float szCr, float szAx);
    void setColor(const osg::Vec4f &color);
protected:
    osg::Geometry* m_geomPick;
};

}


#endif // CONSTRUCTIONTOOL_H
