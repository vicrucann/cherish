#ifndef TOOLLOCAL_H
#define TOOLLOCAL_H

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"

#include "Settings.h"

/* ToolLocal is a base class for any construction tools on scene.
 * Have to re-define setVertices()
 * TODO: put visibility, state set params into base class so that the structure is
 * [osg::Group] -> [osg::Switch] -> (osg::Geode)
 * TODO2: auto-resize for certain tools like bookmarks, global axis, local axis
 */

namespace entity {
class ToolLocal : public osg::Group
{
public:
    ToolLocal(unsigned int nVerts,
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

class ToolNormal : public ToolLocal
{
public:
    ToolNormal();

    void setVertices(const osg::Vec3f &center, float szX, float szY, float, float);
};

class ToolAxisLocal : public ToolLocal
{
public:
    ToolAxisLocal();
    void setColor();
    void setVertices(const osg::Vec3f &center, float, float, float, float szAx);
};

class ToolFrame : public ToolLocal
{
public:
    ToolFrame();
    void setVertices(const osg::Vec3f &center, float szX, float szY,
                     float szCr, float);
    void setIntersection(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4);
    const osg::Vec3Array* getVertices() const;
    void setColor(const osg::Vec4f &colorMain, const osg::Vec4f& colorIntersect = dureu::CANVAS_CLR_PREVIOUS);
protected:
    osg::Geometry* m_geomPick;
    osg::Geometry* m_geomIntersect;
};

class ToolIntersectionLine : public ToolLocal
{
public:
    ToolIntersectionLine(const osg::Vec3f& P1, const osg::Vec3f& P2,
                         const osg::Vec3f& P3, const osg::Vec3f& P4);
    void setVertices(const osg::Vec3f &, float, float, float, float){}
    void setVertices(const osg::Vec3f& P1, const osg::Vec3f& P2,
                     const osg::Vec3f& P3, const osg::Vec3f& P4);
};

class ToolFrameSelected : public ToolLocal
{
public:
    ToolFrameSelected(const osg::Vec3f &center, float szX, float szY, float szCr, float);
    void setVertices(const osg::Vec3f &center, float szX, float szY, float, float);

protected:
    osg::Geometry* getQuadGeometry(float szCr, float X, float Y) const;

    osg::Geometry* m_geomCenter;
    osg::Geometry* m_geomCr1, * m_geomCr2, * m_geomCr3, * m_geomCr4;
    osg::Geometry* m_geomAx1, * m_geomAx2, * m_geomAx3, * m_geomAx4;
};

}


#endif // ToolLocal_H
