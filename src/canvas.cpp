#include "canvas.h"
#include "settings.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/BoundingBox>
#include <osg/LineWidth>
#include <osg/StateSet>

Canvas::Canvas():
    _center(osg::Vec3(0.0f, 0.0f, 0.0f)),
    _normal(osg::Vec3(0.0f, 0.0f, 0.0f)),
    _color(dureu::CANVAS_CLR_CURRENT),
    _bb(osg::BoundingBox(-dureu::CANVAS_MIN_WIDTH/2.0, 0, -dureu::CANVAS_MIN_HEIGHT/2.0,
                         dureu::CANVAS_MIN_WIDTH/2.0, 0, dureu::CANVAS_MIN_HEIGHT/2.0)),
    _boundMargin(dureu::CANVAS_MIN_BOUND_MARGIN)
{
    this->addCanvasDrawables();
}

Canvas::Canvas(osg::Vec3 center, osg::BoundingBox bb, osg::Vec4 color, double boundMargin):
    _center(center),
    _bb(bb),
    _color(color),
    _boundMargin(boundMargin)
{
    this->addCanvasDrawables();
}

void Canvas::addCanvasDrawables(){
    osg::Vec3 top_left(_bb.xMin(), _bb.yMin(), _bb.zMax());
    osg::Vec3 bottom_left(_bb.xMin(), _bb.yMin(), _bb.zMin());
    osg::Vec3 bottom_right(_bb.xMax(), _bb.yMin(), _bb.zMin());
    osg::Vec3 top_right(_bb.xMax(), _bb.yMin(), _bb.zMax());

    osg::Geometry* geom = new osg::Geometry;
    osg::Vec3Array* vertices = new osg::Vec3Array(4);
    (*vertices)[0] = top_left;
    (*vertices)[1] = bottom_left;
    (*vertices)[2] = bottom_right;
    (*vertices)[3] = top_right;
    geom->setVertexArray(vertices);

    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = _color;
    (*colors)[1] = _color;
    (*colors)[2] = _color;
    (*colors)[3] = _color;

    geom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(3.0f);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    geom->setStateSet(stateset);

    this->addDrawable(geom);

    //this->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
}
