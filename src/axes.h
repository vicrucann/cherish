#ifndef AXES
#define AXES

#include <osg/Geode>

class Axes : public osg::Geode {
public:
    Axes(const osg::Vec3 &corner, const osg::Vec3 &xdir, const osg::Vec3 &ydir, const osg::Vec3 &zdir);
    ~Axes();
private:
};

#endif // AXES

