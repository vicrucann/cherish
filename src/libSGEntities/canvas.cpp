#include <assert.h>
#include <iostream>

#include "canvas.h"
#include "settings.h"
#include "stroke.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/BoundingBox>
#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/Plane>
#include <osg/BlendFunc>
#include <osg/BoundingSphere>

Canvas::Canvas(osg::MatrixTransform *transform, const std::string &name):
    _switch(new osg::Switch),
    _transform(transform),
    _switchFrame(new osg::Switch),
    _transformData(new osg::MatrixTransform),
    _geodeFrame(new osg::Geode),
    _geodeAxis(new osg::Geode),
    _frame(new osg::Geometry),
    _pickable(new osg::Geometry),
    _axis(new osg::Geometry),
    _geodeData(new osg::Geode),

    _mSwitchNormal(new osg::Switch),
    _mGeodeNormal(new osg::Geode),
    _mGeometryNormal(new osg::Geometry),

    _mVerticesFrame(new osg::Vec3Array(4)),
    _mVerticesPickable(new osg::Vec3Array(4)),
    _mVerticesAxis(new osg::Vec3Array(4)),
    _mVerticesNormal(new osg::Vec3Array(2)),

    _strokeCurrent(0),
    _photoCurrent(0),

    _center(osg::Vec3f(0.f,0.f,0.f)), // moves only when strokes are introduced so that to define it as centroid
    _normal(dureu::NORMAL),
    _color(dureu::CANVAS_CLR_REST) // frame and pickable color
  , _idPhoto(0)

{
    this->transformData(_transform->getMatrix());
    this->setName(name);
    this->setColor(_color);

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

    this->addChild(_switch.get());
    _switch->addChild(_transform.get(), true);
    _transform->addChild(_switchFrame.get());
    _transform->addChild(_transformData.get());
    _transform->addChild(_mSwitchNormal.get());

    _transformData->addChild(_geodeData.get());
    // _geodeData is  empty, it is for user input: strokes

    _switchFrame->addChild(_geodeFrame.get(), true); // child #0
    _switchFrame->addChild(_geodeAxis.get(), true); // child #1

    _mSwitchNormal->addChild(_mGeodeNormal.get(), false);

    _geodeFrame->addDrawable(_frame.get());
    _geodeFrame->addDrawable(_pickable.get());
    _mGeodeNormal->addDrawable(_mGeometryNormal.get());
    _geodeAxis->addDrawable(_axis.get());

    this->setVertices(_center, dureu::CANVAS_MINW, dureu::CANVAS_MINH, dureu::CANVAS_CORNER, dureu::CANVAS_AXIS);

     osg::Vec4Array* colorAxis = new osg::Vec4Array(4);
     (*colorAxis)[0] = solarized::base2;
     (*colorAxis)[1] = solarized::base2;
     (*colorAxis)[2] = solarized::base2;
     (*colorAxis)[3] = solarized::base2;

     osg::Vec4Array* colorNormal = new osg::Vec4Array(2);
     (*colorNormal)[0] = dureu::CANVAS_CLR_EDIT;
     (*colorNormal)[1] = dureu::CANVAS_CLR_EDIT;

    _frame->setVertexArray(_mVerticesFrame);
    _frame->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

    _mGeometryNormal->setVertexArray(_mVerticesNormal);
    _mGeometryNormal->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
    _mGeometryNormal->setColorArray(colorNormal, osg::Array::BIND_PER_VERTEX);

    _pickable->setVertexArray(_mVerticesPickable);
    _pickable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    _axis->setVertexArray(_mVerticesAxis);
    _axis->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,4));
    _axis->setColorArray(colorAxis, osg::Array::BIND_PER_VERTEX);
}

void Canvas::setColor(osg::Vec4f color)
{
    if (color == dureu::CANVAS_CLR_CURRENT) // hide the axis for "rest" canvases
        _switchFrame->setChildValue(_geodeAxis, true);
    else
        _switchFrame->setChildValue(_geodeAxis, false);

    _color = color;
    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = _color;
    (*colors)[1] = _color;
    (*colors)[2] = _color;
    (*colors)[3] = _color;
    _frame->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    _pickable->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}

osg::Vec4f Canvas::getColor() const{
    return _color;
}

void Canvas::setVisibility(bool vis)
{
    _switch->setChildValue(_switch->getChild(0), vis);
}

bool Canvas::getVisibility() const{
    return _switch->getChildValue(_switch->getChild(0));
}

