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

    void setTransform(osg::MatrixTransform* transform);
    osg::MatrixTransform* getTransform() const;

    void setSwitchName(const std::string& parentName);
    void setTransformName(const std::string& parentName);
    void setGeometryName(const std::string& parentName);
    void setGeodeName(const std::string& parentName);

    std::string getSwitchName() const;
    std::string getTransformName() const;
    std::string getGeometryName() const;
    std::string getGeodeName() const;
protected:
private:
    osg::ref_ptr<osg::Geometry> _frame; // frame drawables
    osg::ref_ptr<osg::Geometry> _pickable; // to select canvas by mouse
    osg::ref_ptr<osg::Geometry> _axis; // local coordinate axis
    osg::ref_ptr<osg::Switch> _swFrame; // frame and pickable visibility
    osg::ref_ptr<osg::Switch> _swAxis; // axis visibility

    osg::ref_ptr<osg::Geode> _geode; // keeps all canvas drawables: frame and user data
    osg::ref_ptr<osg::MatrixTransform> _transform; // matrix transform in 3D space
    osg::ref_ptr<osg::MatrixTransform> _transformUD; // matrix transform for user data on 2D plane
    osg::ref_ptr<osg::Switch> _switch; // inisible or not, the whole canvas data

    osg::Vec3f _center; // centrod of the canvas
    osg::Vec3f _normal; // normal vector to define plane
    osg::Plane _plane; // only to use for math and keep plane params
    osg::Vec3f _x, _y; // plane's 2D local coordinate system, cross(x,y)=n

    osg::Vec4f _color; // display color for canvas drawables
};

#endif // CANVAS

