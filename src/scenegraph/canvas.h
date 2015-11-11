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
 */

#include "settings.h"

#include <osg/Geode>
#include <osg/BoundingBox>

class Canvas : public osg::Geode {
public:
    Canvas(osg::Vec3f center, osg::Vec3f pA, osg::Vec3f pB, osg::Vec4f color);
    ~Canvas(){}

private:
    void addCanvasDrawables();
    osg::Vec3f _center;
    osg::Vec3f _normal;
    osg::Vec4f _color;
    osg::Vec3Array* _vertices;
};

#endif // CANVAS

