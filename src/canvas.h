#ifndef CANVAS
#define CANVAS

#include <osg/Group>

class Canvas : public osg::Group {
public:
    Canvas(){}
    //Canvas(osg::Vec3 center, osg::Vec3 normal, osg::Vec4 color, int width, int height);
    ~Canvas(){}

private:
    osg::Vec3 _center;
    osg::Vec3 _normal;
    osg::Vec4 _color;
    int _width, _height;
    bool _current, _previous;
};

#endif // CANVAS