void Canvas::setVisibilityLocalAxis(bool vis)
{
    _switchFrame->setChildValue(_geodeAxis, vis);
}

bool Canvas::getVisibilityLocalAxis() const
{
    return _switchFrame->getChildValue(_geodeAxis);
}

void Canvas::setTransformPost(osg::MatrixTransform *t){
    osg::Matrix matrix = t->getMatrix();
    _transform->postMult(matrix);
    this->transformData(matrix);
}

void Canvas::setTransformPost(const osg::Matrix &m)
{
    _transform->postMult(m);
    this->transformData(m);
}

void Canvas::setTransformPre(osg::MatrixTransform *r)
{
    osg::Matrix matrix = r->getMatrix();
    _transform->preMult(matrix);
    this->transformData(matrix);
}

osg::MatrixTransform *Canvas::getTransform() const
{
    return _transform.get();
}

void Canvas::setCanvasName(const std::string &name)
{
    this->setName(name);
    this->setSwitchName(name);
    this->setTransformName(name);
    this->setSwitchFrameName(name);
    this->setGeodeDataName(name);
    this->setPhotoNames(name);
}

void Canvas::setSwitchName(const std::string &parentName)
{
    _switch->setName(parentName + "Switch");
}

void Canvas::setTransformName(const std::string &parentName)
{
    _transform->setName(parentName + "Transform");
}

void Canvas::setSwitchFrameName(const std::string &parentName)
{
    _switchFrame->setName(parentName + "SwitchFrame");
}

void Canvas::setGeodeDataName(const std::string &parentName)
{
    _geodeData->setName(parentName + "GeodeData");
}

void Canvas::setPhotoNames(const std::string &parentName)
{
    // for all the photos within canvas
    // set name of each based on parent name
}

std::string Canvas::getSwitchName() const{
    return _switch->getName();
}

std::string Canvas::getTransformName() const{
    return _transform->getName();
}

std::string Canvas::getSwitchFrameName() const
{
    return _switchFrame->getName();
}

std::string Canvas::getGeodeDataName() const{
    return _geodeData->getName();
}

// Near and far are the points representing the ray which is casted by a mouse
// screen coordinates.
// The function takes intersection between the ray and our plane (canvas)
// Then transforms that intersection point into an original local coordinate system.
// The result point is appended to the current stroke.
void Canvas::addStroke(const double u, const double v, int mouse)
{
    if (mouse == 0 || (mouse==1 && !_strokeCurrent.get())){
        debugLogMsg("addStroke(): initialization");
        assert(_strokeCurrent.get() == 0);
        Stroke* stroke  = new Stroke; // an empty stroke structure
        assert(stroke);
        _strokeCurrent = stroke;
        if (!_strokeCurrent.valid()){
            std::cerr << "addStroke(): _strokeCurrent is NULL after initialization" << std::endl;
            return;
        }
        assert(_geodeData.get());
        if (!_geodeData->addDrawable(stroke)){
            std::cerr << "addStroke(): could not add stroke as a child to the current canvas" << std::endl;
            _strokeCurrent = 0;
            return;
        }
    }
    assert(_strokeCurrent.get());
    _strokeCurrent->appendPoint(u,v);

    this->updateFrame();

    if (mouse == 2){
        float len = _strokeCurrent->getLength();
        if (len < dureu::STROKE_MINL){
            if (!this->deleteStroke(_strokeCurrent.get()))
                debugErrMsg("addStroke(): as the stroke was short, it was attempted to delete it, but failed");
        }
        _strokeCurrent = 0;
        //this->updateData();
        std::cout << "addStroke(): finished stroke, observer pointer cleared" << std::endl;
    }
}

bool Canvas::deleteStroke(Stroke *stroke)
{
    if (!stroke){
        debugErrMsg("deleteStroke(): pointer is NULL");
        return false;
    }
    return _geodeData->removeChild(stroke);
}

void Canvas::addPhoto(Photo *photo, const double u, const double v)
{
    if (!photo){
        debugErrMsg("addPhoto(): photo pointer is NULL");
        return;
    }
    if (!_geodeData.get()){
        debugErrMsg("addPhoto(): _geodeData pointer is NULL");
        return;
    }
    if (!_geodeData->addDrawable(photo)){
        debugErrMsg("addPhoto: could not add photo as a child to the _geodeData of current canvas");
        return;
    }
    _geodeData->getOrCreateStateSet()->setTextureAttributeAndModes(0, photo->getTexture());
    //this->setPhotoCurrent(photo);
}

