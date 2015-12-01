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
    _center(osg::Vec3f(0.0f, 0.0f, 0.0f)),
    _normal(osg::Vec3f(0.0f, -1.0f, 0.0f)),
    _color(dureu::CANVAS_CLR_REST),
    _vertices(new osg::Vec3Array(4)),
    _corner(new osg::Vec3Array(4)),
    _geode(new osg::Geode),
    _geometry(new osg::Geometry),
    _pickable(new osg::Geometry),
    _transform(transform),
    _switch(new osg::Switch)
{
    this->setName(name);
    (*_vertices)[0] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*_vertices)[1] = osg::Vec3f(-dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*_vertices)[2] = osg::Vec3f(-dureu::CANVAS_MINW, -dureu::CANVAS_MINH, 0.0f);
    (*_vertices)[3] = osg::Vec3f(dureu::CANVAS_MINW, -dureu::CANVAS_MINH, 0.0f);

    (*_corner)[0] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*_corner)[1] = osg::Vec3f(dureu::CANVAS_MINW-dureu::CANVAS_CORNER, dureu::CANVAS_MINH, 0.0f);
    (*_corner)[2] = osg::Vec3f(dureu::CANVAS_MINW-dureu::CANVAS_CORNER, dureu::CANVAS_MINH-dureu::CANVAS_CORNER, 0.0f);
    (*_corner)[3] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH-dureu::CANVAS_CORNER, 0.0f);

    this->addCanvasDrawables();

    _transform->addChild(_geode.get());
    _transform->setName(this->getName() + "Transform");
    _switch->addChild(_transform.get(), true);
    _switch->setName(this->getName() + "Switch");
    _geode->setName(this->getName() + "Geode");
    _geometry->setName(this->getName() + "Geometry");
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

void Canvas::testWidthPlus()
{
    osg::Vec2Array* test = new osg::Vec2Array(4);
    (*test)[0].set(1.f, 1.f);
    (*test)[1].set(0.f, 1.f);
    (*test)[2].set(0.f, 0.f);
    (*test)[3].set(1.f, 0.f);

    //(*_vertices)[0] += osg::Vec3f(0.5f, 0.f, 0.2f);
    _geometry->setVertexArray(test);
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
    _geometry->setName(parentName + "Geometry");
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
    return _geometry->getName();
}

std::string Canvas::getGeodeName() const{
    return _geode->getName();
}

void Canvas::addCanvasDrawables(){
    _geometry->setVertexArray(_vertices);
    _pickable->setVertexArray(_corner);

    this->setColor(_color);

    _geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));
    _pickable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    _geode->addDrawable(_geometry);
    _geode->addDrawable(_pickable);
}

void Canvas::setColor(osg::Vec4 color){
    _color = color;
    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = _color;
    (*colors)[1] = _color;
    (*colors)[2] = _color;
    (*colors)[3] = _color;
    _geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    _pickable->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}
