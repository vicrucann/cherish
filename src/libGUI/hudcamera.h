#ifndef HUDCAMERA
#define HUDCAMERA

#include <osg/Camera>
#include <osg/Transform>
#include <osg/StateAttribute>
#include <osg/Matrix>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/LineWidth>
#include <osg/BlendFunc>

#include "settings.h"

/* HUDCamera
 * A tester class to create a HUD display that can show
 * text and images.
 */

class HUDCamera: public osg::Camera{
public:
    HUDCamera(double left, double right, double bottom, double top)
    {
        //this->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        this->setClearMask(GL_DEPTH_BUFFER_BIT);
        this->setRenderOrder(osg::Camera::POST_RENDER);
        this->setAllowEventFocus(false);
        //this->setProjectionMatrix(osg::Matrix::ortho2D(0.f, 1.f, 0.f, 1.f));
        this->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        this->addTestDrawables();
    }

    void addTestDrawables(){
        osg::Vec3 corner = osg::Vec3(0.0f,0.0f,0.0f);
        osg::Vec3 dir = osg::Vec3(dureu::AXES_SIZE,dureu::AXES_SIZE,0.0f);
        osg::Vec3Array* coords = new osg::Vec3Array(2);
        (*coords)[0] = corner;
        (*coords)[1] = corner+dir;
        osg::Geometry* geom = new osg::Geometry;
        geom->setVertexArray(coords);

        osg::Vec4Array* color = new osg::Vec4Array(2);
        (*color)[0] = solarized::orange;
        (*color)[1] = solarized::orange;

        geom->setColorArray(color, osg::Array::BIND_PER_VERTEX);
        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

        osg::StateSet* stateset = new osg::StateSet;
        osg::LineWidth* linewidth = new osg::LineWidth();
        linewidth->setWidth(3.0f);
        stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
        osg::BlendFunc* blendfunc = new osg::BlendFunc();
        stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
        stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        geom->setStateSet(stateset);

        osg::Geode* geode = new osg::Geode;
        geode->addDrawable(geom);

        this->addChild(geode);
    }

}; //class HUDCamera

#endif // HUDCAMERA

