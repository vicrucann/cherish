#include "stroke.h"

Stroke::Stroke()
{
    osg::ref_ptr<osg::Vec2Array> verts = new osg::Vec2Array;
    verts->push_back(osg::Vec2(0.f, 0.f));
    verts->push_back(osg::Vec2(1.f,1.f));
    this->setVertexArray(verts);
    this->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

    this->setColor(dureu::STROKE_CLR_NORMAL);
}

bool Stroke::addPoint(float u, float v)
{
    // push_back the point to the vertex data (primitive set)
    //this->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,1));
    return false;

}

void Stroke::setColor(osg::Vec4f color)
{
    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = color;
    (*colors)[1] = color;
    (*colors)[2] = color;
    (*colors)[3] = color;
    this->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}
