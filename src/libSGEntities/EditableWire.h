#ifndef EDITABLEWIRE_H
#define EDITABLEWIRE_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Switch>
#include <osg/StateSet>
#include <osg/Point>
#include <osg/LineWidth>
#include <osg/MatrixTransform>

namespace entity {

/*! \class EditableWire
 * \brief A scene graph class that contains all the geometric entities for camera pose drawing.
 *
 * The EditableWire has a shape of triangle and it indicates a camera pose:
 *
 *    (center)
 *     |
 *  ---|---  (focal)
 *  |  |  |
 *   \ | /
 *    \ /
 *     o  (eye)
*/

class EditableWire : public osg::MatrixTransform
{
public:
    /*! Constructor to initialize the scene graph and to set default colors.
     * \param fov2 is the current FOV within Cherish environment (OpenGL parameter that
     * can be extracted from GLWidget). */
    EditableWire(double fov2);

    /*! \return global eye of the camera pose wire. */
    osg::Vec3f getEye3D() const;

    /*! \return local eye position of the camera pose wire. */
    osg::Vec3f getEye2D() const;

    /*! \return global center of the camera  pose wire. */
    osg::Vec3f getCenter3D() const;

    /*! \return 2D right direction vector */
    osg::Vec3f getRight2D() const;

    /*! \return 3D right vector */
    osg::Vec3f getRight3D() const;

    /*! \return global up vector. */
    osg::Vec3f getUp() const;

    /*! A method to obtain local coordinates of the center points.
     * \param p1 is the input parameter for eye location,
     * \param p2 is the input parameter for center location. */
    void getCenter2D(osg::Vec2f& p1, osg::Vec2f& p2) const;

    /*! \return const pointer on geode node which contains all the drawables (i.e. return geode parent). */
    const osg::Geode* getGeode() const;

    /*! A method to edit the position of the camera eye (point of the triangular wire). */
    void editEye(double u, double v);

    /*! A method to edit the center position of the camera pose, i.e., the camera look direction.
     * \param theta is the rotation angle from the current position. */
    void editCenter(double theta);

    /*! A method to edit focal distance of the camera pose.
     * \param distance is the new distance from camera eye. Note, the distance can never be smaller
     * than certain threshold, e.g., 0.2. */
    void editFocal(double angle);

    /*! A method to set up defult colors for all the geometries. */
    void unselect();

    /*! A method to set up selected color for a geometry.
     * \param index is the given geometry index, e.g., 1 for eye, 2 for center, and 3 for focal geometry. */
    void pick(int index);

    /*! A method to set up the selected geometry to unselected default color. */
    void unpick();

protected:
    /*! Set a given color to the whole focal geometry. \param color is the color input. */
    void setColorFocal(const osg::Vec4f& color);

    /*! A method to set up default colors to eye geometry. */
    void setColorEyeDefaults();

    /*! A method to set up default colors to center geometry. */
    void setColorCenterDefaults();

    /*! A method to update geometries after new colors are assigned. */
    void updateGeometry(osg::Geometry* geom);

    /*! A method to move all the geometries to another place. Normally called from editEye().
     * \param u is the new u local coordinate
     * \param v is the new v local coordinate */
    void move(double u, double v);

    /*! A method to rotate all the geometries on a given angle. Normally called from editCenter().
     * \param theta is the rotation angle. The rotation is performed around the camera's eye position.  */
    void rotate(double theta);

    /*! A method to translate the focal geometry along the center line of camera pose wire.
     * \param d is the new distance between the camera eye and the new position of the focal geometry. The method
     * also updates the new length of the focal geometry since it is based on the distance from an eye. */
    void translate(double d);

    void expand(double a);

private:
    osg::Geode* m_geode;
    osg::Geometry* m_eye; /*!< geometry responsible for camera eye allocation */
    osg::Geometry* m_center; /*!< geometry responsible for camera center allocation */
    osg::Geometry* m_focal; /*!< geometry that includes all the wire and is responsible for camera focal allocation */
    double         m_fov2; /*!< FOV, full angle in degrees. */

    int m_selection; /*!< index of selected geometry: 1 - eye, 2 - center, 3 - focal */

}; // class EditableWire

} // namespace entity

#endif // EDITABLEWIRE_H
