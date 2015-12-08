#include "stroke.h"

#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/BlendFunc>

Stroke::Stroke():
    _mDrawArrayLines(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP)),
    _mVertexData(new osg::Vec3Array),
    _mColors(new osg::Vec4Array)
{
    this->addPrimitiveSet(_mDrawArrayLines);
    this->setVertexArray(_mVertexData);
    this->setColorArray(_mColors, osg::Array::BIND_PER_VERTEX);

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(2.0);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    //blendfunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ANTIALIAS);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    this->setStateSet(stateset);
    std::cout << "Stroke(): ctor complete" << std::endl;
}

void Stroke::appendPoint(float u, float v)
{
    _mVertexData->push_back(osg::Vec3f(u,v,0.f));
    _mColors->push_back(dureu::STROKE_CLR_NORMAL);
    _mDrawArrayLines->setFirst(0);
    _mDrawArrayLines->setCount(_mVertexData->size());
    this->dirtyDisplayList(); // so that frame() is called to update
    this->dirtyBound();
}

void Stroke::setColor(osg::Vec4f color)
{
    for (unsigned int i = 0; i<_mColors->size(); ++i)
        (*_mColors)[i] = color;
}
