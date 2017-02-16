#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>

#include <QColor>
#include <QCursor>

#include <osgGA/GUIEventAdapter>
#include <osgViewer/View>
#include <osg/Camera>
#include <osg/Plane>
#include <osg/Matrixd>

#include "Canvas.h"
#include "Stroke.h"
#include "SVMData.h"
#include "DraggableWire.h"

/*! \class Utilities
 * \brief Some generic methods compiled into one class.
 * Methods that describe:
 * * The relationships between screen coordinates and world coordinates.
 * * The relationships between local system of coordinates and global system of coordinates.
 * * Color convertions between Qt and OSG libraries.
 * * Geometric derivations such as distance between skew lines.
*/
class Utilities
{
public:
    /*! A method to convert an OSG color representation into a Qt
     * \sa getQColor(). */
    static QColor getQColor(const osg::Vec4f& color);

    /*! A method to convert a Qt color representation into an OSG.
     * \sa getQColor(). */
    static osg::Vec4f getOsgColor(const QColor& color);

    /*! A tester method in order to check if a set of strokes can be projected into a selected canvas under the given view point.
     * \param strokes is the set of selected strokes
     * \param source is the Canvas which contains the set of strokes to project
     * \param target is the Canvas on which the projection should be done
     * \param camera is the camera view
     * \retun true if all the strokes can be projected successfully. */
    static bool areStrokesProjectable(const std::vector<entity::Stroke *> &strokes, entity::Canvas *source, entity::Canvas *target, osg::Camera* camera);

    /*! A method to obtain two matrices - VPW and its invertse given the action adapter.
     * \param aa is OSG-based variable from which camera, view and other matrices can be derived.
     * \param VPW is the output View-Projection-World matrix
     * \param invVPW is the ourput inverse of View-Projection-World matrix.
     * \return true if both matrices were obtained without an error. */
    static bool getViewProjectionWorld(osgGA::GUIActionAdapter& aa,
                                       osg::Matrix& VPW, osg::Matrix& invVPW);

    /*! A method to obtain far and near points of the screen point that cast a projection ray.
     * \param x - x-coordinate of the screen
     * \param y - y-coordinate of the screen
     * \param invVPW - inverse matrix of View-Projection-World matrix
     * \param near is the result 3D point on the near point on the cast ray
     * \param far is the result 3D point as the far point on the cast ray.
     * \return true if the points were obtained successfully.
     * \sa getViewProjectionWorld(). */
    static void getFarNear(double x, double y, const osg::Matrix& invVPW,
                           osg::Vec3f& near, osg::Vec3f& far);

    /*! A method to obtain intersection point between a given plane and a line (ray).
     * \param plane is the given plane
     * \param center is an arbitrary point on the plane that represents the plane's center.
     * \param nearPoint is the 3D near point of the ray that was cast from a screen point
     * \param farPoint is the 3D far point of the ray that was cast from a screen point
     * \param P is the result point that represents intersection of the plane and a ray that is given by far and near points
     * \return true if the point was obtained without any errors
     * \sa getFarNear() */
    static bool getRayPlaneIntersection(const osg::Plane& plane, const osg::Vec3f& center,
                                        const osg::Vec3f& nearPoint, const osg::Vec3f& farPoint, osg::Vec3f& P);

    /*! A method to obtain model matrix (global to local conversion matrix) from a given canvas.
     * \param canvas is the 2D plane canvas from which to derive the model matrix
     * \param M is the result model matrix
     * \param invM is the result inverse of model matrix
     * \return true if model matrices were obtaibed successfully. */
    static bool getModel(entity::Canvas* canvas, osg::Matrix& M, osg::Matrix& invM);

    /*! A method to convert a global point into local system of coordinates.
     * \param P is the requested point to convert
     * \param invM is an inverted model matrix
     * \param p is the result 2D point (local system of coordinates).
     * \return true is derivation passed successfully.
     * \sa getModel(), getLocalFromGlobal() */
    static bool getLocalFromGlobal(const osg::Vec3f& P,const osg::Matrix& invM, osg::Vec3f& p);

    /*! A method which is inverse to the method getLocalFromGlobal().
     * \sa getModel(), getLocalFromGlobal(). */
    static bool getGlobalFromLocal(const osg::Vec3f& p, const osg::Matrix& M, osg::Vec3f& P);

