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
//    Canvas();
//    Canvas(osg::Vec3f center, osg::Vec3f pA, osg::Vec3f pB, osg::Vec4f color);
    Canvas(osg::MatrixTransform *transform);
    ~Canvas(){}

    void setColor(osg::Vec4f color);

private:
    void addCanvasDrawables();
    osg::Vec3f _center;
    osg::Vec3f _normal;
    osg::Vec4f _color;
    osg::Vec3Array* _vertices;
    osg::ref_ptr<osg::Geode> _geode;
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::MatrixTransform> _transform;
    osg::ref_ptr<osg::Switch> _switch;
};

#endif // CANVAS

