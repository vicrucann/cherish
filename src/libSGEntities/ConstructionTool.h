#ifndef CONSTRUCTIONTOOL_H
#define CONSTRUCTIONTOOL_H

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"

/* ConstructionTool is a base class for any construction tools on scene.
 * Have to re-define setVertices()
 * TODO: put visibility, state set params into base class so that the structure is
 * [osg::Group] -> [osg::Switch] -> (osg::Geode)
 * TODO2: auto-resize for certain tools like bookmarks, global axis, local axis
 */

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
    virtual const osg::Vec4f& getColor() const;

protected:
    osg::Geode* m_geodeTool;
    osg::Geometry* m_geomTool;
};

class ToolBookmark : public ConstructionTool
{
public:
    ToolBookmark(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);

    void setVertices(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);
    void setVertices(const osg::Vec3f&, float, float, float, float );
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
                     float szCr, float);
    const osg::Vec3Array* getVertices() const;
    void setColor(const osg::Vec4f &color);
protected:
    osg::Geometry* m_geomPick;
};

class ToolIntersectionLine : public ConstructionTool
{
public:
    ToolIntersectionLine(const osg::Vec3f& P1, const osg::Vec3f& P2,
                         const osg::Vec3f& P3, const osg::Vec3f& P4);
    void setVertices(const osg::Vec3f &, float, float, float, float){}
    void setVertices(const osg::Vec3f& P1, const osg::Vec3f& P2,
                     const osg::Vec3f& P3, const osg::Vec3f& P4);
};

}


#endif // CONSTRUCTIONTOOL_H
