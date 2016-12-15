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

private:
    osg::ref_ptr<osg::Switch>               m_switch;
};
} // namespace entity

#endif // SVMDATA_H
