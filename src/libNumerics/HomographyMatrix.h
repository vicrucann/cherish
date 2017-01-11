#ifndef HOMOGRAPHYMATRIX_H
#define HOMOGRAPHYMATRIX_H

#include "vector"
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif // GNUC
#include "Eigen/Dense"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // GNUC

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
