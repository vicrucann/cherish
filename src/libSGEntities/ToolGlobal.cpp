#include "ToolGlobal.h"
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/LineStipple>

#include <assert.h>

#include "Settings.h"


entity::ToolGlobal::ToolGlobal(int nVerts, osg::Array::Binding colorBind, osg::PrimitiveSet *primitiveSet, float linewidth)
    : osg::Group()
    , m_switch(new osg::Switch)
    , m_geodeWire(new osg::Geode)
    , m_geomWire(new osg::Geometry)
{
    osg::Vec3Array* verts = new osg::Vec3Array(nVerts);
    m_geomWire->setVertexArray(verts);

    osg::Vec4Array* clrs = new osg::Vec4Array(nVerts);
    m_geomWire->setColorArray(clrs, colorBind);

    osg::StateSet* ss = new osg::StateSet;
    osg::LineWidth* lw = new osg::LineWidth;
    lw->setWidth(linewidth);
    ss->setAttributeAndModes(lw,osg::StateAttribute::ON);
    ss->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
    ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    m_geomWire->setStateSet(ss);
    m_geomWire->addPrimitiveSet(primitiveSet);

    m_geodeWire->addDrawable(m_geomWire);

    this->addChild(m_switch);
}

void entity::ToolGlobal::setVertices(const std::vector<osg::Vec3f> &source)
{
    osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(m_geomWire->getVertexArray());
    assert(vertices->size() == source.size());
    for (size_t i=0; i<vertices->size(); ++i)
        (*vertices)[i] = source[i];
    this->updateGeometry();
}

const osg::Vec3Array *entity::ToolGlobal::getVertices() const
{
    return static_cast<osg::Vec3Array*>(m_geomWire->getVertexArray());
}

void entity::ToolGlobal::setColor(const osg::Vec4f &color)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geomWire->getColorArray());
    assert(colors->size()>0);
    (*colors)[0] = color;
    this->updateGeometry();
}

const osg::Vec4f &entity::ToolGlobal::getColor() const
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geomWire->getColorArray());
    assert(colors->size()>0);
    return (*colors)[0];
}

osg::Geode *entity::ToolGlobal::getGeodeWire() const
{
    return m_geodeWire;
}

osg::Vec3f entity::ToolGlobal::getCenterLocal() const
{
    float u=0, v=0;
    const osg::Vec3Array* verts = this->getVertices();
    u += (*verts)[0].x();
    u += (*verts)[1].x();
    u += (*verts)[2].x();
    u += (*verts)[3].x();
    u /= 4.f;

    v += (*verts)[0].y();
    v += (*verts)[1].y();
    v += (*verts)[2].y();
    v += (*verts)[3].y();
    v /= 4.f;

    return osg::Vec3f(u,v,0.f);
}

void entity::ToolGlobal::updateGeometry()
{
    m_geomWire->dirtyDisplayList();
    m_geomWire->dirtyBound();
}

entity::BookmarkTool::BookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
    : ToolGlobal(12, osg::Array::BIND_OVERALL,
           new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,12))
    , m_AT(new osg::AutoTransform)
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

    this->initializeSG();
    this->setVisibility(true);
}

void entity::BookmarkTool::initializeSG()
{
    m_switch->addChild(m_AT);
    m_AT->addChild(m_geodeWire);
}

void entity::BookmarkTool::setVisibility(bool on)
{
    m_switch->setChildValue(m_AT, on);
}

bool entity::BookmarkTool::getVisibility() const
{
    return m_switch->getChildValue(m_AT);
}

