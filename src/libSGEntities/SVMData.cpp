#include "SVMData.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Point>

entity::SVMData::SVMData()
    : osg::ProtectedGroup()
    , m_switch(new osg::Switch)
    , m_transform(new osg::MatrixTransform)
    , m_geode(new osg::Geode)
{
    this->addChild(m_switch.get());
    m_switch->addChild(m_transform.get());
    m_transform->addChild(m_geode.get());

    // populate geode with initial geometry
    osg::Vec3Array* verts = new osg::Vec3Array;
    verts->push_back(osg::Vec3f(0.1f, 0.1f, 0.f));
    verts->push_back(osg::Vec3f(-0.1f, 0.1f, 0.f));
    verts->push_back(osg::Vec3f(-0.1f, -0.1f, 0.f));
    verts->push_back(osg::Vec3f(0.1f, -0.1f, 0.f));

    osg::ref_ptr<osg::Geometry> geomLine = new osg::Geometry;
    geomLine->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 0, verts->size()));
    geomLine->setVertexArray(verts);

    osg::ref_ptr<osg::Geometry> geomPts = new osg::Geometry;
    geomPts->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, verts->size()));
    geomPts->setVertexArray(verts);

    m_geode->addDrawable(geomLine.get());
    m_geode->addDrawable(geomPts.get());

    osg::StateSet* ss = m_geode->getOrCreateStateSet();
    ss->setAttribute(new osg::Point(10.f), osg::StateAttribute::ON);
}

void entity::SVMData::setTransform(osg::Matrix m)
{
    m_transform->setMatrix(m);
}
