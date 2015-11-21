#ifndef AXES
#define AXES

/* Axes branch structure:
 * Group -> Switch -> Camera -> Geode -> Drawables
 */

#include <osg/ref_ptr>
#include <osg/Switch>
#include <osg/Camera>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Node>

class Axes : public osg::Group {
public:
    Axes();
    ~Axes();

    void setVisibility(bool vis);

private:
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Geode> _geode;
    osg::ref_ptr<osg::Camera> _camera;
    osg::ref_ptr<osg::Switch> _switch;
};

#endif // AXES

