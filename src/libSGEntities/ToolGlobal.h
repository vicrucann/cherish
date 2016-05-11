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

/*! \class ToolGlobal
 * Class description
*/
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

/*! \class BookmarkTool
 * Class description
*/
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

/*! \class AxisGlobalTool
 * Class description
*/
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

/*! \class FrameTool
 * Class description
*/
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
    void setEditable(bool editable);

    osg::Geometry* getPickable() const;
    bool isSelected() const;

    /* Entity2D API */
    virtual void moveDelta(double du, double dv);
    virtual void scale(double scaleX, double scaleY, osg::Vec3f center);
    virtual void scale(double scale, osg::Vec3f center);
    virtual void rotate(double theta, osg::Vec3f center); /* 2d rotation: within canvas */

protected:
    void initQuadGeometry(osg::Geometry* geom, const std::string& name = "");
    void initLineGeometry(osg::Geometry* geom, float lineWidth, const std::string& name = "");

    void setQuadGeometry(osg::Geometry* geom, const osg::Vec3f& P, float szX, float szY, float theta = 0, const osg::Vec3f& center = dureu::CENTER);
    void setLineGeometry(osg::Geometry* geom, const osg::Vec3f& P1, const osg::Vec3f& P2);

    void setColorGeometry(osg::Geometry* geom, const osg::Vec4f& color);
    void updateGeometry(osg::Geometry* geom);

    void moveDeltaWireGeometry(osg::Geometry*geometry, double du, double dv);
    void scaleWireGeometry(osg::Geometry* geometry, double scaleX, double scaleY, osg::Vec3f center);
    void scaleWireGeometry(osg::Geometry* geometry, double scale, osg::Vec3f center);
    void rotateWireGeometry(osg::Geometry* geometry, double theta, osg::Vec3f center);

private:
    osg::Geode* m_geodePickable, * m_geodeIntersect
                , * m_geodeAxis, * m_geodeScales, * m_geodeNormal, * m_geodeRotation;
    osg::Geometry* m_geomPickable, * m_geomIntersect, * m_geomCenter
                , * m_geomAxisU, * m_geomAxisV;

    /* tools to scale entities: uniform scaling and axis aligned */
    osg::Geometry * m_geomScaleUV1, * m_geomScaleUV2, * m_geomScaleUV3, * m_geomScaleUV4;

    /* canvas offset and 3d rotation drawables */
    osg::Geometry * m_geomNormal1, * m_geomNormal2;
    osg::Geometry * m_geomRotateX1, * m_geomRotateX2, * m_geomRotateY1, * m_geomRotateY2;

    osg::Camera* m_cameraAxis; /* so that local axis always renders on top of photos */

    bool m_selected; // if there is selection present on scene
    bool m_editable; // if canvas is in edit mode: offset and rotation
    bool m_visible;

    std::vector<bool> m_visibilityState;
};

}


#endif // TOOLGlobal_H
