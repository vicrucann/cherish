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

Canvas::Canvas():
    _center(osg::Vec3f(0.0f, 0.0f, 0.0f)),
    _normal(osg::Vec3f(0.0f, 1.0f, 0.0f)),
    _color(dureu::CANVAS_CLR_REST),
    _vertices(new osg::Vec3Array(4))
{
    osg::Plane plane(_normal, _center);
    assert(plane.valid());
    osg::Vec4 params = plane.asVec4();
    (*_vertices)[0] = osg::Vec3f(dureu::CANVAS_MINW, 0.0f, dureu::CANVAS_MINH);
    (*_vertices)[1] = osg::Vec3f(-dureu::CANVAS_MINW, 0.0f, dureu::CANVAS_MINH);
    (*_vertices)[2] = osg::Vec3f(-dureu::CANVAS_MINW, 0.0f, -dureu::CANVAS_MINH);
    (*_vertices)[3] = osg::Vec3f(dureu::CANVAS_MINW, 0.0f, -dureu::CANVAS_MINH);
    this->addCanvasDrawables();
}

/* Given three points on a plane (canvas), color and bound margin;
 * initialize normal and canvas current size */
Canvas::Canvas(osg::Vec3f center, osg::Vec3f pA, osg::Vec3f pB, osg::Vec4f color):
    _center(center),
    _normal((pA - center)^(pB - center)), // cross product returns normal
    _color(color),
    _vertices(new osg::Vec3Array(4))
{
    (*_vertices)[0] = pA + _center;
    (*_vertices)[1] = pB + _center;
    (*_vertices)[2] = -pA + _center;
    (*_vertices)[3] = -pB + _center;
    osg::Plane plane(_normal, _center);
    assert(plane.valid());
    this->addCanvasDrawables();
}

void Canvas::addCanvasDrawables(){
    osg::Geometry* geom = new osg::Geometry;
    geom->setVertexArray(_vertices);

    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = _color;
    (*colors)[1] = _color;
    (*colors)[2] = _color;
    (*colors)[3] = _color;

    geom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

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
    geom->setStateSet(stateset);

    this->addDrawable(geom);

    //this->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
}
