#ifndef HOMOGRAPHYMATRIX_H
#define HOMOGRAPHYMATRIX_H

#include "osg/Matrix"
#include "vector"

/*! \class HomographyMatrix
 * \brief Extracts the homography matrix given image-to-world correspondences.
*/
class HomographyMatrix
{
public:

    static std::vector<double> solve();
};

#endif // HOMOGRAPHYMATRIX_H
