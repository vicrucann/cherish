#include <assert.h>
#include <iostream>

#include "Canvas.h"
#include "settings.h"
#include "Stroke.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/BoundingBox>
#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/Plane>
#include <osg/BlendFunc>
#include <osg/BoundingSphere>

entity::Canvas::Canvas()
    : osg::Group()
{

}

entity::Canvas::Canvas(const entity::Canvas& cnv, const osg::CopyOp& copyop)
    : osg::Group(cnv, copyop)
{

}

entity::Canvas::Canvas(osg::MatrixTransform *transform, const std::string &name)
    : osg::Group()
    , m_switch(new osg::Switch)
    , _transform(transform)
    , m_switchFrame(new osg::Switch)
    , m_geodeAxis(new osg::Geode)
    , m_frame(new osg::Geometry)
    , m_pickable(new osg::Geometry)
    , m_axis(new osg::Geometry)
    , m_geodeData(new osg::Geode)

    , m_switchNormal(new osg::Switch)

    , _mVerticesFrame(new osg::Vec3Array(4))
    , _mVerticesPickable(new osg::Vec3Array(4))
    , _mVerticesAxis(new osg::Vec3Array(4))
    , _mVerticesNormal(new osg::Vec3Array(2))

    , _strokeCurrent(0)
    , _photoCurrent(0)

    , m_center(osg::Vec3f(0.f,0.f,0.f)) // moves only when strokes are introduced so that to define it as centroid
    , m_normal(dureu::NORMAL)
    , m_color(dureu::CANVAS_CLR_REST) // frame and pickable color

{
    this->transformData(_transform->getMatrix());
    this->setName(name);
    this->setColor(m_color);

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(1.5);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    //blendfunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ANTIALIAS);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    this->setStateSet(stateset);

    osg::MatrixTransform* tdata = new osg::MatrixTransform; // matrix transform for user data on 2D plane
    this->addChild(m_switch.get());
    m_switch->addChild(_transform.get(), true);
    _transform->addChild(m_switchFrame.get());
    _transform->addChild(tdata);
    _transform->addChild(m_switchNormal.get());

    tdata->addChild(m_geodeData.get());
    // _geodeData is  empty, it is for user input: strokes

    osg::Geode* geodeFrame = new osg::Geode;
    m_switchFrame->addChild(geodeFrame, true); // child #0
    m_switchFrame->addChild(m_geodeAxis.get(), true); // child #1

    osg::Geode* geodeNormal = new osg::Geode;
    m_switchNormal->addChild(geodeNormal, false);

    osg::Geometry* geomNormal = new osg::Geometry;

    geodeFrame->addDrawable(m_frame.get());
    geodeFrame->addDrawable(m_pickable.get());
    geodeNormal->addDrawable(geomNormal);
    m_geodeAxis->addDrawable(m_axis.get());

    this->setVertices(m_center, dureu::CANVAS_MINW, dureu::CANVAS_MINH, dureu::CANVAS_CORNER, dureu::CANVAS_AXIS);

    osg::Vec4Array* colorAxis = new osg::Vec4Array(4);
    (*colorAxis)[0] = solarized::base2;
    (*colorAxis)[1] = solarized::base2;
    (*colorAxis)[2] = solarized::base2;
    (*colorAxis)[3] = solarized::base2;

    osg::Vec4Array* colorNormal = new osg::Vec4Array(2);
    (*colorNormal)[0] = dureu::CANVAS_CLR_EDIT;
    (*colorNormal)[1] = dureu::CANVAS_CLR_EDIT;


    m_frame->setVertexArray(_mVerticesFrame);
    m_frame->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

    geomNormal->setVertexArray(_mVerticesNormal);
    geomNormal->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
    geomNormal->setColorArray(colorNormal, osg::Array::BIND_PER_VERTEX);

    m_pickable->setVertexArray(_mVerticesPickable);
    m_pickable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    m_axis->setVertexArray(_mVerticesAxis);
    m_axis->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,4));
    m_axis->setColorArray(colorAxis, osg::Array::BIND_PER_VERTEX);
}

void entity::Canvas::setSwitch(osg::Switch* sw)
{
    m_switch = sw;
}

const osg::Switch* entity::Canvas::getSwitch() const
{
    return m_switch.get();
}

void entity::Canvas::setSwitchFrame(osg::Switch* sw)
{
    m_switchFrame = sw;
}

const osg::Switch* entity::Canvas::getSwitchFrame() const
{
    return m_switchFrame.get();
}

void entity::Canvas::setGeodeAxis(osg::Geode* geode)
{
    m_geodeAxis = geode;
}

const osg::Geode* entity::Canvas::getGeodeAxis() const
{
    return m_geodeAxis.get();
}

