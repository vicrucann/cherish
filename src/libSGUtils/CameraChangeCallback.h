#ifndef CAMERACHANGECALLBACK_H
#define CAMERACHANGECALLBACK_H

#include <osg/observer_ptr>
#include <osg/Camera>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>
#include <osgGA/GUIEventAdapter>
#include <osgGA/EventQueue>

#include "../libSGControls/Manipulator.h"

class CameraChangeCallback : public osg::NodeCallback
{
public:
    CameraChangeCallback(Manipulator* man, osgGA::EventQueue* eq);
    virtual void operator ()(osg::Node* node, osg::NodeVisitor*);

private:
    osg::observer_ptr<Manipulator> m_manipulator;
    osg::observer_ptr<osgGA::EventQueue> m_eq;
    osg::Matrixd m_invM;
};

#endif // CAMERACHANGECALLBACK_H
