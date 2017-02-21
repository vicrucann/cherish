#include "SceneState.h"

entity::SceneState::SceneState()
    : osg::ProtectedGroup()
    , m_axisFlag(true)
    , m_bookmarksFlag(true)
{
}

entity::SceneState::SceneState(const entity::SceneState &parent, osg::CopyOp copyop)
    : osg::ProtectedGroup(parent, copyop)
    , m_axisFlag(parent.m_axisFlag)
    , m_bookmarksFlag(parent.m_bookmarksFlag)
    , m_canvasDataFlags(parent.m_canvasDataFlags)
    , m_canvasToolFlags(parent.m_canvasToolFlags)
    , m_photoTransparencies(parent.m_photoTransparencies)
{
}

void entity::SceneState::setAxisFlag(bool flag)
{
    m_axisFlag = flag;
}

bool entity::SceneState::getAxisFlag() const
{
    return m_axisFlag;
}

void entity::SceneState::setBookmarksFlag(bool flag)
{
    m_bookmarksFlag = flag;
}

bool entity::SceneState::getBookmarksFlag() const
{
    return m_bookmarksFlag;
}

void entity::SceneState::setCanvasDataFlags(const std::vector<bool> &flags)
{
    m_canvasDataFlags = flags;
}

const std::vector<bool> &entity::SceneState::getCanvasDataFlags() const
{
    return m_canvasDataFlags;
}

void entity::SceneState::setCanvasToolFlags(const std::vector<bool> &flags)
{
    m_canvasToolFlags = flags;
}

const std::vector<bool> &entity::SceneState::getCanvasToolFlags() const
{
    return m_canvasToolFlags;
}

void entity::SceneState::setPhotoTransparencies(const std::vector<float> &transparencies)
{
    m_photoTransparencies = transparencies;
}

const std::vector<float> &entity::SceneState::getPhotoTransparencies() const
{
    return m_photoTransparencies;
}

void entity::SceneState::stripDataFrom(RootScene *scene)
{
    this->clear();
    m_axisFlag = scene->getAxesVisibility();
    m_bookmarksFlag = scene->getBookmarkToolVisibility();

    size_t sz = scene->getUserScene()->getNumCanvases();
    m_canvasDataFlags.resize(sz);
    m_canvasToolFlags.resize(sz);
    for (size_t i=0; i<sz; ++i){
        entity::Canvas* cnv = scene->getUserScene()->getCanvas(i);
        if (!cnv) continue;
        m_canvasDataFlags[i] = cnv->getVisibilityAll();
        m_canvasToolFlags[i] = cnv->getVisibilityFrameInternal();
        for (size_t j=0; j < cnv->getNumPhotos(); ++j){
            entity::Photo* photo = cnv->getPhoto(j);
            if (!photo) continue;
            float t = photo->getTransparency();
            m_photoTransparencies.push_back(t);
        }
    }
}

bool entity::SceneState::isEmpty() const
{
    if (m_canvasDataFlags.size() == 0 || m_canvasToolFlags.size() == 0)
        return true;
    return false;
}

void entity::SceneState::clear()
{
    m_canvasDataFlags.clear();
    m_canvasToolFlags.clear();
    m_photoTransparencies.clear();
}

void entity::SceneState::pushDataFlag(bool flag)
{
    m_canvasDataFlags.push_back(flag);
}

void entity::SceneState::popBackDataFlag()
{
    m_canvasDataFlags.pop_back();
}

void entity::SceneState::pushToolFlag(bool flag)
{
    m_canvasToolFlags.push_back(flag);
}

void entity::SceneState::popBackToolFlag()
{
    m_canvasToolFlags.pop_back();
}

void entity::SceneState::pushTransparency(float t)
{
    m_photoTransparencies.push_back(t);
}

void entity::SceneState::popBackTransparency()
{
    m_photoTransparencies.pop_back();
}

void entity::SceneState::insertDataFlag(int index, bool flag)
{
    if (index<0 || index>static_cast<int>(m_canvasDataFlags.size())){
        qFatal("insertDataFlag called: index is out of range. "
               "No insert will be performed. Scene state data is out of sync.");
        return;
    }
    m_canvasDataFlags.insert(m_canvasDataFlags.begin()+index, flag);

}

