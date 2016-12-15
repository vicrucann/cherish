#ifndef CANVASNORMALPROJECTOR_H
#define CANVASNORMALPROJECTOR_H

#include <tuple>

#include "Canvas.h"
#include "VirtualPlaneIntersector.h"

class CanvasNormalProjector : public VirtualPlaneIntersector<entity::Canvas>
{
public:
    typedef std::tuple<osg::Vec3f, bool> Projection;

    CanvasNormalProjector(entity::Canvas* canvas);

    /*! Algorithm:
     * Cast the ray into 3D space
     * Make sure the ray is not parallel to the normal
     * The new offset point will be located on the projected point
     * between the ray and canvas normal.
     * Ray and normal are skew lines in 3d space, so we only need
     * to extract the projection point of the ray into the normal.
    */
    Projection getProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

protected:
    bool getProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Vec3f& XC);

    /*! Algorithm for distance between skew lines:
     *
     * For two points P1 and P2 on skew lines;
     * and d - the direction vector from P1 to P2;
     * u1 and u2 - unit direction vectors for the lines
     */
    /*! A method to obtain a distance between skew lines. For more info, see: http://www2.washjeff.edu/users/mwoltermann/Dorrie/69.pdf
     * \param center is the first point P1 (see notations in the link above)
     * \param farPoint is to descrive the line on which P1 lies
     * \param nearPoint is the second point P2
     * \param normal is to describe the line on which P2 lies
     * \param X1 is the projection point on first line onto second line
     * \return true if point was obtained successfully.
     * \sa getSkewLinesDistance() */
    static bool getSkewLinesProjection(const osg::Vec3f& center,
                                       const osg::Vec3f& farPoint, const osg::Vec3f& nearPoint,
                                       const osg::Vec3f& normal, osg::Vec3f& X1);
};

#endif // CANVASNORMALPROJECTOR_H
