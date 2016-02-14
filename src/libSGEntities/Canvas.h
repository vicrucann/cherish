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

#include "Settings.h"
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

    /* ctor to be used only within serializer, do not use it anywhere else */
    Canvas(const Canvas& cnv, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    virtual void initializeSG();

    META_Node(entity, Canvas)

    void setMatrixRotation(const osg::Matrix& R);
    const osg::Matrix& getMatrixRotation() const;

    void setMatrixTranslation(const osg::Matrix& T);
    const osg::Matrix& getMatrixTranslation() const;

    void setTransform(osg::MatrixTransform* t);
    const osg::MatrixTransform* getTransform() const;
    osg::MatrixTransform* getTransform();

    void setSwitch(osg::Switch* sw);
    const osg::Switch* getSwitch() const;

    void setFrame(osg::Geometry* geom);
    const osg::Geometry* getFrame() const;

    void setPickable(osg::Geometry* geom);
    const osg::Geometry* getPickable() const;

    void setAxis(osg::Geometry* geom);
    const osg::Geometry* getAxis() const;

    void setNorm(osg::Geometry* n);
    const osg::Geometry* getNorm() const;

    void setIntersection(osg::Geometry* i);
    const osg::Geometry* getIntersection() const;

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

    void translate(const osg::Matrix& mt);
    void rotate(const osg::Matrix& mr);

    void unselectAll();
    void unselectStrokes();
    void selectAllStrokes();

    void setStrokeCurrent(entity::Stroke* stroke);
    void setStrokeCurrent(bool current);
    entity::Stroke* getStrokeCurrent() const;

    void addStrokesSelectedAll();
    void addStrokesSelected(entity::Stroke* stroke);
    void resetStrokesSelected();
    void resetStrokeSelected(entity::Stroke* stroke);
    const std::vector< entity::Stroke* >& getStrokesSelected() const;
    int getStrokesSelectedSize() const;
    osg::Vec3f getStrokesSelectedCenter() const;
    void moveStrokes(std::vector<Stroke *> &strokes, double du, double dv);
    void moveStrokesSelected(double du, double dv);
    void scaleStrokes(std::vector<Stroke *> &strokes, double s, osg::Vec3f center);
    void scaleStrokesSelected(double s, osg::Vec3f center);
    void rotateStrokes(std::vector<entity::Stroke*> strokes, double theta, osg::Vec3f center);
    void rotateStrokesSelected(double theta, osg::Vec3f center);

    bool setPhotoCurrent(entity::Photo* photo);
    void setPhotoCurrent(bool current);
    entity::Photo* getPhotoCurrent() const;

    // recalculate frame's geometry based on strokes
    void updateFrame();
    //void updateData(); // update centroid as well, degenerated for the moment

    void setModeEdit(bool on); // changes certain colors, shows or hides normal
    bool getModeEdit() const;

    osg::Plane getPlane() const;
    osg::MatrixTransform* getMatrixTransform() const;
    entity::Canvas* clone() const;

protected:
    ~Canvas();

    void setStrokeSelected(entity::Stroke* stroke);
    void setStrokeSelected(bool selected);
    bool isStrokeSelected(entity::Stroke* stroke) const;
    entity::Stroke* getStrokeSelected() const;

    void updateTransforms();
    void setVertices(const osg::Vec3f& center, float szX, float szY, float szCr, float szAx);
private:
    osg::Matrix m_mR;
    osg::Matrix m_mT;
    osg::ref_ptr<osg::MatrixTransform> m_transform; // matrix transform in 3D space
    osg::ref_ptr<osg::Switch> m_switch; // inisible or not, the whole canvas
    osg::ref_ptr<osg::Geode> m_geodeData; // keeps user canvas drawables such as strokes
    osg::ref_ptr<osg::Geometry> m_frame; // frame drawables
    osg::ref_ptr<osg::Geometry> m_pickable; // to select canvas by mouse
    osg::ref_ptr<osg::Geometry> m_axis; // local coordinate axis
    osg::ref_ptr<osg::Geometry> m_norm;
    osg::ref_ptr<osg::Geometry> m_intersection;

    osg::observer_ptr<entity::Stroke> m_strokeCurrent; /* for stroke drawing */
    std::vector<entity::Stroke*> m_strokesSelected;
    osg::observer_ptr<entity::Stroke> m_strokeSelected; /* for stroke editing, e.g., push, delete, copy, cut */
    osg::observer_ptr<entity::Photo> m_photoCurrent; /* for editing photo position and properties*/

    osg::Vec3f m_center;
    osg::Vec3f m_normal;
    osg::Vec4f m_color; // display color for canvas drawables

    bool m_edit;

};
}

#endif // CANVAS

