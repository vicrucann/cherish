#include "PathFitter.h"

#include <math.h>
#include <algorithm>
#include <memory>

#include "Settings.h"

template <typename Real>
PathFitter<Real>::PathFitter(const osg::Vec3Array &path)
    : m_data(0)
{
    /* copy points from path and filter out adjacent duplicates */
    osg::Vec3f prev = osg::Vec3f(NAN,NAN,NAN);
    for (unsigned int i=0; i<path.size(); ++i){
        auto point = path.at(i);
        Q_ASSERT(std::fabs(point.z()) < cher::EPSILON);
        if (prev.isNaN() || prev != point)
        {
            m_data.push_back(osg::Vec2f(point.x(), point.y()));
            prev = point;
        }
    }
}

template <typename Real>
osg::Vec3Array *PathFitter<Real>::fit(Real error)
{
    osg::ref_ptr<osg::Vec3Array> bezierSet = new osg::Vec3Array;
    auto length = m_data.size();

    if (length>0){
        if (length > 1){
            auto tan1 = m_data.at(1) - m_data.at(0);
            auto tan2 = m_data.at(length-2) - m_data.at(length-1);
            tan1.normalize();
            tan2.normalize();
            this->fitCubic(bezierSet.get(), error, 0, length-1,
                           tan1, // left tangent
                           tan2); // right tangent
        }
    }
    return bezierSet.release();
}

template <typename Real>
void PathFitter<Real>::fitCubic(osg::Vec3Array *bezierSet, Real error, int first, int last, const osg::Vec2f& tan1, const osg::Vec2f& tan2)
{
    /* 2 points case */
    if (last-first == 1){
        auto pt1 = m_data.at(first);
        auto pt2 = m_data.at(last);
        auto dist = getDistance(pt1, pt2) / 3.f;
        auto pta = pt1 + tan1 * dist;
        auto ptb = pt2 + tan2 * dist;

        Curve curve(4);
        curve[0] = pt1;
        curve[1] = pta;
        curve[2] = ptb;
        curve[3] = pt2;

        this->addCurve(bezierSet, curve);
        return;
    }

    /* parameterize points and attempt to fit the curve */
    auto uPrime = this->chordLengthParameterize(first, last);
    auto maxError = (std::max)(error, error*error);
    int split = -1;
    bool parametersInOrder =  true;

    /* 4 iterations */
    for (int i=0; i<=4; ++i){
        Curve curve = this->generateBezier(first, last, uPrime, tan1, tan2);

        int index;
        auto merr = this->findMaxError(first, last, curve, uPrime, index);
        if (merr < error && parametersInOrder){
            this->addCurve(bezierSet, curve);
            return;
        }

        split = index;
        if (merr >= maxError)
            break;

        parametersInOrder = this->reparameterize(first, last, uPrime, curve);
        maxError = merr;
    }

    /* fitting failed */
    auto tanCenter = m_data.at(split-1) - m_data.at(split+1);
    this->fitCubic(bezierSet, error, first, split, tan1, tanCenter);
    this->fitCubic(bezierSet, error, split, last, -tanCenter, tan2 );
}

template <typename Real>
void PathFitter<Real>::addCurve(osg::Vec3Array *bezierSet, const Curve &curve)
{
    auto p0 = curve.at(0)
            , p1 = curve.at(1)
            , p2 = curve.at(2)
            , p3 = curve.at(3);
    bezierSet->push_back( osg::Vec3f(p0.x(), p0.y(), 0) );
    bezierSet->push_back( osg::Vec3f(p1.x(), p1.y(), 0) );
    bezierSet->push_back( osg::Vec3f(p2.x(), p2.y(), 0) );
    bezierSet->push_back( osg::Vec3f(p3.x(), p3.y(), 0) );
}

template <typename Real>
Real PathFitter<Real>::getDistance(const osg::Vec2f &p1, const osg::Vec2f &p2)
{
    auto x = p1.x() - p2.x();
    auto y = p1.y() - p2.y();
    return sqrt(x*x + y*y);
}

