#include <iostream>

#include "ObserveSceneCallback.h"
#include "Settings.h"
#include "Canvas.h"
#include "Photo.h"

ObserveSceneCallback::ObserveSceneCallback()
    : osg::NodeCallback()
    , m_scene(0)
    , m_text(new osgText::Text)
    , m_geode(new osg::Geode)
{
    this->setTextProperties(osg::Vec3(dureu::HUD_TEXT_POSX, dureu::HUD_TEXT_POSY, dureu::HUD_TEXT_POSZ), dureu::HUD_TEXT_SIZE);
    m_geode->addDrawable(m_text.get());
    this->setName("Observer");
}

void ObserveSceneCallback::operator ()(osg::Node *node, osg::NodeVisitor *nv)
{
    std::string content = "Scene structure:\n";
    if (m_scene.valid()){
        for (unsigned int i=0; i<m_scene->getNumChildren(); ++i){
            // first level of the scene;
            // when more levels will be added, then we can always check if there is
            // need to display the current level by checking the number of canvases;
            // Then we can use nested loops to go through each child of each entity,
            // and display them
            entity::Canvas* cnv = dynamic_cast<entity::Canvas*>(m_scene->getChild(i));
            if (cnv) {
                content += "|-- " + m_scene->getChild(i)->getName() + "\n";
                for (unsigned int j=0; j<cnv->getGeodeData()->getNumChildren(); ++j){
                    entity::Photo* photo = dynamic_cast<entity::Photo*>(cnv->getGeodeData()->getChild(j));
                    if (photo){
                        content += "  |-- " + photo->getName() + "\n";
                    }
                }
            }
        }
    }
    if (m_text.valid())
        m_text->setText(content);
    //traverse(node,nv);
}


void ObserveSceneCallback::setScenePointer(osg::Group *scene){
    m_scene = scene;
}

const osg::Group *ObserveSceneCallback::getScenePointer() const{
    return m_scene.get();
}

void ObserveSceneCallback::setTextPointer(osgText::Text *text){
    m_text = text;
}

void ObserveSceneCallback::setTextProperties(const osg::Vec3 &pos, float size) {
    osg::notify(osg::NOTICE) << "setTextProperties(): (Vec3&, float)" << std::endl;
    m_text->setDataVariance(osg::Object::DYNAMIC);
    m_text->setCharacterSize(size);
    m_text->setAxisAlignment(osgText::TextBase::XY_PLANE);
    m_text->setPosition(pos);
    m_text->setColor(solarized::base0);
}

osg::Geode *ObserveSceneCallback::getTextGeode() const{
    return m_geode.get();
}
