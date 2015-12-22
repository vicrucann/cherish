#include "stroke.h"

#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/BlendFunc>
#include <osg/Point>

Stroke::Stroke():
    _mDrawArrayLines(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP)),
    _mDrawArrayPoints(new osg::DrawArrays(osg::PrimitiveSet::POINTS)),
    _mVertexData(new osg::Vec3Array),
    _mColors(new osg::Vec4Array)
{
    this->addPrimitiveSet(_mDrawArrayLines);
    this->addPrimitiveSet(_mDrawArrayPoints);
    this->setVertexArray(_mVertexData);
    this->setColorArray(_mColors);
    this->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(dureu::STROKE_LINE_WIDTH);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    //blendfunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ANTIALIAS);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setAttributeAndModes(new osg::Point(dureu::STROKE_LINE_WIDTH));
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    this->setStateSet(stateset);

    this->setDataVariance(osg::Object::DYNAMIC);
    this->setUseDisplayList(false);
    this->setUseVertexBufferObjects(true);

    std::cout << "Stroke(): ctor complete" << std::endl;
}

void Stroke::appendPoint(float u, float v)
{
    _mVertexData->push_back(osg::Vec3f(u,v,0.f));
    _mColors->push_back(dureu::STROKE_CLR_NORMAL);
    unsigned int sz = _mVertexData->size();
    _mDrawArrayLines->setFirst(0);
    _mDrawArrayLines->setCount(sz);

    _mDrawArrayPoints->setFirst(0);
    _mDrawArrayPoints->setCount(sz);

    _mVertexData->dirty();
    _mColors->dirty();

    //this->dirtyDisplayList(); // so that frame() is called to update
    this->dirtyBound();
    // read more: http://forum.openscenegraph.org/viewtopic.php?t=2190&postdays=0&postorder=asc&start=15
}

float Stroke::getLength() const
{
    osg::BoundingBox bb = this->getBoundingBox();
    if (std::fabs(bb.zMax()-bb.zMin()) > dureu::EPSILON ){
        debugErrMsg("getLength: z coordinates of a stroke are unexpected values");
        debugLogVal("zMax", bb.zMax());
        debugLogVal("zMin", bb.zMin());
        return 0;
    }
    return std::max(bb.xMax() - bb.xMin(), bb.yMax() - bb.yMin());
}

void Stroke::setColor(osg::Vec4f color)
{
    _mColors->front() = color;
    _mColors->dirty();
    /*for (unsigned int i = 0; i<_mColors->size(); ++i)
        (*_mColors)[i] = color;*/
}