entity::AxisGlobalTool::AxisGlobalTool()
    : ToolGlobal(6, osg::Array::BIND_PER_VERTEX,
           new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,6))
    , m_AT(new osg::AutoTransform)
    , m_camera(new osg::Camera)
{
    this->setColor( dureu::AXES_CLR_X,  dureu::AXES_CLR_Y,  dureu::AXES_CLR_Z);

    osg::Vec3 corner = osg::Vec3(0.0f,0.0f,0.0f);
    osg::Vec3 xdir = osg::Vec3(dureu::AXES_SIZE,0.0f,0.0f);
    osg::Vec3 ydir = osg::Vec3(0.0f,dureu::AXES_SIZE,0.0f);
    osg::Vec3 zdir = osg::Vec3(0.0f,0.0f,dureu::AXES_SIZE);
    std::vector<osg::Vec3f> verts;
    verts.push_back(corner);
    verts.push_back(corner+xdir);
    verts.push_back(corner);
    verts.push_back(corner+ydir);
    verts.push_back(corner);
    verts.push_back(corner+zdir);
    this->setVertices(verts);

    this->initializeSG();
    this->setVisibility(true);
}

void entity::AxisGlobalTool::initializeSG()
{
    m_AT->setAutoScaleToScreen(true);
    m_AT->setPosition(osg::Vec3d(0,0,0));

    m_camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    m_camera->setRenderOrder(osg::Camera::POST_RENDER);

    m_switch->addChild(m_camera, true);
    m_camera->addChild(m_AT);
    m_AT->addChild(m_geodeWire);
}

void entity::AxisGlobalTool::setVisibility(bool on)
{
    m_switch->setChildValue(m_camera, on);
}

bool entity::AxisGlobalTool::getVisibility() const
{
    return m_switch->getChildValue(m_camera);
}

void entity::AxisGlobalTool::setColor(const osg::Vec4f c1, const osg::Vec4f c2, const osg::Vec4f c3)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(m_geomWire->getColorArray());
    (*colors)[0] = c1;
    (*colors)[1] = c1;
    (*colors)[2] = c2;
    (*colors)[3] = c2;
    (*colors)[4] = c3;
    (*colors)[5] = c3;
    this->updateGeometry();
}

entity::FrameTool::FrameTool()
    : ToolGlobal(4, osg::Array::BIND_OVERALL, new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0,4), 1)

    , m_geodePickable(new osg::Geode)
    , m_geodeIntersect(new osg::Geode)
    , m_geodeAxis(new osg::Geode)
    , m_geodeScales(new osg::Geode)

    , m_geomPickable(new osg::Geometry)
    , m_geomIntersect(new osg::Geometry)
    , m_geomCenter(new osg::Geometry)
    , m_geomAxisU(new osg::Geometry)
    , m_geomAxisV(new osg::Geometry)

    , m_geomScaleUV1(new osg::Geometry)
    , m_geomScaleUV2(new osg::Geometry)
    , m_geomScaleUV3(new osg::Geometry)
    , m_geomScaleUV4(new osg::Geometry)
//    , m_geomScaleU1(new osg::Geometry)
//    , m_geomScaleU2(new osg::Geometry)
//    , m_geomScaleV1(new osg::Geometry)
//    , m_geomScaleV2(new osg::Geometry)

    , m_cameraAxis(new osg::Camera)
    , m_selected(false)
{
    this->initializeSG();
    this->setColor(dureu::CANVAS_CLR_REST);
    this->setVisibility(true);
}

