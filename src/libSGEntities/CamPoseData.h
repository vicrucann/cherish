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
class CamPoseData
{
public:
    /*! Default constructor. Creates an empty instance without initializing the structure.*/
    CamPoseData();

private:
    osg::ref_ptr<osg::Switch>   m_switch; /*!< Child scene graph visibility. */
    entity::EditableWire*       m_wire;  /*!< Camera pose wire geometries. */
    osg::Camera*                m_camera; /*!< So that frames are always rendered on top of photos. */

}; // class CamPoseData
} // namespace entity

#endif // CAMPOSEDATA_H
