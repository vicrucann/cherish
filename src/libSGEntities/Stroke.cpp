#include "Stroke.h"

#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/BlendFunc>
#include <osg/Point>

Stroke::Stroke():
    mDrawArrayLines(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP)),
    mDrawArrayPoints(new osg::DrawArrays(osg::PrimitiveSet::POINTS)),
    mVertexData(new osg::Vec3Array),
    mColors(new osg::Vec4Array)
{
    noticeMsg("New stroke started");
    this->addPrimitiveSet(mDrawArrayLines);
    this->addPrimitiveSet(mDrawArrayPoints);
    this->setVertexArray(mVertexData);
    this->setColorArray(mColors);
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

    noticeMsg("New stroke ctor complete");
}

Stroke::Stroke(const Stroke &copy, const osg::CopyOp &copyop)
    : osg::Geometry(copy, copyop)
{
}

void Stroke::appendPoint(const float u, const float v)
{
    mVertexData->push_back(osg::Vec3f(u,v,0.f));
    mColors->push_back(dureu::STROKE_CLR_NORMAL);
    unsigned int sz = mVertexData->size();
    mDrawArrayLines->setFirst(0);
    mDrawArrayLines->setCount(sz);

    mDrawArrayPoints->setFirst(0);
    mDrawArrayPoints->setCount(sz);

    mVertexData->dirty();
    mColors->dirty();

    //this->dirtyDisplayList(); // so that frame() is called to update
    this->dirtyBound();
    // read more: http://forum.openscenegraph.org/viewtopic.php?t=2190&postdays=0&postorder=asc&start=15
}

float Stroke::getLength() const
{
    osg::BoundingBox bb = this->getBoundingBox();
    if (std::fabs(bb.zMax()-bb.zMin()) > dureu::EPSILON ){
        warningMsg("Stroke->getLength(): z coordinates of a stroke are unexpected values");
        warningVal("zMax", bb.zMax());
        warningVal("zMin", bb.zMin());
        return 0;
    }
    return std::max(bb.xMax() - bb.xMin(), bb.yMax() - bb.yMin());
}

void Stroke::setColor(const osg::Vec4f &color)
{
    mColors->front() = color;
    mColors->dirty();
    /*for (unsigned int i = 0; i<mColors->size(); ++i)
        (*mColors)[i] = color;*/
}