void entity::Canvas::setFrame(osg::Geometry* geom)
{
    m_frame = geom;
}

const osg::Geometry* entity::Canvas::getFrame() const
{
    return m_frame.get();
}

void entity::Canvas::setPickable(osg::Geometry* geom)
{
    m_pickable = geom;
}

const osg::Geometry* entity::Canvas::getPickable() const
{
    return m_pickable.get();
}

void entity::Canvas::setAxis(osg::Geometry* geom)
{
    m_axis = geom;
}

const osg::Geometry* entity::Canvas::getAxis() const
{
    return m_axis.get();
}

void entity::Canvas::setSwitchNormal(osg::Switch* sw)
{
    m_switchNormal = sw;
}

const osg::Switch* entity::Canvas::getSwitchNormal() const
{
    return m_switchNormal.get();
}

void entity::Canvas::setGeodeData(osg::Geode* geode)
{
    m_geodeData = geode;
}

const osg::Geode* entity::Canvas::getGeodeData() const
{
    return m_geodeData.get();
}

osg::Geode*entity::Canvas::getGeodeData()
{
    return m_geodeData.get();
}

void entity::Canvas::setCenter(const osg::Vec3f &center)
{
    m_center = center;
}

const osg::Vec3f& entity::Canvas::getCenter() const
{
    return m_center;
}

void entity::Canvas::setNormal(const osg::Vec3f &normal)
{
    m_normal = normal;
}

const osg::Vec3f& entity::Canvas::getNormal() const
{
    return m_normal;
}

void entity::Canvas::setColor(const osg::Vec4f &color)
{
    if (color == dureu::CANVAS_CLR_CURRENT) // hide the axis for "rest" canvases
        m_switchFrame->setChildValue(m_geodeAxis, true);
    else
        m_switchFrame->setChildValue(m_geodeAxis, false);

    m_color = color;
    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = m_color;
    (*colors)[1] = m_color;
    (*colors)[2] = m_color;
    (*colors)[3] = m_color;
    m_frame->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    m_pickable->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}

const osg::Vec4f& entity::Canvas::getColor() const
{
    return m_color;
}

void entity::Canvas::setVisibility(bool vis)
{
    m_switch->setChildValue(m_switch->getChild(0), vis);
}

bool entity::Canvas::getVisibility() const{
    return m_switch->getChildValue(m_switch->getChild(0));
}

void entity::Canvas::setVisibilityLocalAxis(bool vis)
{
    m_switchFrame->setChildValue(m_geodeAxis, vis);
}

bool entity::Canvas::getVisibilityLocalAxis() const
{
    return m_switchFrame->getChildValue(m_geodeAxis);
}

void entity::Canvas::setTransformPost(osg::MatrixTransform *t){
    osg::Matrix matrix = t->getMatrix();
    _transform->postMult(matrix);
    this->transformData(matrix);
}

void entity::Canvas::setTransformPost(const osg::Matrix &m)
{
    _transform->postMult(m);
    this->transformData(m);
}

void entity::Canvas::setTransformPre(osg::MatrixTransform *r)
{
    osg::Matrix matrix = r->getMatrix();
    _transform->preMult(matrix);
    this->transformData(matrix);
}

osg::MatrixTransform* entity::Canvas::getTransform() const
{
    return _transform.get();
}

void entity::Canvas::movePhoto(entity::Photo *photo, const double u, const double v, int mouse)
{
    if (!photo){
        outErrMsg("movePhoto(): photo pointer is NULL");
        return;
    }
    if (mouse == 0){
        photo->setModeEdit(true);
    }

    photo->move(u,v);
    this->updateFrame();

    if (mouse == 2){
        photo->setModeEdit(false);
        //this->updateData();
    }
}

void entity::Canvas::setPhotoCurrent(entity::Photo *photo)
{
    if (_photoCurrent.get() == photo)
        return;
    if (_photoCurrent.get()!=0)
        this->setPhotoCurrent(false);
    _photoCurrent = photo;
    this->setPhotoCurrent(true);
}

// changes photo frame color based on bool varialbe
void entity::Canvas::setPhotoCurrent(bool current)
{
    if (!current)
        _photoCurrent->setFrameColor(dureu::PHOTO_CLR_REST);
    else
        _photoCurrent->setFrameColor(dureu::PHOTO_CLR_SELECTED);
}

void entity::Canvas::updateFrame()
{
    osg::BoundingBox bb = m_geodeData->getBoundingBox();
    assert(bb.valid());
    float dx = 0.5*(bb.xMax()-bb.xMin());
    float dy = 0.5*(bb.yMax()-bb.yMin());
    float szX = std::max(dx, dureu::CANVAS_MINW);
    float szY = std::max(dy, dureu::CANVAS_MINW);

    this->setVertices(bb.center(), szX, szY, dureu::CANVAS_CORNER, dureu::CANVAS_AXIS);
}

