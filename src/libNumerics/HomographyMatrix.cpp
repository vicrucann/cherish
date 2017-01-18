#include "HomographyMatrix.h"

#include "QtGlobal"
#include "QDebug"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif // GNUC
#include <Eigen/Jacobi>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // GNUC

Eigen::Matrix3d HomographyMatrix::solve(entity::SVMData *svm)
{
    if (!svm) qCritical("The provided SVMData structure is NULL");

    const int n = 4;
    Eigen::MatrixXd A(n*2, 9);

    // XY=x1, uv=x2
    for (int i = 0; i < n; ++i) {
            int j = 2*i;
            osg::Vec3f P = svm->getGlobalFloor(i);
            osg::Vec3f p = svm->getLocalWall(i);
            A(j,0) = P.x(); // x1_(0, i);
            A(j,1) = P.y(); // x1_(1, i);
            A(j,2) = 1.0;
            A(j,6) = -p.x() * P.x(); //-x2_(0, i) * x1_(0, i);
            A(j,7) = -p.x() * P.y(); //-x2_(0, i) * x1_(1, i);
            A(j,8) = -p.x(); // -x2_(0, i);

            ++j;
            A(j,3) = P.x(); //x1_(0, i);
            A(j,4) = P.y(); //x1_(1, i);
            A(j,5) = 1.0;
            A(j,6) = -p.y() * P.x(); //-x2_(1, i) * x1_(0, i);
            A(j,7) = -p.y() * P.y(); //-x2_(1, i) * x1_(1, i);
            A(j,8) = -p.y(); //-x2_(1, i);
    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullV); // or U?
    Eigen::MatrixXd V((int)svd.matrixV().rows(), (int)svd.matrixV().cols());
    V = svd.matrixV();
    qDebug() << "V dims=[" << V.rows() << "," << V.cols() << "]";
//    Q_ASSERT(V.rows() == 9 && V.cols() == 1);

    Eigen::Map<Eigen::Matrix3d, Eigen::Aligned> H(V.data(), 3, 3);

    return  H;
}
