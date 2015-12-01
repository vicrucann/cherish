#ifndef HUDCAMERA
#define HUDCAMERA

#include <osg/Group>
#include <osg/Camera>
#include <osg/Switch>
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

class HUDCamera: public osg::Group{
public:
    HUDCamera(double left, double right, double bottom, double top):
        _camera(new osg::Camera),
        _switch(new osg::Switch)
    {
        this->addChild(_switch);
        _switch->addChild(_camera);

        _camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        _camera->setClearMask(GL_DEPTH_BUFFER_BIT);
        _camera->setRenderOrder(osg::Camera::POST_RENDER);
        _camera->setAllowEventFocus(false);
        _camera->setProjectionMatrix(osg::Matrix::ortho2D(left,right,bottom,top));
        _camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        //this->addTestDrawables();
    }

    void setVisibility(bool vis){
        _switch->setChildValue(_switch->getChild(0), vis);
    }

    bool getVisibility(){
        return _switch->getChildValue(_switch->getChild(0));
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

private:
    osg::Camera* _camera;
    osg::Switch* _switch;
}; //class HUDCamera

#endif // HUDCAMERA