void entity::FrameTool::initializeSG()
{
    /* pickable settings */
    this->initQuadGeometry(m_geomPickable, "Pickable");
    this->initQuadGeometry(m_geomCenter, "Center");
    this->initQuadGeometry(m_geomAxisU, "AxisU");
    this->initQuadGeometry(m_geomAxisV, "AxisV");
//    this->initQuadGeometry(m_geomScaleU1, "ScaleU1");
//    this->initQuadGeometry(m_geomScaleU2, "ScaleU2");
//    this->initQuadGeometry(m_geomScaleV1, "ScaleV1");
//    this->initQuadGeometry(m_geomScaleV2, "ScaleV2");
    this->initQuadGeometry(m_geomScaleUV1, "ScaleUV1");
    this->initQuadGeometry(m_geomScaleUV2, "ScaleUV2");
    this->initQuadGeometry(m_geomScaleUV3, "ScaleUV3");
    this->initQuadGeometry(m_geomScaleUV4, "ScaleUV4");

    /* intersection settings */
    osg::LineStipple* ls = new osg::LineStipple;
    ls->setFactor(1);
    ls->setPattern(0xf00f);

    m_geomIntersect->setVertexArray(new osg::Vec3Array(4));
    m_geomIntersect->setColorArray(new osg::Vec4Array(4), osg::Array::BIND_OVERALL);
    m_geomIntersect->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0,4));
    m_geomIntersect->getOrCreateStateSet()->setAttributeAndModes(ls, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    /* scene graph structure */
    m_geodeIntersect->addDrawable(m_geomIntersect);
    m_geodePickable->addDrawable(m_geomPickable);
    m_geodeAxis->addDrawable(m_geomCenter);
    m_geodeAxis->addDrawable(m_geomAxisU);
    m_geodeAxis->addDrawable(m_geomAxisV);
//    m_geodeScales->addDrawable(m_geomScaleU1);
//    m_geodeScales->addDrawable(m_geomScaleU2);
//    m_geodeScales->addDrawable(m_geomScaleV1);
//    m_geodeScales->addDrawable(m_geomScaleV2);
    m_geodeScales->addDrawable(m_geomScaleUV1);
    m_geodeScales->addDrawable(m_geomScaleUV2);
    m_geodeScales->addDrawable(m_geomScaleUV3);
    m_geodeScales->addDrawable(m_geomScaleUV4);

    m_cameraAxis->setClearMask(GL_DEPTH_BUFFER_BIT);
    m_cameraAxis->setRenderOrder(osg::Camera::POST_RENDER);
    m_cameraAxis->addChild(m_geodeAxis);

    m_switch->addChild(m_geodeIntersect);
    m_switch->addChild(m_geodeWire);
    m_switch->addChild(m_geodePickable);
    m_switch->addChild(m_cameraAxis);
    m_switch->addChild(m_geodeScales);
}

// TODO: push to stack visibility values, and then pop them
// when visibility is turned on back
void entity::FrameTool::setVisibility(bool on)
{
    m_switch->setChildValue(m_geodeWire, on);
    m_switch->setChildValue(m_geodePickable, on);
    m_switch->setChildValue(m_geodeIntersect, on);
    m_switch->setChildValue(m_cameraAxis, false); // FIXME
    m_switch->setChildValue(m_geodeScales, false); // FIXME
}

bool entity::FrameTool::getVisibility() const
{
    return m_switch->getChildValue(m_geodeWire);
}

void entity::FrameTool::setVertices(const osg::Vec3f &center, float szX, float szY,
                                    float szCr, float szAx,
                                    const osg::Vec3f &centerCustom, double theta,
                                    bool selectionIsEmpty)
{
    /* wireframe drawables */
    std::vector<osg::Vec3f> verts;
    verts.push_back(center + osg::Vec3(szX,szY,0.f));
    verts.push_back(center + osg::Vec3(-szX,szY,0.f));
    verts.push_back(center + osg::Vec3(-szX,-szY,0.f));
    verts.push_back(center + osg::Vec3(szX,-szY,0.f));
    ToolGlobal::setVertices(verts);

    m_selected = !selectionIsEmpty;

    osg::Vec3f p0 = verts.at(0);
    /* if normal mode, set up pickable position */
    if (selectionIsEmpty){
        this->setQuadGeometry(m_geomPickable, p0, szCr, szCr);
        m_switch->setChildValue(m_geodePickable, true);
        m_switch->setChildValue(m_cameraAxis, false);
        m_switch->setChildValue(m_geodeScales, false);
    }
    /* if edit mode, set up selected drawables */
    else {
        m_switch->setChildValue(m_geodePickable, false);
        m_switch->setChildValue(m_cameraAxis, true);
        m_switch->setChildValue(m_geodeScales, true);

        osg::Vec3f Pc = centerCustom + osg::Vec3f(szCr*0.5, szCr*0.5, 0);
        this->setQuadGeometry(m_geomCenter, Pc, szCr, szCr);

        osg::Vec3f Pau = Pc + osg::Vec3f(szAx + 0.1, 0, 0);
        this->setQuadGeometry(m_geomAxisU, Pau, szAx, szCr, theta, centerCustom);

        osg::Vec3f Pav = Pc + osg::Vec3f(0, szAx + 0.1, 0);
        this->setQuadGeometry(m_geomAxisV, Pav, szCr, szAx, theta, centerCustom);

        float sz05 = szCr*0.5;
        osg::Vec3f p1 = verts.at(1) + osg::Vec3f(szCr,0,0);
        osg::Vec3f p2 = verts.at(2) + osg::Vec3f(szCr, szCr, 0);
        osg::Vec3f p3 = verts.at(3) + osg::Vec3f(0, szCr, 0);
        osg::Vec3f p01 = p1 + osg::Vec3f(szX-sz05,0,0);
        osg::Vec3f p23 = p2 + osg::Vec3f(szX-sz05,0,0);
        osg::Vec3f p12 = p2 + osg::Vec3f(0,szY-sz05,0);
        osg::Vec3f p30 = p3 + osg::Vec3f(0,szY-sz05,0);
        this->setQuadGeometry(m_geomScaleUV1, p0, szCr, szCr);
        this->setQuadGeometry(m_geomScaleUV2, p1, szCr, szCr);
        this->setQuadGeometry(m_geomScaleUV3, p2, szCr, szCr);
        this->setQuadGeometry(m_geomScaleUV4, p3, szCr, szCr);
//        this->setQuadGeometry(m_geomScaleU1, p01, szCr, szCr);
//        this->setQuadGeometry(m_geomScaleU2, p23, szCr, szCr);
//        this->setQuadGeometry(m_geomScaleV1, p12, szCr, szCr);
//        this->setQuadGeometry(m_geomScaleV2, p30, szCr, szCr);
    }
}