void Canvas::movePhoto(Photo *photo, const double u, const double v, int mouse)
{
    if (!photo){
        debugErrMsg("movePhoto(): photo pointer is NULL");
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

void Canvas::setPhotoCurrent(Photo *photo)
{
    if (_photoCurrent.get() == photo)
        return;
    if (_photoCurrent.get()!=0)
        this->setPhotoCurrent(false);
    _photoCurrent = photo;
    this->setPhotoCurrent(true);
}

// changes photo frame color based on bool varialbe
void Canvas::setPhotoCurrent(bool current)
{
    if (!current)
        _photoCurrent->setFrameColor(dureu::PHOTO_CLR_REST);
    else
        _photoCurrent->setFrameColor(dureu::PHOTO_CLR_SELECTED);
}

void Canvas::updateFrame()
{
    osg::BoundingBox bb = _geodeData->getBoundingBox();
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
void Canvas::updateData()
{
    osg::BoundingBox bb = _geodeData->getBoundingBox();
    assert(bb.valid());

    debugLogVec("_center", _center.x(), _center.y(), _center.z());
    debugLogVec("bb.center", bb.center().x(), bb.center().y(), bb.center().z());
    osg::Matrix mat;
    mat.makeTranslate(bb.center() - _center);
    this->transformData(mat);
}

void Canvas::setModeOffset(bool on)
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
    _mSwitchNormal->setChildValue(_mGeodeNormal, on);
}

osg::Vec3f Canvas::getCenter() const
{
    return _center;
}

osg::Plane Canvas::getPlane() const
{
    osg::Plane plane(_normal, _center);
    return plane;
}

osg::Vec3f Canvas::getNormal() const
{
    return _normal;
}

Stroke *Canvas::getStrokeCurrent() const
{
    return _strokeCurrent.get();
}

void Canvas::finishStrokeCurrent()
{
    assert(_strokeCurrent.get());
    _strokeCurrent = 0;
    std::cout << "finishStrokeCurrent(): finished stroke, observer pointer cleared" << std::endl;
    this->updateFrame();
    //this->updateData();
}

Photo *Canvas::getPhotoCurrent() const
{
    return _photoCurrent.get();
}

// to transform plane, centroid and local axis
// must be called every time when transform node is changed
void Canvas::transformData(const osg::Matrix &matrix)
{
    osg::Plane plane(_normal, _center);
    _center = _center * matrix;
    plane.transform(matrix);
    _normal = plane.getNormal();
    debugLogVec("transformData(): Canvas center", _center.x(),_center.y(),_center.z());
    debugLogVec("transformData(): Canvas normal", _normal.x(), _normal.y(), _normal.z());
    assert(plane.valid());
}

void Canvas::setVertices(const osg::Vec3f &center, float szX, float szY, float szCr, float szAx)
{
    assert(szX>=dureu::CANVAS_MINW && szY>=dureu::CANVAS_MINH);

    (*_mVerticesFrame)[0] = center + osg::Vec3(szX,szY,0.f);
    (*_mVerticesFrame)[1] = center + osg::Vec3(-szX,szY,0.f);
    (*_mVerticesFrame)[2] = center + osg::Vec3(-szX,-szY,0.f);
    (*_mVerticesFrame)[3] = center + osg::Vec3(szX,-szY,0.f);
    _frame->dirtyDisplayList();
    _frame->dirtyBound();

    assert(szCr >= dureu::CANVAS_CORNER);
    osg::Vec3 p0 = (*_mVerticesFrame)[0];
    (*_mVerticesPickable)[0] = p0;
    (*_mVerticesPickable)[1] = p0 + osg::Vec3(-szCr, 0.f, 0.f);
    (*_mVerticesPickable)[2] = p0 + osg::Vec3(-szCr, -szCr, 0.f);
    (*_mVerticesPickable)[3] = p0 + osg::Vec3(.0f, -szCr, 0.f);
    _pickable->dirtyDisplayList();
    _pickable->dirtyBound();

    assert(szAx>=dureu::CANVAS_AXIS);
    (*_mVerticesAxis)[0] = center;
    (*_mVerticesAxis)[1] = center + osg::Vec3(szAx,0.f,0.f);
    (*_mVerticesAxis)[2] = center;
    (*_mVerticesAxis)[3] = center + osg::Vec3(0.f, szAx, 0.f);
    _axis->dirtyDisplayList();
    _axis->dirtyBound();

    float szN = std::max(szX,szY);
    (*_mVerticesNormal)[0] = center;
    (*_mVerticesNormal)[1] = center + osg::Vec3f(0.f,0.f, szN);
}


