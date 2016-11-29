#ifndef SVMDATA_H
#define SVMDATA_H

#include "ProtectedGroup.h"

#include <osg/Switch>
#include <osg/MatrixTransform>

/*! \class SVMData
 * \brief A class to manage geometries that helps to obtain camera position through single view metrology method.
 * For more details on SVM, refer to "Single View Metrology" by Criminisi et. al., 2000.
*/

namespace entity {
class SVMData : public osg::ProtectedGroup
{
public:
    /*! Default constructor. Creates an empty instance without initializing the structure.*/
    SVMData();

    /*! A method to pass a canvas transformation matrix to the class. The transformation matrix ,
     * once set, never changes, i.e., the rectanles remain within the same planes once they are set.
     * \param m is an entity::Canvas transform matrix. */
    void setTransform(osg::Matrix m);

private:
    osg::ref_ptr<osg::Switch>               m_switch;
    osg::ref_ptr<osg::MatrixTransform>      m_transform; // photo transform
    osg::ref_ptr<osg::Geode>                m_geode;
};
} // namespace entity

#endif // SVMDATA_H
