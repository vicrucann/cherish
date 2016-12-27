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

    static entity::SVMData* getParentSVM(entity::DraggableWire* wire);

private:
    osg::ref_ptr<osg::Switch>   m_switch;
    entity::DraggableWire*      m_wire1;
    entity::DraggableWire*      m_wire2;
};
} // namespace entity

#endif // SVMDATA_H
