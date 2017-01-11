#ifndef HOMOGRAPHYMATRIX_H
#define HOMOGRAPHYMATRIX_H

#include "vector"
#include "Eigen/Dense"
#include "SVMData.h"

/*! \class HomographyMatrix
 * \brief Extracts the homography matrix given image-to-world correspondences.
*/
class HomographyMatrix
{
public:

    static Eigen::Matrix3d solve(entity::SVMData* svm);
};

#endif // HOMOGRAPHYMATRIX_H
