#ifndef HOMOGRAPHYMATRIX_H
#define HOMOGRAPHYMATRIX_H

/*! \class HomographyMatrix
 * \brief Extracts the homography matrix given image-to-world correspondences.
*/
class HomographyMatrix
{
public:
    static bool solve();
};

#endif // HOMOGRAPHYMATRIX_H
