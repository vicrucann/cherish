#include "axes.h"
#include "settings.h"

#include <osg/Drawable>
#include <osg/ref_ptr>
#include <osg/Geometry>
#include <osg/Vec3>
#include <osg/BlendFunc>
#include <osg/LineWidth>

Axes::Axes(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::Vec3Array* coords = new osg::Vec3Array(6);
    (*coords)[0] = corner;
    (*coords)[1] = corner+xdir;
    (*coords)[2] = corner;
    (*coords)[3] = corner+ydir;
    (*coords)[4] = corner;
    (*coords)[5] = corner+zdir;

    geom->setVertexArray(coords);
    osg::Vec4Array* color = new osg::Vec4Array(6);
    (*color)[0] = dureu::AXES_CLR_X;
    (*color)[1] = dureu::AXES_CLR_X;
    (*color)[2] = dureu::AXES_CLR_Y;
    (*color)[3] = dureu::AXES_CLR_Y;
    (*color)[4] = dureu::AXES_CLR_Z;
    (*color)[5] = dureu::AXES_CLR_Z;

    geom->setColorArray(color, osg::Array::BIND_PER_VERTEX);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(3.0f);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    //blendfunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ANTIALIAS);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    geom->setStateSet(stateset);
    this->addDrawable(geom.get());
}

Axes::~Axes(){}
