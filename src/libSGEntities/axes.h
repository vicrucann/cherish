#ifndef AXES
#define AXES

/* Axes branch structure:
 * Group -> Switch -> Camera -> Geode -> Drawables
 * Inherits from osg::Group so that to hold any structure we want
 * osg::Switch variable is used to set up visibility of the axes
 * osg::Camera variable sets up the drawables to be always post rendered
 * osg::Geode contains all the drawables of the axes
 * osg::Drawable variable contans lines representing the axes
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

