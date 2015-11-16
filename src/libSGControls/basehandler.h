#ifndef BASEHANDLER
#define BASEHANDLER

/* BaseHandler
 * This is the base event handler class for all the user-defined
 * handlers. Its main characteristic is the usage of the middle
 * mouse button.
 * Middle mouse button press and drag: rotate the camera view.
 * Middle mouse button scroll: zoom-in and zoom-out.
 * The middle mouse button functionality will be present for any
 * other user-defined handlers, EXCEPT: in FixedViewHandler where
 * the rotation is overriden (replaced by zoom).
 * For more details on OSG GUIEventHandler: see p. 232-234, Open
 * Scene Graph: beginner's guide
 * The base class is GUIEventHandler and not StandardManipulator or
 * similar because we want to be able to process enum EventType which
 * includes not only mouse and keyboard, but also Wacom pen events.
 */

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/observer_ptr>
#include <osg/Matrix>

#include "rootscene.h"

class BaseHandler : public osgGA::GUIEventHandler {
public:
    BaseHandler();
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    virtual void setTransformation (const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);
    virtual void getTransformation (osg::Vec3d &eye, osg::Vec3d &center, osg::Vec3d &up) const;
protected:
    virtual void 	zoomModel (const float dy, bool pushForwardIfNeeded=true);

    // same camera parameters as in osgGA::OrbitManipulator
    osg::Vec3 _center;
    osg::Quat _rotation;
    double _distance;
    double _trackballSize;
    double _wheelZoomFactor;
    double _minimumDistance;
};



#endif // BASEHANDLER

