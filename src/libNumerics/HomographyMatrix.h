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

    static osg::Matrix solveEigen(entity::SVMData* svm);

    static void solveDecompose(entity::SVMData* svm);

    static void solvePnP(entity::SVMData* svm);

    /*! A method to extract Homogrpahy matrix from the given 4 point matches represented by
     * entity::SVMData. */
    static osg::Matrix solve(entity::SVMData* svm);

    /*! A tester method to evaluate the calcualted Homography matrix for the given entity::SVMData. */
    static double evaluate(entity::SVMData* svm, const osg::Matrix& H);

    /*! A method to obtain rotation/translation matrix from Homography matrix. */
    static osg::Matrixd getRt(const osg::Matrix& H);
};

#endif // HOMOGRAPHYMATRIX_H
