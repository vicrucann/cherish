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

    osg::Geode* getGeodeWire() const;
    osg::Vec3f getCenterLocal() const;

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
                     const osg::Vec3f& centerCustom = dureu::CENTER ,
                     double theta = 0,
                     bool selectionIsEmpty = true);

    void setColor(const osg::Vec4f& color, const osg::Vec4f& colorIntersect = dureu::CANVAS_CLR_PREVIOUS);

    void setIntersection(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4);
    void setColorIntersection(const osg::Vec4f& colorIntersect);

    osg::Geometry* getPickable() const;
    bool isSelected() const;

    /* Entity2D API */
    virtual void moveDelta(double du, double dv);
    virtual void scale(double scaleX, double scaleY, osg::Vec3f center);
    virtual void scale(double scale, osg::Vec3f center);
    virtual void rotate(double theta, osg::Vec3f center);

protected:
    void initQuadGeometry(osg::Geometry* geom, const std::string& name = "");
//    void setQuadGeometry(osg::Geometry* geom, const osg::Vec3f& P, float szX,
//                         float theta = 0, float szY = 0);

    void setQuadGeometry(osg::Geometry* geom, const osg::Vec3f& P, float szX, float szY, float theta = 0, const osg::Vec3f& center = dureu::CENTER);

    void setColorQuadGeometry(osg::Geometry* geom, const osg::Vec4f& color);
    void updateGeometry(osg::Geometry* geom);

    void moveDeltaWireGeometry(osg::Geometry*geometry, double du, double dv);
    void scaleWireGeometry(osg::Geometry* geometry, double scaleX, double scaleY, osg::Vec3f center);
    void scaleWireGeometry(osg::Geometry* geometry, double scale, osg::Vec3f center);
    void rotateWireGeometry(osg::Geometry* geometry, double theta, osg::Vec3f center);

private:
    osg::Geode* m_geodePickable, * m_geodeIntersect
                , * m_geodeAxis, * m_geodeNormal, * m_geodeScales;
    osg::Geometry* m_geomPickable, * m_geomIntersect, * m_geomCenter
                , * m_geomAxisU, * m_geomAxisV, * m_geomNormal;

    /* tools to scale entities: uniform scaling and axis aligned */
    osg::Geometry * m_geomScaleUV1, * m_geomScaleUV2, * m_geomScaleUV3, * m_geomScaleUV4;
//    osg::Geometry * m_geomScaleU1, * m_geomScaleU2;
//    osg::Geometry * m_geomScaleV1, * m_geomScaleV2;

    osg::Camera* m_cameraAxis;

    bool m_selected;
};

}


#endif // TOOLGlobal_H
