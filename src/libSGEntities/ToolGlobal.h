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

#include "Settings.h"

namespace entity {
class ToolGlobal : public osg::Group
{
public:
    ToolGlobal(int nVerts, osg::Array::Binding colorBind,
         osg::PrimitiveSet* primitiveSet, float linewidth=3.f);

    /* connect switch to geode: either directly or use other nodes in between */
    virtual void initializeSG() = 0;

    virtual void setVertices(const std::vector<osg::Vec3f>& source);
    virtual const osg::Vec3Array* getVertices() const;
    virtual void setColor(const osg::Vec4f& color);
    virtual const osg::Vec4f& getColor() const;

    virtual void setVisibility(bool on) = 0;
    virtual bool getVisibility() const = 0;

protected:
    void updateGeometry();

    osg::Switch* m_switch;
    osg::Geode* m_geodeWire;
    osg::Geometry* m_geomWire;
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

class FrameTool : public ToolGlobal
{
public:
    FrameTool();
    void initializeSG();
    void setVisibility(bool on);
    bool getVisibility() const;
    /* the bounding box frame, also updates all other geometries */
    void setVertices(const osg::Vec3f &center, float szX, float szY, float szCr, float szAx,
                     const osg::Vec3f& centerCustom = dureu::CENTER ,bool selectionIsEmpty = true);
    void setColor(const osg::Vec4f& color, const osg::Vec4f& colorIntersect = dureu::CANVAS_CLR_PREVIOUS);

    void setIntersection(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4);
    void setColorIntersection(const osg::Vec4f& colorIntersect);

    osg::Geometry* getPickable() const;
    osg::Geometry* getCenterGeometry() const;
    osg::Geometry* getAxisUGeometry() const;
    osg::Geometry* getAxisVGeometry() const;


protected:
    void initQuadGeometry(osg::Geometry* geom);
    void setQuadGeometry(osg::Geometry* geom, const osg::Vec3f& P, float szX, float szY = 0);
    void setColorQuadGeometry(osg::Geometry* geom, const osg::Vec4f& color);
    void updateGeometry(osg::Geometry* geom);

private:
    osg::Geode* m_geodePickable, * m_geodeIntersect
                , * m_geodeAxis, * m_geodeNormal, * m_geodeScales;
    osg::Geometry* m_geomPickable, * m_geomIntersect, * m_geomCenter
                , * m_geomAxisU, * m_geomAxisV, * m_geomNormal;

    /* tools to scale entities: uniform scaling and axis aligned */
    osg::Geometry * m_geomScaleUV1, * m_geomScaleUV2, * m_geomScaleUV3, * m_geomScaleUV4;
    osg::Geometry * m_geomScaleU1, * m_geomScaleU2;
    osg::Geometry * m_geomScaleV1, * m_geomScaleV2;

    osg::Camera* m_cameraAxis;
};

}


#endif // TOOLGlobal_H
