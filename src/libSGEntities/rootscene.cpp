#include "iostream"
#include <sstream>
#include <stdlib.h>
#include <assert.h>
#include <typeinfo>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include "osg/MatrixTransform"
#include <osgUtil/SceneView>
#include <osg/ValueObject>
#include <osg/Switch>

#include "rootscene.h"
#include "axes.h"
#include "settings.h"
#include "Canvas.h"
#include "Photo.h"
#include "findnodevisitor.h"
#include "AddCanvasCommand.h"
#include "AddPhotoCommand.h"

RootScene::RootScene(QUndoStack *undoStack)
    : osg::Group()
    , m_userScene(new entity::UserScene)
    , m_axisGlo(new Axes)
    , m_observer(new ObserveSceneCallback)
    , m_hud(new HUDCamera(dureu::HUD_LEFT, dureu::HUD_RIGHT, dureu::HUD_BOTTOM, dureu::HUD_TOP))
{
    m_userScene->setUndoStack(undoStack);

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

bool RootScene::loadSceneFromFile()
{
    /*if (!_undoStack){
        fatalMsg("loadSceneFromFile(): undo stack is NULL. "
                 "Restart the program to ensure undo stack initialization.");
        return false;
    }
    osg::ref_ptr<osg::Group> scene = dynamic_cast<osg::Group*>(osgDB::readNodeFile(m_userScene->getFilePath()));
    if (!scene.get()){
        outErrMsg("loadSceneFromFile: could not load from file, or could not perform the dynamic_cast<osg::Group*>");
        return false;
    }

    if (m_userScene->getNumChildren() > 0){
        outLogMsg("loadSceneFromFile(): the current m_userScene will be replaced by a scene from file");
        if (!this->clearUserData()){
            outErrMsg("loadSceneFromFile(): could not clear the current user scene data");
            scene = 0;
            return false;
        }
    }
    this->insertChild(0, scene.get());
    m_userScene = scene.get();

    m_observer->setScenePointer(m_userScene.get());
    m_userScene->addUpdateCallback(m_observer.get());

    _undoStack->clear();
    for (unsigned int i=0; i<m_userScene->getNumChildren(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv){
            outErrMsg("loadSceneFromFile: could not extract a canvas from loaded scene");
            return false;
        }
        cnv->setColor(dureu::CANVAS_CLR_REST);
        m_userScene->setCanvasCurrent(cnv);
    }
    scene = 0;*/
    return true;
}

bool RootScene::writeSceneToFile() const
{
    if (m_userScene->getNumChildren() == 0){
        outErrMsg("The scene is empty, there is nothing to save.");
        return false;
    }
    osg::Node* node = dynamic_cast<osg::Node*>(m_userScene.get());
    if (!node){
        outErrMsg("writeSceneToFile: could not perform the dynamic_cast. File will not be saved.");
        return false;
    }
    return osgDB::writeNodeFile(*node, m_userScene->getFilePath());
}

RootScene::~RootScene()
{

}




