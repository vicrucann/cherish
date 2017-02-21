#ifndef CAMPOSEDATA_H
#define CAMPOSEDATA_H

#include <osg/Switch>
#include <osg/MatrixTransform>
#include <osg/Camera>

#include "ProtectedGroup.h"
#include "EditableWire.h"

// forware declaration
namespace entity {
class EditableWire;
}

/*! \class CamPoseData
 * \brief A class to manage geometries that helps to obtain camera position through user's manual location.
*/

namespace entity {
class CamPoseData : public osg::ProtectedGroup
{
public:
    /*! Default constructor. Creates an empty instance without initializing the structure.*/
    CamPoseData(double fov);

    /*! A method  to set up world to local matrix transform for the wire. */
    void setTransform(osg::Matrix m);

    /*! Method to set up visibility of geometries. */
    void setVisibility(bool visibility);

    /*! \return whether the wire geometries are visible (true) or not (false). */
    bool getVisibility() const;

    /*! \return pointer on parent CamPoseData structure of the given wire. */
    static entity::CamPoseData* getParentCamPose(entity::EditableWire* wire);

    /*! \return The wire geometries. */
    entity::EditableWire* getWire() const;

private:
    osg::ref_ptr<osg::Switch>   m_switch; /*!< Child scene graph visibility. */
    entity::EditableWire*       m_wire;  /*!< Camera pose wire geometries. */
    osg::Camera*                m_camera; /*!< So that frames are always rendered on top of photos. */

}; // class CamPoseData
} // namespace entity

#endif // CAMPOSEDATA_H
