#ifndef AXES
#define AXES

#include <osg/Camera>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Node>

class Axes : public osg::Camera {
public:
    Axes(const osg::Vec3 &corner, const osg::Vec3 &xdir, const osg::Vec3 &ydir, const osg::Vec3 &zdir);
    ~Axes();

private:
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Geode> _geode;
};

#endif // AXES

