#ifndef MANIPULATOR
#define MANIPULATOR

#include "settings.h"
#include "rotatemanipulator.h"
#include "panmanipulator.h"
#include "zoommanipulator.h"

#include <osg/ref_ptr>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/TrackballManipulator>

class Manipulator : public osgGA::TrackballManipulator {
public:
    Manipulator(dureu::MANIP_MODE mode = dureu::MANIP_NONE);
protected:
    //virtual bool handleMouseMove (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);
    //virtual bool handleMouseDrag (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);
    //virtual bool handleMousePush (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);
    //virtual bool handleMouseRelease (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);

private:
    int _mode;
    osg::ref_ptr<RotateManipulator> _RM;
    osg::ref_ptr<PanManipulator> _PM;
    osg::ref_ptr<ZoomManipulator> _ZM;
};

#endif // MANIPULATOR

