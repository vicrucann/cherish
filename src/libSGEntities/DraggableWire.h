#ifndef DRAGGABLEWIRE_H
#define DRAGGABLEWIRE_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Switch>
#include <osg/StateSet>
#include <osg/Point>
#include <osg/LineWidth>
#include <osg/MatrixTransform>

#include "SVMData.h"

namespace entity {

/*! \class DraggableWire
 * \brief A scene graph class that contains all the geometric entities of entity::SVMData.
*/

class DraggableWire : public osg::MatrixTransform
{
    friend class SVMData;
public:
    /*! A constructor - initializes the scene graph and sets default wire colors */
    DraggableWire();

    /*! \return global center of the wire plane. In local coordinates, the center is always (0,0). */
    osg::Vec3f getCenter3D() const;

    /*! \return a plane that describes the position of all the points of the wire. */
    osg::Plane getPlane() const;

    /*! \return const pointer on geode node which contains all the drawables (wire and points). */
    const osg::Geode* getGeode() const;

    /*! A method to edit a location of a selected point within the plane of the wire. \param u is the new local u coordinate, \param v is the new local v coordinate. */
    void editPick(double u, double v);

    /*! A method to set up defult colors for wire and point geometries. */
    void unselect();

    /*! A method to set up selected color for the wire geometry. */
    void select();

    /*! A method to set up selected color to a specified point.
     * \param index is the point index at which the color will be assigned. */
    void pick(int index);

    /*! A method to set up selected point color to defaults. */
    void unpick();

    /*! A method to set up gragged colors to a point and adjacent edges of the wire. */
    void drag();

    /*! A method to indicate color changes from state drag to state hover over point. */
    void dragStop();

    /*! \return i-th point coordinates in local coordinate system. */
    osg::Vec3f getPoint2D(int i) const;

    /*! \return i-th point coordinates in global coordinate system. */
    osg::Vec3f getPoint3D(int i) const;

protected:
    /*! A method to set up default colors to all the points. */
    void setColorPointsDefaults();

    /*! Set a given color to the whole wire geometry. \param color is the color input. */
    void setColorWire(osg::Vec4f color);

    /*! A method to assign a color to a specific point. \param index is the point index, \param color is the input color. */
    void setColorPoint(int index, osg::Vec4f color);

    /*! A method to assign a color to a specific point and wire index. \sa setColorPoint(). */
    void setColorPointWire(int index, osg::Vec4f color);

    /*! A method to update geometries after new colors are assigned. */
    void updateGeometry(osg::Geometry* geom);

private:
    osg::Geode*         m_geode;
    osg::Geometry*      m_wire; /*!< pointer on a geometry for wireframe */
    osg::Geometry*      m_points; /*!< pointer on a geometry for points */
    int                 m_selectedPoint; /*!< index of a selected point, in a range [0,3]; if none, it is -1 */

}; // class DraggableWire
} // namespace entity


#endif // DRAGGABLEWIRE_H
