#include "iostream"
#include <sstream>
#include <stdlib.h>
#include <assert.h>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include "RootScene.h"
#include "axes.h"
#include "Settings.h"

RootScene::RootScene(QUndoStack *undoStack)
    : osg::Group()
    , m_userScene(new entity::UserScene)
    , m_axisGlo(new Axes)
    , m_observer(new ObserveSceneCallback)
    , m_hud(new HUDCamera(dureu::HUD_LEFT, dureu::HUD_RIGHT, dureu::HUD_BOTTOM, dureu::HUD_TOP))
    , m_undoStack(undoStack)
    , m_saved(false)
{
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

void RootScene::setFilePath(const std::string& name)
{
    m_userScene->setFilePath(name);
    m_saved = false;
}

bool RootScene::isSetFilePath() const
{
    return m_userScene->isSetFilePath();
}

bool RootScene::isSavedToFile() const
{
    return m_saved;
}

bool RootScene::isEmptyScene() const
{
    return m_userScene->isEmptyScene();
}

void RootScene::clearUserData()
{
    m_userScene->clearUserData();
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
    if (osgDB::writeNodeFile(*(m_userScene.get()), m_userScene->getFilePath())){
        m_saved = true;
        return true;
    }
    return false;

}

bool RootScene::loadSceneFromFile()
{
    if (!m_undoStack){
        fatalMsg("loadSceneFromFile(): undo stack is NULL. "
                 "Restart the program to ensure undo stack initialization.");
        return false;
    }
    osg::ref_ptr<entity::UserScene> newscene = dynamic_cast<entity::UserScene*>(osgDB::readNodeFile(m_userScene->getFilePath()));
    if (!newscene.get()){
        outErrMsg("loadSceneFromFile: could not load from file, or could not perform the dynamic_cast<osg::Group*>");
        return false;
    }

    /* replace the original */
    if (!this->replaceChild(m_userScene.get(), newscene.get())){
        outErrMsg("loadSceneFromFile: could not replace the original child");
        return false;
    }

    /* update pointer */
    m_userScene = newscene.get();

    /* reset observer */
    m_observer->setScenePointer(m_userScene.get());
    m_userScene->addUpdateCallback(m_observer.get());

    /* update current/previous canvases */
    for (unsigned int i=0; i<m_userScene->getNumChildren(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv){
            outErrMsg("loadSceneFromFile: could not extract a canvas from loaded scene");
            return false;
        }
        cnv->setColor(dureu::CANVAS_CLR_REST);
        m_userScene->setCanvasCurrent(cnv);
    }
    newscene = 0;
    this->printScene();
    m_saved = true;
    return true;
}

int RootScene::getStrokeLevel() const
{
    return m_userScene->getStrokeLevel();
}

int RootScene::getCanvasLevel() const
{
    return m_userScene->getCanvasLevel();
}

int RootScene::getPhotoLevel() const
{
    return m_userScene->getPhotoLevel();
}

void RootScene::addCanvas(const osg::Matrix& R, const osg::Matrix& T)
{
    m_userScene->addCanvas(m_undoStack, R, T);
    m_saved = false;
}

void RootScene::addCanvas(const osg::Matrix& R, const osg::Matrix& T, const std::string& name)
{
    m_userScene->addCanvas(m_undoStack, R, T, name);
    m_saved = false;
}

void RootScene::addStroke(float u, float v, dureu::EVENT event)
{
    m_userScene->addStroke(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::selectAllStrokes()
{
    if (this->getCanvasCurrent())
        this->getCanvasCurrent()->selectAllStrokes();
}

void RootScene::addPhoto(const std::string& fname)
{
    m_userScene->addPhoto(m_undoStack, fname);
    m_saved = false;
}

void RootScene::eraseStroke(entity::Stroke *stroke, const osg::Vec3d &hit, dureu::EVENT event)
{
    m_userScene->eraseStroke(m_undoStack, stroke, hit, event);
    m_saved = false;
}

void RootScene::setCanvasSelected(entity::Canvas *cnv)
{
    m_userScene->setCanvasSelected(cnv);
}

bool RootScene::setCanvasCurrent(entity::Canvas* cnv)
{
    return m_userScene->setCanvasCurrent(cnv);
}

bool RootScene::setCanvasPrevious(entity::Canvas* cnv)
{
    return m_userScene->setCanvasPrevious(cnv);
}

entity::Canvas *RootScene::getCanvasSelected() const
{
    return m_userScene->getCanvasSelected();
}

entity::Canvas* RootScene::getCanvasCurrent() const
{
    return m_userScene->getCanvasCurrent();
}

entity::Canvas* RootScene::getCanvasPrevious() const
{
    return m_userScene->getCanvasPrevious();
}

void RootScene::editCanvasOffset(const osg::Vec3f& translate, dureu::EVENT event)
{
    m_userScene->editCanvasOffset(m_undoStack, translate, event);
    m_saved = false;
}

void RootScene::editCanvasRotate(const osg::Quat &rotation, dureu::EVENT event)
{
    m_userScene->editCanvasRotate(m_undoStack, rotation, event);
    m_saved = false;
}

void RootScene::editPhotoMove(const double u, const double v, dureu::EVENT event)
{
    m_userScene->editPhotoMove(m_undoStack, u, v, event);
}

void RootScene::editStrokesPush(osg::Camera *camera)
{
    m_userScene->editStrokesPush(camera);
    m_saved = false;
}

RootScene::~RootScene()
{

}

void RootScene::printScene()
{
    outLogVal("RootScene #children", this->getNumChildren());
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        outLogVal("Child", this->getChild(i)->getName());
    }
    outLogMsg("For each canvas");
    m_userScene->printScene();
}




