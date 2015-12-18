#ifndef CANVAS
#define CANVAS

/* The canvas is defined by its center location -R3
 * and its normal -R3. The default size values are provided
 * when construction the Canvas: point pA and pB -R3.
 * Those points are treated as two consequtive points in a
 * rectangle representing the outer border of the canvas.
 * When more drawables are introduced by user, e.g., strokes,
 * the canvas borders (kept at variable verticies) are redifined
 * using osg::BoundingBox().
 *
 * TODO:
 * For current canvas only: intersections (projection) line with
 * previous canvas.
 * Boundary margin: when initialize, make sure the boundaries do
 * not overlap.
 * Constructor: center, normal, color (set the boundaries based on
 * default minimum values for size)
 * Constructor: center, color, and to which plane is parallel (x=1,y=2,z=3)
 *
 * Canvas representation (ideas):
 * Given plane parameters and canvas centroid
 * Extract normal
 * Extract local x' and y' coordinate vectors (make sure they are axes aligned)
 * The point within the canvas is defined by a local 2d coordinate p' = (a,b)
 * The global 3d coordinate of such point is calculated: p = a*x' + b*y'
 *
 * Therefore, it enough to represent a canvas by a normal and a centroid,
 * or plane equation Ax+By+Cz+D=0 and centroid 3D coordinate.
 *
 * When the canvas is transformed by rotation or offset, we only need to
 * apply the TransformMatrix to the canvas internal parameters:
 * - either centroid and normal
 * - or plane and centroid
 * Then update info on 3d coordinates based on the newly obtained local coordinate
 * system x' and y'.
 *
 * Canvas has the next branch structure:
 * Canvas -> Switch -> Transform -> Geometry -> Canvas drawables
 *                                           |-> User drawables (strokes)
 */

#include "settings.h"
#include "stroke.h"
#include "photo.h"

#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/Group>
#include <osg/BoundingBox>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Switch>

class Canvas : public osg::Group {
public:
    Canvas(osg::MatrixTransform *transform, const std::string& name);
    ~Canvas(){}

    void setColor(osg::Vec4f color);
    osg::Vec4f getColor() const;

    void setVisibility(bool vis);
    bool getVisibility() const;
    void setVisibilityLocalAxis(bool vis);
    bool getVisibilityLocalAxis() const;

    void setTransformPost(osg::MatrixTransform* t);
    void setTransformPost(const osg::Matrix& m);
    void setTransformPre(osg::MatrixTransform* r);
    osg::MatrixTransform* getTransform() const;

    void setCanvasName(const std::string& name);
    void setSwitchName(const std::string& parentName);
    void setTransformName(const std::string& parentName);
    void setSwitchFrameName(const std::string& parentName);
    void setGeodeDataName(const std::string& parentName);
    void setPhotoNames(const std::string& parentName);

    std::string getSwitchName() const;
    std::string getTransformName() const;
    std::string getSwitchFrameName() const;
    std::string getGeodeDataName() const;

    void addStroke(const double u, const double v, int mouse);
    void addPhoto(Photo* photo, const double u = 0, const double v = 0);
    void movePhoto(Photo* photo, const double u, const double v, int mouse);
    void setPhotoCurrent(Photo* photo);
    void setPhotoCurrent(bool current);

    // recalculate frame's geometry based on strokes
    // update centroid as well
    void updateFrame();

    void setModeOffset(bool on); // changes certain colors, shows or hides normal

    osg::Vec3f getCenter() const;
    osg::Plane getPlane() const;
    osg::Vec3f getNormal() const;

    Stroke* getStrokeCurrent() const;
    void finishStrokeCurrent();

    Photo* getPhotoCurrent() const;

protected:
    void transformData(const osg::Matrix& matrix);
    void setVertices(const osg::Vec3f& center, float szX, float szY, float szCr, float szAx);
private:
    osg::ref_ptr<osg::Switch> _switch; // inisible or not, the whole canvas
    osg::ref_ptr<osg::MatrixTransform> _transform; // matrix transform in 3D space
    osg::ref_ptr<osg::Switch> _switchFrame; // frame, pickable, axis
    osg::ref_ptr<osg::MatrixTransform> _transformData; // matrix transform for user data on 2D plane
    osg::ref_ptr<osg::Geode> _geodeFrame; // frame and pickable
    osg::ref_ptr<osg::Geode> _geodeAxis;
    osg::ref_ptr<osg::Geometry> _frame; // frame drawables
    osg::ref_ptr<osg::Geometry> _pickable; // to select canvas by mouse
    osg::ref_ptr<osg::Geometry> _axis; // local coordinate axis
    osg::ref_ptr<osg::Geode> _geodeData; // keeps user canvas drawables such as strokes

    osg::ref_ptr<osg::Switch> _mSwitchNormal; // normal's data
    osg::ref_ptr<osg::Geode> _mGeodeNormal;
    osg::ref_ptr<osg::Geometry> _mGeometryNormal; // the length is always 0.25*max(framewidth, frameheight)

    osg::Vec3Array* _mVerticesFrame;
    osg::Vec3Array* _mVerticesPickable;
    osg::Vec3Array* _mVerticesAxis;
    osg::Vec3Array* _mVerticesNormal;

    osg::observer_ptr<Stroke> _strokeCurrent;
    osg::observer_ptr<Photo> _photoCurrent;

    osg::Vec3f _center; // centrod of the canvas
    osg::Vec3f _normal;

    osg::Vec4f _color; // display color for canvas drawables

    int _idPhoto; // so that to give unique names to photo entities

};

#endif // CANVAS

