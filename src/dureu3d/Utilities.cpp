#include "Utilities.h"

#include "Settings.h"



bool Utilities::areStrokesProjectable(const std::vector<entity::Stroke *> &strokes, entity::Canvas *source, entity::Canvas *target, const osg::Vec3f &eye)
{
    if (!target || !source || strokes.empty()) {
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

    for (unsigned int i=0; i<strokes.size(); ++i){
        entity::Stroke* s0 = strokes.at(i);
        osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(s0->getVertexArray());
        for (unsigned int j=0; j<verts->size(); ++j){
            osg::Vec3 p = (*verts)[j];
            osg::Vec3 P = p * M;
            osg::Vec3 dir = P - eye;

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
    return true;
}
