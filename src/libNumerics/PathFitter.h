#ifndef PATHFITTER_H
#define PATHFITTER_H


#include <vector>
#include <osg/Array>

/*! \class PathFitter
 * \brief Fits a sequence of as few curves as possible through the path’s anchor points.
 * Based on the paper
 * "An Algorithm for Automatically Fitting Digitized Curves", Philip J. Schneider, Graphics Gems, Academic Press, 1990
*/

typedef std::vector<osg::Vec2f> Curve; /*!< Curve is a vector of 2d points */

template <typename Real>
class PathFitter
{
public:
    PathFitter(const osg::Vec3Array& path);
    osg::Vec3Array* fit(Real error = 2.5);

protected:
    /* Fit a Bezier curve to a (sub-)set of digitized points */
    void fitCubic(osg::Vec3Array *curves, Real error, int first, int last, const osg::Vec2f& tan1, const osg::Vec2f& tan2);

private:
    void addCurve(osg::Vec3Array *curvesSet, const Curve& curve);

    Real getDistance(const osg::Vec2f& p1, const osg::Vec2f& p2);

    Curve generateBezier(int first, int last, const std::vector<Real>& uPrime, const osg::Vec2f& tan1, const osg::Vec2f& tan2);

    bool reparameterize(int first, int last, std::vector<Real> &u, const Curve& curve);
    Real findRoot(const Curve& curve, const osg::Vec2f& point, Real u);
    osg::Vec2f evaluate(int degree, const Curve& curve, Real t) const;
    std::vector<Real> chordLengthParameterize(int first, int last);
    Real findMaxError(int first, int last, const Curve& curve, const std::vector<Real> &u, int& index);

private:
    Curve m_data;
};

#endif // PATHFITTER_H
