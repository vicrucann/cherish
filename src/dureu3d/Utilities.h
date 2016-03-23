#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>

#include <QColor>

#include <osgGA/GUIEventAdapter>
#include <osgViewer/View>
#include <osg/Camera>
#include <osg/Plane>

#include "Canvas.h"
#include "Stroke.h"

class Utilities
{
public:
    static QColor getQColor(const osg::Vec4f& color);

    static bool areStrokesProjectable(const std::vector<entity::Stroke *> &strokes, entity::Canvas *source, entity::Canvas *target, osg::Camera* camera);

    static bool getViewProjectionWorld(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                       osg::Matrix& VPW, osg::Matrix& invVPW);

    static void getFarNear(double x, double y, const osg::Matrix& invVPW,
                           osg::Vec3f& near, osg::Vec3f& far);

    static bool getRayPlaneIntersection(const osg::Plane& plane, const osg::Vec3f& center,
                                        const osg::Vec3f& nearPoint, const osg::Vec3f& farPoint, osg::Vec3f& P);

    static bool getModel(entity::Canvas* canvas, osg::Matrix& M, osg::Matrix& invM);

    static bool getLocalFromGlobal(const osg::Vec3f& P,const osg::Matrix& invM, osg::Vec3f& p);

    static bool getGlobalFromLocal(const osg::Vec3f& p, const osg::Matrix& M, osg::Vec3f& P);

    /* algorithm for distance between skew lines:
     * http://www2.washjeff.edu/users/mwoltermann/Dorrie/69.pdf
     * For two points P1 and P2 on skew lines;
     * and d - the direction vector from P1 to P2;
     * u1 and u2 - unit direction vectors for the lines
     */
    static bool getSkewLinesProjection(const osg::Vec3f& center,
                                       const osg::Vec3f& farPoint, const osg::Vec3f& nearPoint,
                                       const osg::Vec3f& normal, osg::Vec3f& X1);


    static double getSkewLinesDistance(const osg::Vec3d &r1, const osg::Vec3d &r2,
                                       const osg::Vec3d &v1, const osg::Vec3d &v2);


    /* intersection between two canvases that is projected onto frame on canvas2 */
    static int getCanvasesIntersection(entity::Canvas* current, entity::Canvas* previous,
                                       osg::Vec3f& p1, osg::Vec3f& p2, osg::Vec3f& p3, osg::Vec3f& p4);

    /* for more detailes, see
     * http://geomalgorithms.com/a05-_intersect-1.html */
    static int getPlanesIntersection(entity::Canvas* canvas1, entity::Canvas* canvas2, osg::Vec3f& iP, osg::Vec3f& u);

    static osg::Vec3f projectPointOnLine(const osg::Vec3f& iP, const osg::Vec3f& u, const osg::Vec3f& P);
};

#endif // UTILITIES_H