void entity::SceneState::eraseDataFlag(int index)
{
    if (index<0 || index>=static_cast<int>(m_canvasDataFlags.size())){
        qFatal("eraseDataFlag called: index is out of range. "
               "No erase will be performed. Scene state data is out of sync.");
        return;
    }

    m_canvasDataFlags.erase(m_canvasDataFlags.begin() + index);
}

void entity::SceneState::insertToolFlag(int index, bool flag)
{
    if (index<0 || index>static_cast<int>(m_canvasToolFlags.size())){
        qFatal("insertToolFlag called: index is out of range. "
               "No insert will be performed. Scene state data is out of sync.");
        return;
    }
    m_canvasToolFlags.insert(m_canvasToolFlags.begin()+index, flag);
}

void entity::SceneState::eraseToolFlag(int index)
{
    if (index<0 || index>=static_cast<int>(m_canvasToolFlags.size())){
        qFatal("eraseToolFlag called: index is out of range. "
               "No erase will be performed. Scene state data is out of sync.");
        return;
    }
    m_canvasToolFlags.erase(m_canvasToolFlags.begin()+index);
}

void entity::SceneState::insertTransparency(int index, float t)
{
    if (index<0 || index>static_cast<int>(m_photoTransparencies.size())){
        qFatal("insertTransparency called: index is out of range. "
               "No insert will be performed. Scene state data is out of sync.");
        return;
    }
    m_photoTransparencies.insert(m_photoTransparencies.begin()+index, t);
}

void entity::SceneState::eraseTransparency(int start, int number)
{
    if (start<0 || start>=static_cast<int>(m_photoTransparencies.size()) ||
            number<0 || number>static_cast<int>(m_photoTransparencies.size()) ||
            start+number>static_cast<int>(m_photoTransparencies.size()) ){
        qFatal("eraseTransparency called: index is out of range. "
               "No erase will be performed. Scene state data is out of sync.");
        return;
    }
    m_photoTransparencies.erase(m_photoTransparencies.begin()+start, m_photoTransparencies.begin()+start+number);
}

void entity::SceneState::resetTransparency(int index, float t)
{
    if (index<0 || index>=static_cast<int>(m_photoTransparencies.size())){
        qFatal("insertTransparency called: index is out of range. "
               "No insert will be performed. Scene state data is out of sync.");
        return;
    }
    m_photoTransparencies[index] = t;
}

bool entity::SceneState::addSVMData(const osg::Matrix &wall, const osg::Matrix &floor)
{
    osg::ref_ptr< entity::SVMData> svm = new entity::SVMData();
    Q_CHECK_PTR(svm.get());
    svm->setTransformWall(wall);
    svm->setTransformFloor(floor);
    return this->addChild(svm);
}

bool entity::SceneState::addCamPoseData(const osg::Matrix &matrix)
{
    osg::ref_ptr<entity::CamPoseData> cam = new entity::CamPoseData(60);
    Q_CHECK_PTR(cam.get());
    cam->setTransform(matrix);
    return this->addChild(cam);
}

entity::SVMData *entity::SceneState::getSVMData()
{
    if (this->getNumChildren() == 0) return NULL;
    osg::Node* node = this->getChild(0);
    Q_CHECK_PTR(node);
    return dynamic_cast<entity::SVMData*>(node);
}

osg::ProtectedGroup *entity::SceneState::getChildData()
{
    if (this->getNumChildren() == 0) return NULL;
    osg::Node* node = this->getChild(0);
    Q_CHECK_PTR(node);
    return dynamic_cast<osg::ProtectedGroup*>(node);
}

REGISTER_OBJECT_WRAPPER(SceneState_Wrapper
                        , new entity::SceneState
                        , entity::SceneState
                        , "osg::Object osg::Group entity::SceneState")
{
    ADD_BOOL_SERIALIZER(AxisFlag, true);
    ADD_BOOL_SERIALIZER(BookmarksFlag, true);
    ADD_LIST_SERIALIZER(CanvasDataFlags, std::vector<bool>);
    ADD_LIST_SERIALIZER(CanvasToolFlags, std::vector<bool>);
    ADD_LIST_SERIALIZER(PhotoTransparencies, std::vector<float>);
}

