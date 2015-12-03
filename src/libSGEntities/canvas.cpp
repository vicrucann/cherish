#include <assert.h>
#include <iostream>

#include "canvas.h"
#include "settings.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/BoundingBox>
#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/Plane>
#include <osg/BlendFunc>

Canvas::Canvas(osg::MatrixTransform *transform, const std::string &name):
    _frame(new osg::Geometry),
    _pickable(new osg::Geometry),
    _axis(new osg::Geometry),
    _swFrame(new osg::Switch),
    _swAxis(new osg::Switch),
    _geode(new osg::Geode),
    _transform(transform),
    _transformUD(new osg::MatrixTransform),
    _switch(new osg::Switch),

    _center(osg::Vec3f(0.0f, 0.0f, 0.0f)),
    _normal(osg::Vec3f(0.0f, -1.0f, 0.0f)),
    _plane(osg::Plane(_normal, _center)),
    _x(osg::Vec3f(1.f,0.f,0.f)),
    _y(osg::Vec3f(0.f,1.f,0.f)),
    _color(dureu::CANVAS_CLR_REST)

{
    this->setName(name);
    this->setColor(_color);

     osg::Vec3Array* frame = new osg::Vec3Array(4);
    (*frame)[0] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*frame)[1] = osg::Vec3f(-dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*frame)[2] = osg::Vec3f(-dureu::CANVAS_MINW, -dureu::CANVAS_MINH, 0.0f);
    (*frame)[3] = osg::Vec3f(dureu::CANVAS_MINW, -dureu::CANVAS_MINH, 0.0f);

     osg::Vec3Array* pickable = new osg::Vec3Array(4);
    (*pickable)[0] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*pickable)[1] = osg::Vec3f(dureu::CANVAS_MINW-dureu::CANVAS_CORNER, dureu::CANVAS_MINH, 0.0f);
    (*pickable)[2] = osg::Vec3f(dureu::CANVAS_MINW-dureu::CANVAS_CORNER, dureu::CANVAS_MINH-dureu::CANVAS_CORNER, 0.0f);
    (*pickable)[3] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH-dureu::CANVAS_CORNER, 0.0f);

    _frame->setVertexArray(frame);
    _frame->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));
    _pickable->setVertexArray(pickable);
    _pickable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    _geode->addDrawable(_frame);
    _geode->addDrawable(_pickable);

    _transform->addChild(_geode.get());
    _transform->setName(this->getName() + "Transform");
    _switch->addChild(_transform.get(), true);
    _switch->setName(this->getName() + "Switch");
    _geode->setName(this->getName() + "Geode");
    _frame->setName(this->getName() + "Geometry");
    _pickable->setName(this->getName() + "Pickable");
    this->addChild(_switch.get());

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

void Canvas::setSwitchName(const std::string &parentName)
{
    _switch->setName(parentName + "Switch");
}

void Canvas::setTransformName(const std::string &parentName)
{
    _transform->setName(parentName + "Transform");
}

void Canvas::setGeometryName(const std::string &parentName)
{
    _frame->setName(parentName + "Geometry");
}

void Canvas::setGeodeName(const std::string &parentName)
{
    _geode->setName(parentName + "Geode");
}

std::string Canvas::getSwitchName() const{
    return _switch->getName();
}

std::string Canvas::getTransformName() const{
    return _transform->getName();
}

std::string Canvas::getGeometryName() const{
    return _frame->getName();
}

std::string Canvas::getGeodeName() const{
    return _geode->getName();
}

void Canvas::setColor(osg::Vec4 color){
    _color = color;
    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = _color;
    (*colors)[1] = _color;
    (*colors)[2] = _color;
    (*colors)[3] = _color;
    _frame->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    _pickable->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}
