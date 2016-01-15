#include "iostream"
#include <sstream>
#include <stdlib.h>
#include <assert.h>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include "RootScene.h"
#include "axes.h"
#include "settings.h"

RootScene::RootScene(QUndoStack *undoStack)
    : osg::Group()
    , m_userScene(new entity::UserScene)
    , m_axisGlo(new Axes)
    , m_observer(new ObserveSceneCallback)
    , m_hud(new HUDCamera(dureu::HUD_LEFT, dureu::HUD_RIGHT, dureu::HUD_BOTTOM, dureu::HUD_TOP))
{
    if (undoStack)
        m_userScene->setUndoStack(undoStack);
    else
        outErrMsg("RootScene ctor: undo stack is NULL");

    // child #0
    if (!this->addChild(m_userScene.get())){
        outErrMsg("RootScene(): could not add user scene as a child");
        fatalMsg("RootScene(): could not add user scene as a child");
    }

    // child #1
    if (!this->addChild(m_axisGlo.get())){
        outErrMsg("RootScene(): could not add axes as a child");
        fatalMsg("RootScene(): could not add axes as a child");
    }

    m_observer->setScenePointer(m_userScene.get());
    m_userScene->addUpdateCallback(m_observer.get());

    // child #2
    if (!this->addChild(m_hud.get())){
        outErrMsg("RootScene(): could not add hud as a child");
        fatalMsg("RootScene(): could not add hud as a child");
    }

    if (!m_hud->setText(m_observer->getTextGeode())){
        outErrMsg("RootScene(): could not set text to hud camera");
        fatalMsg("RootScene(): could not set text to hud camera");
    }

    this->setName("RootScene");
}

entity::UserScene*RootScene::getUserScene() const
{
    return m_userScene.get();
}

void RootScene::setAxesVisibility(bool vis) {
    m_axisGlo->setVisibility(vis);
}

bool RootScene::getAxesVisibility() const{
    return m_axisGlo->getVisibility();
}

const Axes* RootScene::getAxes() const{
    return m_axisGlo.get();
}

const ObserveSceneCallback *RootScene::getSceneObserver() const{
    return m_observer.get();
}

const HUDCamera *RootScene::getHudCamera() const{
    return m_hud.get();
}

void RootScene::setHudCameraVisibility(bool vis){
    m_hud->setVisibility(vis);
}

bool RootScene::getHudCameraVisibility() const{
    return m_hud->getVisibility();
}

bool RootScene::writeScenetoFile()
{
    if (m_userScene->getFilePath() == "")
        return false;
    return osgDB::writeNodeFile(*(m_userScene.get()), m_userScene->getFilePath());

}

bool RootScene::loadSceneFromFile()
{
    if (!m_userScene->getUndoStack()){
        fatalMsg("loadSceneFromFile(): undo stack is NULL. "
                 "Restart the program to ensure undo stack initialization.");
        return false;
    }
    osg::ref_ptr<entity::UserScene> scene = dynamic_cast<entity::UserScene*>(osgDB::readNodeFile(m_userScene->getFilePath()));
    outLogMsg("dynamic_cast complete");
    if (!scene.get()){
        outErrMsg("loadSceneFromFile: could not load from file, or could not perform the dynamic_cast<osg::Group*>");
        return false;
    }

    if (m_userScene->getNumChildren() > 0){
        outLogMsg("loadSceneFromFile(): the current m_userScene will be replaced by a scene from file");
        if (!m_userScene->clearUserData()){
            outErrMsg("loadSceneFromFile(): could not clear the current user scene data");
            scene = 0;
            return false;
        }
    }
    outLogVal("ptr before", m_userScene.get());
    //this->replaceChild(m_userScene.get(), dynamic_cast<entity::UserScene*>(osgDB::readNodeFile(m_userScene->getFilePath())));
    //this->replaceChild(m_userScene.get(), scene.get());
    if (!this->insertChild(0, dynamic_cast<osg::Node*>(scene.get())))
    {
        outErrMsg("Could not insert child");
        return false;
    }
    m_userScene = scene.get();
    m_observer->setScenePointer(m_userScene.get());
    m_userScene->addUpdateCallback(m_observer.get());
    outLogVal("ptr after", m_userScene.get());

    //m_userScene->clearUndoStack();

    for (unsigned int i=0; i<m_userScene->getNumChildren(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv){
            outErrMsg("loadSceneFromFile: could not extract a canvas from loaded scene");
            return false;
        }
        cnv->setColor(dureu::CANVAS_CLR_REST);
        m_userScene->setCanvasCurrent(cnv);
    }
    //scene = 0;
    return true;
}

RootScene::~RootScene()
{

}




