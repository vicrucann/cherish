#include "CameraChangeCallback.h"

#include "Settings.h"

CameraChangeCallback::CameraChangeCallback(Manipulator *man, osgGA::EventQueue *eq)
    : osg::NodeCallback()
    , m_manipulator(man)
    , m_eq(eq)
    , m_invM(osg::Matrixd::identity())
{
}

void CameraChangeCallback::operator ()(osg::Node *node, osg::NodeVisitor *)
{
    if (node){
        osg::Camera* camera = dynamic_cast<osg::Camera*>(node);
        if (camera && m_manipulator.get() && m_eq.get()){
            outLogVal("EA event type", m_eq->getCurrentEventState()->getEventType());

            /*if (m_ea->getEventType() == osgGA::GUIEventAdapter::RELEASE){
                if (camera->getInverseViewMatrix() != m_invM){
                    outLogMsg("cam view changed");
                    m_invM = camera->getInverseViewMatrix();
                }
            }*/
        }
    }
}



