#ifndef BASEMANIPULATOR
#define BASEMANIPULATOR

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/TrackballManipulator>

class BaseManipulator : public osgGA::TrackballManipulator {
public:
    BaseManipulator();
    bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);

    virtual void setTransformation( const osg::Vec3d&, const osg::Quat& ) {}
    virtual void setTransformation( const osg::Vec3d&, const osg::Vec3d&, const osg::Vec3d& ) {}
    virtual void getTransformation( osg::Vec3d&, osg::Quat& )const {}
    virtual void getTransformation( osg::Vec3d&, osg::Vec3d&, osg::Vec3d& ) const {}
protected:
    ~BaseManipulator();
    virtual bool performMovementLeftMouseButton (const double eventTimeDelta, const double dx, const double dy);
    virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );
    virtual bool wrapRotation(const double eventTimeDelta, const double dx, const double dy);
};

#endif // BASEMANIPULATOR