void entity::FrameTool::setColor(const osg::Vec4f &color, const osg::Vec4f &colorIntersect)
{
    ToolGlobal::setColor(color);
    this->setColorQuadGeometry(m_geomPickable, color);
    this->setColorQuadGeometry(m_geomCenter, solarized::base00);
    this->setColorQuadGeometry(m_geomAxisU, solarized::base00);
    this->setColorQuadGeometry(m_geomAxisV, solarized::base00);
//    this->setColorQuadGeometry(m_geomScaleU1, solarized::base00);
//    this->setColorQuadGeometry(m_geomScaleU2, solarized::base00);
//    this->setColorQuadGeometry(m_geomScaleV1, solarized::base00);
//    this->setColorQuadGeometry(m_geomScaleV2, solarized::base00);
    this->setColorQuadGeometry(m_geomScaleUV1, solarized::base00);
    this->setColorQuadGeometry(m_geomScaleUV2, solarized::base00);
    this->setColorQuadGeometry(m_geomScaleUV3, solarized::base00);
    this->setColorQuadGeometry(m_geomScaleUV4, solarized::base00);
    this->setColorIntersection(colorIntersect);
}

void entity::FrameTool::setIntersection(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4)
{
    if (P1.isNaN() || P2.isNaN() || P3.isNaN() || P4.isNaN()) return;
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(m_geomIntersect->getVertexArray());
    assert(verts->size() == 4);
    (*verts)[0] = P1;
    (*verts)[1] = P2;
    (*verts)[2] = P3;
    (*verts)[3] = P4;

    m_geomIntersect->dirtyDisplayList();
    m_geomIntersect->dirtyBound();
}

void entity::FrameTool::setColorIntersection(const osg::Vec4f &colorIntersect)
{
    osg::Vec4Array* colorInter = static_cast<osg::Vec4Array*>(m_geomIntersect->getColorArray());
    assert(colorInter->size() > 0);
    (*colorInter)[0] = colorIntersect;
    m_geomIntersect->dirtyDisplayList();
    m_geomIntersect->dirtyBound();
}

osg::Geometry *entity::FrameTool::getPickable() const
{
    return m_geomPickable;
}

bool entity::FrameTool::isSelected() const
{
    return m_selected;
}

