#ifndef ROOTSCENE
#define ROOTSCENE

#include "axes.h"
#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Geode>

class RootScene : public osg::Group {
public:
    RootScene();
    ~RootScene();
private:
    osg::ref_ptr<osg::Group> _userScene;
    osg::ref_ptr<Axes> _axes;
};

#endif // SCENE

