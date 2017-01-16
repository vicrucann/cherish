#ifndef SVMDATA_H
#define SVMDATA_H

#include <osg/Switch>
#include <osg/MatrixTransform>

#include "ProtectedGroup.h"
#include "DraggableWire.h"

/*! \class SVMData
 * \brief A class to manage geometries that helps to obtain camera position through single view metrology method.
 * For more details on SVM, refer to "Single View Metrology" by Criminisi et. al., 2000.
*/

// forware declaration
namespace entity {
class DraggableWire;
}

namespace entity {
class SVMData : public osg::ProtectedGroup
{
public:
    /*! Default constructor. Creates an empty instance without initializing the structure.*/
    SVMData();

    /*! Method to set up world to local matrix transform for a "Wall" wire. */
    void setTransformWall(osg::Matrix m);

    /*! Method to set up world to local matrix transform for a "Floor" wire. */
    void setTransformFloor(osg::Matrix m);

    /*! Method to set up visibility of geometries. */
    void setVisibility(bool visibility);

    /*! \return whether the wire geometries are visible (true) or not (false). */
    bool getVisibility() const;

    /*! \return i-th point local coordinates [u, v, 0] of the first wire (wall). */
    osg::Vec3f getLocalWall(int i) const;

    /*! \return i-th point global coordinates [X, Y, Z] of the second wire (floor). */
    osg::Vec3f getGlobalFloor(int i) const;

    /*! \return pointer on parent SVMData structure of the given wire. */
    static entity::SVMData* getParentSVM(entity::DraggableWire* wire);

    /*! \return wire associated with Floor canvas. */
    entity::DraggableWire* getWallWire() const;

    /*! \return wire associated with Wall canvas. */
    entity::DraggableWire* getFlootWire() const;

private:
    osg::ref_ptr<osg::Switch>   m_switch; /*!< Elements visibilities */
    entity::DraggableWire*      m_wire1; /*!< Wall wire */
    entity::DraggableWire*      m_wire2; /*!< Floot wire */
};
} // namespace entity

#endif // SVMDATA_H
