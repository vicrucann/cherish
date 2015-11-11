#ifndef ROOTSCENE
#define ROOTSCENE

#include <iostream>
#include <string>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Geode>

#include "axes.h"

class RootScene : public osg::Group {
public:
    RootScene();
    ~RootScene();

    bool loadSceneFromFile(const std::string fname);
private:
    osg::ref_ptr<osg::Group> _userScene;
    osg::ref_ptr<Axes> _axes;
};

#endif // SCENE

