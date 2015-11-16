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
 */

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

class BaseHandler : public osgGA::GUIEventHandler {
public:
    BaseHandler();
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
private:
};



#endif // BASEHANDLER

