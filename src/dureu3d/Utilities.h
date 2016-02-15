#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>

#include "Canvas.h"
#include "Stroke.h"

class Utilities
{
public:
    static bool areStrokesProjectable(const std::vector<entity::Stroke *> &strokes, entity::Canvas *source, entity::Canvas *target, osg::Camera* camera)
    {
        if (!target || !source || strokes.empty() || !camera) {
            std::cerr << "push strokes: one of the pointers is NULL " << std::endl;
            return false;
        }
        osg::Matrix M = source->getTransform()->getMatrix();
        osg::Matrix m = target->getTransform()->getMatrix();
        osg::Matrix invM;
        if (!invM.invert(m)){
            std::cerr << "push strokes: could not invert model matrix" << std::endl;
            return false;
        }
        const osg::Plane plane = target->getPlane();
        const osg::Vec3 center = target->getCenter();

        /* get camera matrices and other data */
        osg::Vec3f eye, c, u;
        camera->getViewMatrixAsLookAt(eye, c, u);
        osg::Matrix VPW = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
        osg::Matrix invVPW;
        if (!invVPW.invert(VPW)){
            std::cerr << "areStrokesProjectable(): could not invert View-projection-world matrix for ray casting" << std::endl;
            return false;
        }

        std::vector<osg::Vec3f> ray(2);

        for (unsigned int i=0; i<strokes.size(); ++i){
            entity::Stroke* s0 = strokes.at(i);
            osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(s0->getVertexArray());
            for (unsigned int j=0; j<verts->size(); ++j){
                osg::Vec3 p = (*verts)[j];
                osg::Vec3 P = p * M;
                osg::Vec3 dir = P - eye;
                osg::Vec3f screen = P * VPW;
                osg::Vec3f nearPoint = osg::Vec3f(screen.x(), screen.y(), 0.f) * invVPW;
                osg::Vec3f farPoint = osg::Vec3f(screen.x(), screen.y(), 1.f) * invVPW;
                ray[0] = nearPoint;
                ray[1] = farPoint;

                if (plane.intersect(ray)){
                    outErrMsg("push strokes: some point projections do not intersect the target canvas plane."
                              "To resolve, change the camera view");
                    return false;
                }

                if (! plane.dotProductNormal(dir)){ // denominator
                    std::cerr << "push strokes: one of the points of projected stroke forms parallel projection to the canvas plane."
                                 "To resolve, change camera view." << std::endl;
                    return false;
                }
                if (! plane.dotProductNormal(center-P)){
                    std::cerr << "push strokes: plane contains the projected stroke or its part, so no single intersection can be defined."
                                 "To resolve, change camera view." << std::endl;
                    return false;
                }
                double len = plane.dotProductNormal(center-P) / plane.dotProductNormal(dir);
                osg::Vec3 P_ = dir * len + P;
                osg::Vec3 p_ = P_ * invM;
                if (std::fabs(p_.z())>dureu::EPSILON){
                    std::cerr << "push strokes: error while projecting point from global 3D to local 3D, z-coordinate is not zero." << std::endl;
                    outLogVec("P_", P_.x(), P_.y(), P_.z());
                    outLogVec("p_", p_.x(), p_.y(), p_.z());
                    return false;
                }
            }
        }
        outLogMsg("strokes are projectable");
        return true;
    }

    static const bool test();
};

#endif // UTILITIES_H
