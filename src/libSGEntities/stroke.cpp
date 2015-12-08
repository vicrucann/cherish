#include "stroke.h"

Stroke::Stroke():
    _mDrawArrayLines(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP)),
    _mVertexData(new osg::Vec3Array),
    _mColors(new osg::Vec4Array)
{
    this->addPrimitiveSet(_mDrawArrayLines);
    this->setVertexArray(_mVertexData);
    this->setColorArray(_mColors, osg::Array::BIND_PER_VERTEX);
    std::cout << "Stroke(): ctor complete" << std::endl;
}

void Stroke::appendPoint(float u, float v)
{
    _mVertexData->push_back(osg::Vec3f(u,v,0.f));
    _mColors->push_back(dureu::STROKE_CLR_NORMAL);
    _mDrawArrayLines->setFirst(0);
    _mDrawArrayLines->setCount(_mVertexData->size());
    std::cout << "appendPoint(): complete" << std::endl;
}

void Stroke::setColor(osg::Vec4f color)
{
    for (unsigned int i = 0; i<_mColors->size(); ++i)
        (*_mColors)[i] = color;
}
