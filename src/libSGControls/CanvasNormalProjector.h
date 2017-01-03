#ifndef CANVASNORMALPROJECTOR_H
#define CANVASNORMALPROJECTOR_H

#include <tuple>

#include "Canvas.h"
#include "VirtualPlaneIntersector.h"

class CanvasNormalProjector : public VirtualPlaneIntersector<entity::Canvas>
{
public:
    CanvasNormalProjector(entity::Canvas* canvas);

    /*! Algorithm:
     * Cast the ray into 3D space
     * Make sure the ray is not parallel to the normal
     * The new offset point will be located on the projected point
     * between the ray and canvas normal.
     * Ray and normal are skew lines in 3d space, so we only need
     * to extract the projection point of the ray into the normal.
    */
    Intersection3D getProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

protected:
    bool getProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Vec3f& XC);
};

#endif // CANVASNORMALPROJECTOR_H
