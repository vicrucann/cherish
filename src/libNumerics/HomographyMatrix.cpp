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

#include "libHomogrpahy/Hmatrix.h"
#include "libHomogrpahy/matrix.h"

//#include <opencv2/core.hpp>
//#include <opencv2/calib3d.hpp>
#include <vector>

osg::Matrix HomographyMatrix::solveEigen(entity::SVMData *svm)
{
    osg::Matrix H;
    if (!svm) {
        qCritical("The provided SVMData structure is NULL");
        return H;
    }

    const int n = 4;
    Eigen::MatrixXd A(n*2, 9);

    // XY=x1, uv=x2
    for (int i = 0; i < n; ++i) {
            int j = 2*i;
            osg::Vec3f P = svm->getLocalFloor(i);
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

    H(0,0) = V(0,8);
    H(0,1) = V(1,8);
    H(0,2) = V(2,8);
    H(1,0) = V(3,8);
    H(1,1) = V(4,8);
    H(1,2) = V(5,8);
    H(2,0) = V(6,8);
    H(2,1) = V(7,8);
    H(2,2) = V(8,8);

    qDebug() << "errorEigen=" << HomographyMatrix::evaluate(svm, H);

    return  H;
}

//osg::Matrix HomographyMatrix::solvePnP(entity::SVMData *svm)
//{
//    std::vector<cv::Point3d> object;
//    std::vector<cv::Point2d> image;
//    object.push_back(cv::Point3d(svm->getGlobalFloor(0).x(), svm->getGlobalFloor(0).y(), svm->getGlobalFloor(0).z()));
//    object.push_back(cv::Point3d(svm->getGlobalFloor(1).x(), svm->getGlobalFloor(1).y(), svm->getGlobalFloor(1).z()));
//    object.push_back(cv::Point3d(svm->getGlobalFloor(2).x(), svm->getGlobalFloor(2).y(), svm->getGlobalFloor(2).z()));
//    object.push_back(cv::Point3d(svm->getGlobalFloor(3).x(), svm->getGlobalFloor(3).y(), svm->getGlobalFloor(3).z()));
//    image.push_back(cv::Point2d(svm->getLocalWall(0).x(), svm->getLocalWall(0).y()));
//    image.push_back(cv::Point2d(svm->getLocalWall(1).x(), svm->getLocalWall(1).y()));
//    image.push_back(cv::Point2d(svm->getLocalWall(2).x(), svm->getLocalWall(2).y()));
//    image.push_back(cv::Point2d(svm->getLocalWall(3).x(), svm->getLocalWall(3).y()));

//    cv::Mat rvec;
//    cv::Mat tvec;
//    auto K = cv::Mat::eye(3,3, CV_32F);
//    cv::solvePnP(object, image, K, cv::noArray(), rvec, tvec);

//    cv::Mat R;
//    cv::Rodrigues(rvec, R);

//    std::vector<cv::Point2d> projected;
//    cv::projectPoints(object, rvec, tvec, K, cv::noArray(), projected);

//    for(unsigned int i = 0; i < projected.size(); ++i)
//    {
//        std::cout << "image=" << image[i] << " projected=" << projected[i] << std::endl;
//    }

//    // copy OCV data to OSG
//    std::vector<double> tvec_;
//    auto r00 = R.at<double>(0,0);
//    auto r01 = R.at<double>(0,1);
//    auto r02 = R.at<double>(0,2);
//    auto r10 = R.at<double>(1,0);
//    auto r11 = R.at<double>(1,1);
//    auto r12 = R.at<double>(1,2);
//    auto r20 = R.at<double>(2,0);
//    auto r21 = R.at<double>(2,1);
//    auto r22 = R.at<double>(2,2);
//    auto t00 = tvec.at<double>(0,0);
//    auto t10 = tvec.at<double>(1,0);
//    auto t20 = tvec.at<double>(2,0);

//    osg::Matrixd Projection
//            (r00, r01, r02, t00,
//            r10, r11, r12, t10,
//            r20, r21, r22, t20,
//            0, 0, 0, 1 );

//    return Projection;
//}

osg::Matrix HomographyMatrix::solve(entity::SVMData *svm)
{
    osg::Matrix H;
    if (!svm) {
        qCritical("The provided SVMData structure is NULL");
        return H;
    }
    const int n = 4;

    libNumerics::matrix<double> x1(2, n);
    libNumerics::matrix<double> x2(2, n);
    for (int i=0; i<n; ++i){
        osg::Vec3f p1 = svm->getLocalFloor(i);
        osg::Vec3f p2 = svm->getLocalWall(i);
        x1(0,i) = p1.x(); x1(1,i) = p1.y();
        x2(0,i) = p2.x(); x2(1,i) = p2.y();
    }

    libNumerics::matrix<double> H_ini = solveHomography(x1, 0, 0, x2, 0, 0);

    for (int i=0; i<3; ++i){
        for (int j=0; j<3; ++j){
            H(i,j) = H_ini(i,j);
        }
    }
    qDebug() << "error=" << HomographyMatrix::evaluate(svm, H);

    return H;
}

double HomographyMatrix::evaluate(entity::SVMData *svm, const osg::Matrix &H)
{
    libNumerics::matrix<double> H_ini(3,3);
    for (int i=0; i<3; ++i){
        for (int j=0; j<3; ++j){
            H_ini(i,j) = H(i,j);
        }
    }

    const int n = 4;
    double avgError = 0;
    for (int i=0; i<n; ++i){
        osg::Vec3f p1 = svm->getLocalFloor(i);
        osg::Vec3f p2 = svm->getLocalWall(i);

        libNumerics::matrix<double> cs = libNumerics::matrix<double>::zeros(3,1);
        cs(0,0) = p1.x();
        cs(1,0) = p1.y();
        cs(2,0) = 1;

        libNumerics::matrix<double> hcs = H_ini * cs;
        double px1 = hcs(0,0)/hcs(2,0), py1 = hcs(1,0)/hcs(2,0);
        double err = std::sqrt( (px1-p2.x())*(px1-p2.x()) + (py1-p2.y())*(py1-p2.y()) );
        avgError += err;
    }
    avgError /= 4;
    return avgError;
}

osg::Matrixd HomographyMatrix::getRt(const osg::Matrix &H)
{
    // [row][col]
    osg::Vec3d H1 = osg::Vec3d(H(0,0), H(0,1), H(0,2));
    osg::Vec3d H2 = osg::Vec3d(H(1,0), H(1,1), H(1,2));

    auto norm1 = H1.length();
    auto norm2 = H2.length();
    auto tnorm = (norm1 + norm2)/2.0;

    osg::Vec3d T = osg::Vec3d(H(2,0), H(2,1), H(2,2)) / tnorm;

    H1.normalize();
    H2.normalize();
    osg::Vec3d H3 = H1^H2;
    H3.normalize();

    osg::Matrixd RT(H1[0], H2[0], H3[0], T[0],
                    H1[1], H2[1], H3[1], T[1],
                    H1[2], H2[2], H3[3], T[2],
                    0.0  , 0.0  , 0.0  , 1.0 );

    return RT;
}