template <typename Real>
Curve PathFitter<Real>::generateBezier(int first, int last, const std::vector<Real> &uPrime, const osg::Vec2f &tan1, const osg::Vec2f &tan2)
{
    double epsilon = 1.0e-6;
    auto pt1 = m_data.at(first);
    auto pt2 = m_data.at(last);

    // C and X matrices
    Real C[2][2];
    Real X[2];

    for (int i=0, l=last-first+1; i<l; ++i){
        auto u = uPrime.at(i);
        auto t = 1-u,
                b = 3*u*t,
                b0 = t * t * t,
                b1 = b * t,
                b2 = b * u,
                b3 = u * u * u;
        auto a1 = tan1 * b1,
                a2 = tan2 * b2,
                tmp = m_data.at(first+i) - (pt1 * (b0 + b1)) - (pt2 * (b2 + b3)) ;
        C[0][0] += a1 * a1;
        C[0][1] += a1 * a2;
        C[1][0] = C[0][1];
        C[1][1] += a2 * a2;
        X[0] += a1 * tmp;
        X[1] += a2 * tmp;
    }

    /* determinant of C and X */
    auto detC0C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
    Real alpha1, alpha2;
    if (std::fabs(detC0C1) > epsilon){
        /* Kramer's rule */
        auto detC0X = C[0][0] * X[1] - C[1][0] * X[0];
        auto detXC1 = X[0] * C[1][1] - X[1] * C[0][1];

        /* alpha values */
        alpha1 = detXC1 / detC0C1;
        alpha2 = detC0X / detC0C1;
    }
    else {
        /* matrix is under-determined, assume alpha1=alpha2 */
        auto c0 = C[0][0] + C[0][1];
        auto c1 = C[1][0] + C[1][1];
        if (std::fabs(c0) > epsilon)
            alpha1 = alpha2 = X[0] / c0;
        else if (std::fabs(c1)>epsilon)
            alpha1 = alpha2 = X[1] / c1;
        else
            alpha1 = alpha2 = 0;

    }

    auto segLength = this->getDistance(pt2, pt1);
    auto eps = epsilon * segLength;
    osg::Vec2f handle1, handle2;
    if (alpha1 < eps || alpha2 < eps)
        alpha1 = alpha2 = segLength / 3.f;
    else {
        auto line = pt2 - pt1;
        handle1 = tan1 * alpha1;
        handle2 = tan2 * alpha2;
        if (handle1 * line - handle2 * line > segLength * segLength ){
            alpha1 = alpha2 = segLength / 3.f;
            handle1 = handle2 = osg::Vec2f(NAN, NAN);
        }
    }

    auto pta = handle1.isNaN()? pt1+tan1*alpha1 : pt1+handle1;
    auto ptb = handle2.isNaN() ? pt2+tan2*alpha2 : pt2 + handle2;

    return Curve{pt1, pt1+tan1*alpha1, pt2+tan2*alpha2, pt2};
}

template <typename Real>
bool PathFitter<Real>::reparameterize(int first, int last, std::vector<Real> &u, const Curve &curve)
{
    for (int i=first; i<=last; ++i){
        u[i-first] = this->findRoot(curve, m_data.at(i), u.at(i-first));
    }

    for (int i=1, l=u.size(); i<l; ++i ){
        if (u[i] <= u.at(i-1))
            return false;
    }
    return true;
}

template <typename Real>
Real PathFitter<Real>::findRoot(const Curve &curve, const osg::Vec2f &point, Real u)
{
    Curve Q1(3);
    Curve Q2(2);

    /* control vertices for Q' */
    for (int i=0; i<=2; ++i){
        Q1[i] = (curve.at(i+1) - curve.at(i)) * 3.f;
    }

    /* control vertices for Q'' */
    for (int i=0; i<=1; ++i){
        Q2[i] = (Q1.at(i+1) - Q1.at(i)) * 2.f;
    }

    /* compute Q(u), Q'(u) and Q''(u) */
    auto pt = this->evaluate(3, curve, u);
    auto pt1 = this->evaluate(2, Q1, u);
    auto pt2 = this->evaluate(1, Q2, u);
    auto diff = pt - point;
    auto df = pt1 * pt1 + diff * pt2;

    /* f(u) / f'(u) */
    if (std::fabs(df) < 1.0e-6)
        return u;

    /* u = u - f(u)/f'(u) */
    return u - (diff*pt1) / df;
}

template <typename Real>
osg::Vec2f PathFitter<Real>::evaluate(int degree, const Curve &curve, Real t) const
{
    Curve tmp(curve);

    /* triangle computation */
    for (int i=1; i<=degree; ++i){
        for (int j=0; j<=degree-i; ++j){
            tmp[j] = tmp.at(j) * (1-t) + tmp.at(j+1) * t;
        }
    }
    return tmp.at(0);
}

template <typename Real>
std::vector<Real> PathFitter<Real>::chordLengthParameterize(int first, int last)
{
    std::vector<Real> u(last-first+1);

    for (int i=first+1; i<=last; ++i){
        u[i-first] = u.at(i-first-1) + this->getDistance( m_data.at(i), m_data.at(i-1));
    }

    for (int i=1, m=last-first; i<=m; ++i ){
        u[i] /= u.at(m);
    }

    return u;
}

template <typename Real>
Real PathFitter<Real>::findMaxError(int first, int last, const Curve &curve, const std::vector<Real> &u, int &index)
{
    index = std::floor((last-first+1)/2);
    Real maxDist = 0;
    for (int i=first+1; i<last; ++i){
        auto P = this->evaluate(3, curve, u.at(i-first));
        auto v = P - m_data.at(i);
        auto dist = v.length2(); // squared distance
        if (dist >= maxDist){
            maxDist = dist;
            index = i;
        }
    }
    return maxDist;
}

/* define template instance that will be used in code
 * see more: https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
*/
template class PathFitter<float>;
