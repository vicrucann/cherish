#ifndef AXES
#define AXES

#include <osg/Camera>

class Axes : public osg::Camera {
public:
    Axes(const osg::Vec3 &corner, const osg::Vec3 &xdir, const osg::Vec3 &ydir, const osg::Vec3 &zdir);
    ~Axes();
private:
};

#endif // AXES