void entity::FrameTool::moveDelta(double du, double dv)
{
    this->moveDeltaWireGeometry(m_geomWire, du, dv);
    this->moveDeltaWireGeometry(m_geomCenter, du, dv);
    this->moveDeltaWireGeometry(m_geomAxisU, du, dv);
    this->moveDeltaWireGeometry(m_geomAxisV, du, dv);
//    this->moveDeltaWireGeometry(m_geomNormal, du, dv);
//    this->moveDeltaWireGeometry(m_geomScaleU1, du, dv);
//    this->moveDeltaWireGeometry(m_geomScaleU2, du, dv);
//    this->moveDeltaWireGeometry(m_geomScaleV1, du, dv);
//    this->moveDeltaWireGeometry(m_geomScaleV2, du, dv);
    this->moveDeltaWireGeometry(m_geomScaleUV1, du, dv);
    this->moveDeltaWireGeometry(m_geomScaleUV2, du, dv);
    this->moveDeltaWireGeometry(m_geomScaleUV3, du, dv);
    this->moveDeltaWireGeometry(m_geomScaleUV4, du, dv);
}

void entity::FrameTool::scale(double scaleX, double scaleY, osg::Vec3f center)
{
    this->scaleWireGeometry(m_geomWire, scaleX, scaleY, center);
    this->scaleWireGeometry(m_geomCenter, scaleX, scaleY, center);
    this->scaleWireGeometry(m_geomAxisU, scaleX, scaleY, center);
    this->scaleWireGeometry(m_geomAxisV, scaleX, scaleY, center);
//    this->scaleWireGeometry(m_geomNormal, scaleX, scaleY, center);
//    this->scaleWireGeometry(m_geomScaleU1, scaleX, scaleY, center);
//    this->scaleWireGeometry(m_geomScaleU2, scaleX, scaleY, center);
//    this->scaleWireGeometry(m_geomScaleV1, scaleX, scaleY, center);
//    this->scaleWireGeometry(m_geomScaleV2, scaleX, scaleY, center);
    this->scaleWireGeometry(m_geomScaleUV1, scaleX, scaleY, center);
    this->scaleWireGeometry(m_geomScaleUV2, scaleX, scaleY, center);
    this->scaleWireGeometry(m_geomScaleUV3, scaleX, scaleY, center);
    this->scaleWireGeometry(m_geomScaleUV4, scaleX, scaleY, center);
}

void entity::FrameTool::scale(double scale, osg::Vec3f center)
{
    this->scaleWireGeometry(m_geomWire, scale, center);
    this->scaleWireGeometry(m_geomCenter, scale, center);
    this->scaleWireGeometry(m_geomAxisU, scale, center);
    this->scaleWireGeometry(m_geomAxisV, scale, center);
//    this->scaleWireGeometry(m_geomNormal, scale, center);
//    this->scaleWireGeometry(m_geomScaleU1, scale, center);
//    this->scaleWireGeometry(m_geomScaleU2, scale, center);
//    this->scaleWireGeometry(m_geomScaleV1, scale, center);
//    this->scaleWireGeometry(m_geomScaleV2, scale, center);
    this->scaleWireGeometry(m_geomScaleUV1, scale, center);
    this->scaleWireGeometry(m_geomScaleUV2, scale, center);
    this->scaleWireGeometry(m_geomScaleUV3, scale, center);
    this->scaleWireGeometry(m_geomScaleUV4, scale, center);
}

void entity::FrameTool::rotate(double theta, osg::Vec3f center)
{
    this->rotateWireGeometry(m_geomWire, theta, center);
    this->rotateWireGeometry(m_geomCenter, theta, center);
    this->rotateWireGeometry(m_geomAxisU, theta, center);
    this->rotateWireGeometry(m_geomAxisV, theta, center);
//    this->rotateWireGeometry(m_geomNormal, theta, center);
//    this->rotateWireGeometry(m_geomScaleU1, theta, center);
//    this->rotateWireGeometry(m_geomScaleU2, theta, center);
//    this->rotateWireGeometry(m_geomScaleV1, theta, center);
//    this->rotateWireGeometry(m_geomScaleV2, theta, center);
    this->rotateWireGeometry(m_geomScaleUV1, theta, center);
    this->rotateWireGeometry(m_geomScaleUV2, theta, center);
    this->rotateWireGeometry(m_geomScaleUV3, theta, center);
    this->rotateWireGeometry(m_geomScaleUV4, theta, center);
}

