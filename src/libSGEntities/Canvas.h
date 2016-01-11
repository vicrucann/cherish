#ifndef CANVAS
#define CANVAS

/* The canvas is defined by its center location -R3
 * and its normal -R3. The default size values are provided
 * when construction the Canvas.
 * When more drawables are introduced by user, e.g., strokes,
 * the canvas borders are redifined (expanded) using osg::BoundingBox().
 *
 * TODO:
 * For current canvas only: intersections (projection) line with
 * previous canvas.
 * Proper canvas transform when rotating (the offset is already done).
 *
 * Canvas has the next branch structure:
 * Canvas -> Switch (-> Transform) -> Geometry -> Canvas drawables
 *                                           |-> User drawables (strokes)
 */

#include "settings.h"
#include "Stroke.h"
#include "Photo.h"

#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/Group>
#include <osg/BoundingBox>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osgDB/ObjectWrapper>

namespace entity {
class Canvas : public osg::Group {
public:
    Canvas();
    Canvas(const Canvas& cnv, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    Canvas(osg::MatrixTransform *transform, const std::string& name);

    void setSwitch(osg::Switch* sw);
    const osg::Switch* getSwitch() const;

    void setSwitchFrame(osg::Switch* sw);
    const osg::Switch* getSwitchFrame() const;

    void setGeodeAxis(osg::Geode* geode);
    const osg::Geode* getGeodeAxis() const;

    void setFrame(osg::Geometry* geom);
    const osg::Geometry* getFrame() const;

    void setPickable(osg::Geometry* geom);
    const osg::Geometry* getPickable() const;

    void setAxis(osg::Geometry* geom);
    const osg::Geometry* getAxis() const;

    void setSwitchNormal(osg::Switch* sw);
    const osg::Switch* getSwitchNormal() const;

    void setGeodeData(osg::Geode* geode);
    const osg::Geode* getGeodeData() const;
    osg::Geode* getGeodeData();

    void setCenter(const osg::Vec3f& center);
    const osg::Vec3f& getCenter() const;

    void setNormal(const osg::Vec3f& normal);
    const osg::Vec3f& getNormal() const;

    void setColor(const osg::Vec4f& color);
    const osg::Vec4f& getColor() const;

    void setVisibility(bool vis);
    bool getVisibility() const;
    void setVisibilityLocalAxis(bool vis);
    bool getVisibilityLocalAxis() const;

    void setTransformPost(osg::MatrixTransform* t);
    void setTransformPost(const osg::Matrix& m);
    void setTransformPre(osg::MatrixTransform* r);
    osg::MatrixTransform* getTransform() const;

    void movePhoto(entity::Photo* photo, const double u, const double v, int mouse);
    void setPhotoCurrent(entity::Photo* photo);
    void setPhotoCurrent(bool current);

    // recalculate frame's geometry based on strokes
    void updateFrame();
    void updateData(); // update centroid as well

    void setModeOffset(bool on); // changes certain colors, shows or hides normal

    osg::Plane getPlane() const;

    entity::Stroke* getStrokeCurrent() const;
    void finishStrokeCurrent();

    entity::Photo* getPhotoCurrent() const;

protected:
    ~Canvas();
    void transformData(const osg::Matrix& matrix);
    void setVertices(const osg::Vec3f& center, float szX, float szY, float szCr, float szAx);
private:
    osg::ref_ptr<osg::Switch> m_switch; // inisible or not, the whole canvas
    osg::ref_ptr<osg::MatrixTransform> _transform; // matrix transform in 3D space
    osg::ref_ptr<osg::Switch> m_switchFrame; // frame, pickable, axis
    osg::ref_ptr<osg::Geode> m_geodeAxis;
    osg::ref_ptr<osg::Geometry> m_frame; // frame drawables
    osg::ref_ptr<osg::Geometry> m_pickable; // to select canvas by mouse
    osg::ref_ptr<osg::Geometry> m_axis; // local coordinate axis
    osg::ref_ptr<osg::Geode> m_geodeData; // keeps user canvas drawables such as strokes
    osg::ref_ptr<osg::Switch> m_switchNormal; // normal's data

    osg::Vec3Array* _mVerticesFrame;
    osg::Vec3Array* _mVerticesPickable;
    osg::Vec3Array* _mVerticesAxis;
    osg::Vec3Array* _mVerticesNormal;

    osg::observer_ptr<entity::Stroke> _strokeCurrent;
    osg::observer_ptr<entity::Photo> _photoCurrent;

    osg::Vec3f m_center;
    osg::Vec3f m_normal;
    osg::Vec4f m_color; // display color for canvas drawables

};
}

#endif // CANVAS

