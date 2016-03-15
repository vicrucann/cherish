#ifndef EVENTHANDLER
#define EVENTHANDLER

/* EventHandler
 * It is represented as a switcher between all the other
 * user-defined handlers. Depending on the mouse mode (select, edit,
 * navigate, sketch, etc), it choses the right handle() function of the
 * corresponding event handler. Of course, all the handlers must be
 * initialized in the contstructor. The mode can be reset by a user class.
*/

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "Settings.h"
#include "Canvas.h"
#include "Photo.h"
#include "UserScene.h"
#include "RootScene.h"
#include "StrokeIntersector.h"

class EventHandler : public osgGA::GUIEventHandler {
public:
    EventHandler(RootScene* scene, dureu::MOUSE_MODE mode = dureu::MOUSE_SELECT);

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void setMode(dureu::MOUSE_MODE mode);
    dureu::MOUSE_MODE getMode() const;

    void doEraseStroke(entity::Stroke *stroke, int first, int last, dureu::EVENT event = dureu::EVENT_DRAGGED);

    template <typename T1, typename T2>
    void doSelectStroke(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doSketch(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doDeleteEntity(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    template <typename T1, typename T2>
    void doDeleteCanvas(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doEditCanvasOffset(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditCanvasRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doCanvasClone(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    template <typename T1, typename T2>
    void doEditPhotoMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void doEditStrokesMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditStrokesScale(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void doEditStrokesRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    template <typename T1, typename T2>
    void doEditPhotoFlip(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, bool horizontal);

    template <typename T1, typename T2>
    void doEditPhotoScale(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    template <typename T1, typename T2>
    void doEditPhotoRotate(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
    entity::Stroke* getStroke(const StrokeIntersector::Intersection& result);
    entity::Canvas* getCanvas(const osgUtil::LineSegmentIntersector::Intersection& result);
    entity::Photo* getPhoto(const osgUtil::LineSegmentIntersector::Intersection& result);

    template <typename T1, typename T2>
    bool getLineIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, T1& result);

    bool getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                 double& u, double& v);
    bool getRaytraceNormalProjection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                     osg::Vec3f &XC);
    bool getRaytracePlaneIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                      const osg::Vec3f& axis,
                                      osg::Vec3f& P);

    void finishAll();

    dureu::MOUSE_MODE m_mode;
    osg::observer_ptr<RootScene> m_scene;
    osg::observer_ptr<entity::Photo> m_photo;
};

#endif // EVENTHANDLER

