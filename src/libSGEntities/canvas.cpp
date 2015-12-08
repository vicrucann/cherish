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
    _strokeCurrent(0),

    _center(osg::Vec3f(0.f,0.f,0.f)), // moves only when strokes are introduced so that to define it as centroid
    // also represents local coord system center
    _plane(osg::Vec3f(0.f,0.f,1.f), _center), // plane params by center and normal
    _x(_center + osg::Vec3f(1.f,0.f,0.f)), // x and y local coordinate system vectors, for the moment they will be
    _y(_center + osg::Vec3f(0.f,1.f,0.f)), // X and Y (global)-axis aligned
    _color(dureu::CANVAS_CLR_REST) // frame and pickable color

{
    this->transformData();
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

    _transformData->addChild(_geodeData.get());
    // _geodeData is  empty, it is for user input: strokes

    _switchFrame->addChild(_geodeFrame.get(), true); // child #0
    _switchFrame->addChild(_geodeAxis.get(), true); // child #1

    _geodeFrame->addDrawable(_frame.get());
    _geodeFrame->addDrawable(_pickable.get());

    _geodeAxis->addDrawable(_axis.get());

     osg::Vec3Array* frameVertices = new osg::Vec3Array(4);
    (*frameVertices)[0] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*frameVertices)[1] = osg::Vec3f(-dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*frameVertices)[2] = osg::Vec3f(-dureu::CANVAS_MINW, -dureu::CANVAS_MINH, 0.0f);
    (*frameVertices)[3] = osg::Vec3f(dureu::CANVAS_MINW, -dureu::CANVAS_MINH, 0.0f);

     osg::Vec3Array* pickableVertices = new osg::Vec3Array(4);
    (*pickableVertices)[0] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*pickableVertices)[1] = osg::Vec3f(dureu::CANVAS_MINW-dureu::CANVAS_CORNER, dureu::CANVAS_MINH, 0.0f);
    (*pickableVertices)[2] = osg::Vec3f(dureu::CANVAS_MINW-dureu::CANVAS_CORNER, dureu::CANVAS_MINH-dureu::CANVAS_CORNER, 0.0f);
    (*pickableVertices)[3] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH-dureu::CANVAS_CORNER, 0.0f);

     osg::Vec3Array* axisVertices = new osg::Vec3Array(4);
     (*axisVertices)[0] = osg::Vec3f(0.f,0.f,0.f);
     (*axisVertices)[1] = osg::Vec3f(dureu::CANVAS_AXIS, 0.f,0.f);
     (*axisVertices)[2] = osg::Vec3f(0.f,0.f,0.f);
     (*axisVertices)[3] = osg::Vec3f(0.f,dureu::CANVAS_AXIS, 0.f);

     osg::Vec4Array* colorAxis = new osg::Vec4Array(4);
     (*colorAxis)[0] = solarized::base2;
     (*colorAxis)[1] = solarized::base2;
     (*colorAxis)[2] = solarized::base2;
     (*colorAxis)[3] = solarized::base2;

    _frame->setVertexArray(frameVertices);
    _frame->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

    _pickable->setVertexArray(pickableVertices);
    _pickable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    _axis->setVertexArray(axisVertices);
    _axis->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,4));
    _axis->setColorArray(colorAxis, osg::Array::BIND_PER_VERTEX);
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

void Canvas::setTransform(osg::MatrixTransform *transform){
    _transform = transform;
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
void Canvas::addStroke(const osg::Vec3f &nearPoint, const osg::Vec3f &farPoint, int mouse)
{
    assert(_plane.valid());
    std::vector<osg::Vec3f> ray(2);
    ray[0] = nearPoint;
    ray[1] = farPoint;
    if (_plane.intersect(ray)){ // 1 or -1: no intersection
        std::cerr << "addStroke(): no intersection with the ray" << std::endl;
        return;
    }
    osg::Vec3f dir = farPoint-nearPoint;
    if (! _plane.dotProductNormal(dir)){ // denominator
        std::cerr << "addStroke(): projected line is parallel to the canvas plane" << std::endl;
        return;
    }
    if (! _plane.dotProductNormal(_center-nearPoint)){
        std::cerr << "addStroke(): plane contains the line, so no single intersection can be defined" << std::endl;
        return;
    }

    double x = _plane.dotProductNormal(_center-nearPoint) / _plane.dotProductNormal(dir);
    osg::Vec3f P = dir * x + nearPoint;

    osg::Matrix mat =  this->getTransform()->getMatrix();
    osg::Matrix invmat;
    if (!invmat.invert(mat)){
        std::cerr << "addStroke(): could not invert model matrix" << std::endl;
        return;
    }
    osg::Vec3f p = P * invmat;

    double u=p.x(), v=p.y();
    //double u = _x * (P-_center);
    //double v = _y * (P-_center);

    bool success = true;
    if (mouse == 0){
        //Stroke* stroke  = new Stroke(osg::Vec2f(0,0), osg::Vec2f(u,v));
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
    assert(_strokeCurrent);
    // append the (u,v) point to the stroke
    _strokeCurrent->appendPoint(u,v);
    if (mouse == 2){
        _strokeCurrent = 0;
        std::cout << "addStroke(): finished stroke, observer pointer cleared" << std::endl;
    }
}

// to transform plane, centroid and local axis
// must be called every time when transform node is changed
void Canvas::transformData()
{
    const osg::Matrix matrix = _transform->getMatrix();
    _plane.transform(matrix); // every time canvas is transformed (rotate, offset, scale), apply it for plane params
    _plane.makeUnitLength(); // then normalize the params
    assert(_plane.valid());
    _center = matrix * _center;
    _x = matrix * _x;
    _x.normalize();
    _y = matrix * _y;
    _y.normalize();
}

void Canvas::setColor(osg::Vec4 color){
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