// we have to separate updateData() from updateFrame because:
// we do not want to update data during certain operations, such as
// moving photo within the canvas, or adding a stroke to a canvas
// updateData is called on release button for the mentioned cases
void entity::Canvas::updateData()
{
    osg::BoundingBox bb = m_geodeData->getBoundingBox();
    assert(bb.valid());

    outLogVec("_center", m_center.x(), m_center.y(), m_center.z());
    outLogVec("bb.center", bb.center().x(), bb.center().y(), bb.center().z());
    osg::Matrix mat;
    mat.makeTranslate(bb.center() - m_center);
    this->transformData(mat);
}

void entity::Canvas::setModeOffset(bool on)
{
    if (on){
        std::cout << "setModeOffset(): ON - " << on << std::endl;
        this->setColor(dureu::CANVAS_CLR_EDIT);
        this->setVisibilityLocalAxis(false);
    }
    else{
        std::cout << "setModeOffset(): OFF - " << on << std::endl;
        this->setColor(dureu::CANVAS_CLR_CURRENT);
        this->setVisibilityLocalAxis(true); // could be bug here, when originally local axis is off by user
    }
    m_switchNormal->setChildValue(m_switchNormal->getChild(0), on);
}

osg::Plane entity::Canvas::getPlane() const
{
    osg::Plane plane(m_normal, m_center);
    return plane;
}

entity::Stroke* entity::Canvas::getStrokeCurrent() const
{
    return _strokeCurrent.get();
}

void entity::Canvas::finishStrokeCurrent()
{
    assert(_strokeCurrent.get());
    _strokeCurrent = 0;
    std::cout << "finishStrokeCurrent(): finished stroke, observer pointer cleared" << std::endl;
    this->updateFrame();
    //this->updateData();
}

entity::Photo* entity::Canvas::getPhotoCurrent() const
{
    return _photoCurrent.get();
}

entity::Canvas::~Canvas()
{
}

// to transform plane, centroid and local axis
// must be called every time when transform node is changed
void entity::Canvas::transformData(const osg::Matrix &matrix)
{
    osg::Plane plane(m_normal, m_center);
    m_center = m_center * matrix;
    plane.transform(matrix);
    m_normal = plane.getNormal();
    outLogVec("transformData(): Canvas center", m_center.x(),m_center.y(),m_center.z());
    outLogVec("transformData(): Canvas normal", m_normal.x(), m_normal.y(), m_normal.z());
    assert(plane.valid());
}

void entity::Canvas::setVertices(const osg::Vec3f &center, float szX, float szY, float szCr, float szAx)
{
    assert(szX>=dureu::CANVAS_MINW && szY>=dureu::CANVAS_MINH);

    (*_mVerticesFrame)[0] = center + osg::Vec3(szX,szY,0.f);
    (*_mVerticesFrame)[1] = center + osg::Vec3(-szX,szY,0.f);
    (*_mVerticesFrame)[2] = center + osg::Vec3(-szX,-szY,0.f);
    (*_mVerticesFrame)[3] = center + osg::Vec3(szX,-szY,0.f);
    m_frame->dirtyDisplayList();
    m_frame->dirtyBound();

    assert(szCr >= dureu::CANVAS_CORNER);
    osg::Vec3 p0 = (*_mVerticesFrame)[0];
    (*_mVerticesPickable)[0] = p0;
    (*_mVerticesPickable)[1] = p0 + osg::Vec3(-szCr, 0.f, 0.f);
    (*_mVerticesPickable)[2] = p0 + osg::Vec3(-szCr, -szCr, 0.f);
    (*_mVerticesPickable)[3] = p0 + osg::Vec3(.0f, -szCr, 0.f);
    m_pickable->dirtyDisplayList();
    m_pickable->dirtyBound();

    assert(szAx>=dureu::CANVAS_AXIS);
    (*_mVerticesAxis)[0] = center;
    (*_mVerticesAxis)[1] = center + osg::Vec3(szAx,0.f,0.f);
    (*_mVerticesAxis)[2] = center;
    (*_mVerticesAxis)[3] = center + osg::Vec3(0.f, szAx, 0.f);
    m_axis->dirtyDisplayList();
    m_axis->dirtyBound();

    float szN = std::max(szX,szY);
    (*_mVerticesNormal)[0] = center;
    (*_mVerticesNormal)[1] = center + osg::Vec3f(0.f,0.f, szN);
}