// TODO: put all quad and wire functions into abstract class
void entity::FrameTool::initQuadGeometry(osg::Geometry *geom, const std::string &name)
{
    geom->setVertexArray(new osg::Vec3Array(4));
    geom->setColorArray(new osg::Vec4Array(4), osg::Array::BIND_OVERALL);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
    geom->setName(name);
}

/*
 * sxY
 * ^
 * |
 * |
 *
 * v1--v0 (P)
 * |    |
 * |    |
 * v2--v3   ----> szX
 *
 *
 * v0 = P
 * theta is measured from center
 */
void entity::FrameTool::setQuadGeometry(osg::Geometry *geom, const osg::Vec3f &P, float szX, float szY, float theta, const osg::Vec3f &center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(geom->getVertexArray());
    assert(verts->size() == 4);
    (*verts)[0] = P;
    (*verts)[1] = P + osg::Vec3(-szX, 0.f, 0.f);
    (*verts)[2] = P + osg::Vec3(-szX, -szY, 0.f);
    (*verts)[3] = P + osg::Vec3(0.f, -szY, 0.f);

    if (theta != 0){
        for (size_t i=0; i<verts->size(); ++i){
            osg::Vec3f vi = (*verts)[i] - center;
            (*verts)[i] = center + osg::Vec3f(vi.x() * std::cos(theta) - vi.y() * std::sin(theta),
                                              vi.x() * std::sin(theta) + vi.y() * std::cos(theta), 0);
        }
    }
    this->updateGeometry(geom);
}



//void entity::FrameTool::setQuadGeometry(osg::Geometry *geom, const osg::Vec3f &P, float szX, float theta, float szY)
//{
//    szY = (!szY)? szX : szY;
//    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(geom->getVertexArray());
//    assert(verts->size() == 4);
//    (*verts)[0] = P;
//    (*verts)[1] = P + osg::Vec3(-szX * std::cos(theta), -szX * std::sin(theta), 0.f);
//    (*verts)[2] = P + osg::Vec3(-szX * std::cos(theta) + szY * std::sin(theta),
//                                -szX * std::sin(theta) -szY * std::cos(theta), 0.f);
//    (*verts)[3] = P + osg::Vec3(szY * std::sin(theta), -szY * std::cos(theta), 0.f);
//    this->updateGeometry(geom);
//}

void entity::FrameTool::setColorQuadGeometry(osg::Geometry *geom, const osg::Vec4f &color)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(geom->getColorArray());
    assert(colors->size() > 0);
    (*colors)[0] = color;
    this->updateGeometry(geom);
}

// TODO: put it into abstract class
void entity::FrameTool::updateGeometry(osg::Geometry *geom)
{
    geom->dirtyDisplayList();
    geom->dirtyBound();
}

void entity::FrameTool::moveDeltaWireGeometry(osg::Geometry * geometry, double du, double dv)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(geometry->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i];
        (*verts)[i] = osg::Vec3f(du+vi.x(), dv+vi.y(), 0);
    }
    verts->dirty();
    geometry->dirtyBound();
    geometry->dirtyDisplayList();
}

void entity::FrameTool::scaleWireGeometry(osg::Geometry *geometry, double scaleX, double scaleY, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(geometry->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(scaleX*vi.x(), scaleY*vi.y(), 0);
    }
    verts->dirty();
    geometry->dirtyBound();
    geometry->dirtyDisplayList();
}

void entity::FrameTool::scaleWireGeometry(osg::Geometry *geometry, double scale, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(geometry->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(scale*vi.x(), scale*vi.y(), 0);
    }
    verts->dirty();
    geometry->dirtyBound();
    geometry->dirtyDisplayList();
}

void entity::FrameTool::rotateWireGeometry(osg::Geometry *geometry, double theta, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(geometry->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(vi.x() * std::cos(theta) - vi.y() * std::sin(theta),
                                          vi.x() * std::sin(theta) + vi.y() * std::cos(theta), 0);
    }
    verts->dirty();
    geometry->dirtyBound();
    geometry->dirtyDisplayList();
}