    /* algorithm for distance between skew lines:
     *
     * For two points P1 and P2 on skew lines;
     * and d - the direction vector from P1 to P2;
     * u1 and u2 - unit direction vectors for the lines
     */
    /*! A method to obtain a distance between skew lines. For more info, see: http://www2.washjeff.edu/users/mwoltermann/Dorrie/69.pdf
     * or here: http://paulbourke.net/geometry/pointlineplane/
     * \param center is the first point P1 (see notations in the link above)
     * \param farPoint is to describe the line on which P1 lies
     * \param nearPoint is the second point P2
     * \param normal is to describe the line on which P2 lies
     * \param X1 is the projection point on first line onto second line
     * \return true if point was obtained successfully.
     * \sa getSkewLinesDistance() */
    static bool getSkewLinesProjection(const osg::Vec3f& center,
                                       const osg::Vec3f& farPoint, const osg::Vec3f& nearPoint,
                                       const osg::Vec3f& normal, osg::Vec3f& X1);


    /*! A method to obtain a distance between skew lines.
     * \sa getSkewLinesProjection() */
    static double getSkewLinesDistance(const osg::Vec3d &r1, const osg::Vec3d &r2,
                                       const osg::Vec3d &v1, const osg::Vec3d &v2);

    /*! A method to obtain an intersection between the two lines. Each line is presented by two points in
     * 3D space. The algorithm treats the lines as skew and the intersection is calculated as average between
     * two projections on each line. The projection between the skew lines is a shortest distance between the
     * lines.
     * \param La1 is the first 3D point on first line.
     * \param La2 is the second 3D point on first line.
     * \param Lb1 is the first 3D point on second line.
     * \param Lb2 is the second 3D point on second line.
     * \param intersection is the result intersection.
     * \return true if the intersection calculation was successful.
*/
    static bool getLinesIntersection(const osg::Vec3f& La1, const osg::Vec3f& La2, const osg::Vec3f& Lb1, const osg::Vec3f& Lb2,
                                     osg::Vec3f& intersection);


    /*! A method to obtain intersection between two canvases (planes) so that to do projection on the intersection onto the second canvas.
     * \param current is the first canvas
     * \param previous is the second canvas (where the intersection will be cast on)
     * \param p1, \param p2, \param p3, \param p4 are the result intersection points that represent two line segments.
     * \return 2 if the intersection is found.
     * \sa getPlanesIntersection() */
    static int getCanvasesIntersection(entity::Canvas* current, entity::Canvas* previous,
                                       osg::Vec3f& p1, osg::Vec3f& p2, osg::Vec3f& p3, osg::Vec3f& p4);

    /*! A method to obtaib intersection between two planes. For more details on the algorithm, see: http://geomalgorithms.com/a05-_intersect-1.html */
    static int getPlanesIntersection(entity::Canvas* canvas1, entity::Canvas* canvas2, osg::Vec3f& iP, osg::Vec3f& u);

    /*! A method to project 3d point onto 3D line. */
    static osg::Vec3f projectPointOnLine(const osg::Vec3f& iP, const osg::Vec3f& u, const osg::Vec3f& P);

    /*! UI method to obtain cursor data based on the given mouse mode. */
    static QCursor getCursorFromMode(cher::MOUSE_MODE mode);

    /*! A method to obtain camera pose given 4 intersecting rays (obtained from entity::SVMData).
     * The algorithm is as follows: first, find 4 intersections between each neighboring rays (or use all possible pair?).
     * Second, calculate the camera eye - average of the calculated four intersections. The camera's center vector
     * passes through one of the wire's centers, and the up vector is always constant - (0,1,0).
     * \return true if the parameters were calculated successfully, false otherwise. */
    static bool getCameraPosition(entity::SVMData* svm,  osg::Vec3f& eye, osg::Vec3f& center, osg::Vec3f& up);

    /*! A method that uses OpenCV's implementation of SolvePnP, i.e., provided four matches between the
     * object points (3D) and image points (2d) - both extracted from entity::SVMData, it finds the projection
     * matrix (rotation and translation) that does the projection from the object points to homogenious image points.
     * This method is used when we want to update camera position when user is finished entity::SVMData manipulation.
     * \param svm is the input entity::SVMData parameter
     * \param projection is the output projection matrix [R|t], already converted to global coordinates ( for more
     * info see : http://stackoverflow.com/questions/18637494/camera-position-in-world-coordinate-from-cvsolvepnp#18643735 )
     * \return true if projection matrix was extracted successfully, false otherwise. */
//    static bool getProjectionMatrix(entity::SVMData* svm, osg::Matrixd &projection);
};

#endif // UTILITIES_H
