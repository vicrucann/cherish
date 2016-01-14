#ifndef USERSCENE_H
#define USERSCENE_H

#include <string>

#include <QUndoStack>

#include <osg/Group>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgDB/ObjectWrapper>

#include "Canvas.h"
#include "Stroke.h"
#include "Photo.h"

namespace entity {
class UserScene : public osg::Group
{
public:
    /* ctors and initializers */
    UserScene();
    UserScene(const UserScene& scene, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    void setUndoStack(QUndoStack* stack);

    META_Node(entity, UserScene)

    /* setters/getters for serialization */
    void setIdCanvas(unsigned int id);
    unsigned int getIdCanvas() const;

    void setFilePath(const std::string& name);
    const std::string& getFilePath() const;
    bool isSetFilePath() const;

    /* other methods */
    void addCanvas(const osg::Matrix& R, const osg::Matrix& T);
    void addCanvas(const osg::Matrix& R, const osg::Matrix& T, const std::string& name);
    void addStroke(float u, float v, dureu::EVENT event);
    void addPhoto(const std::string& fname);

    entity::Canvas* getCanvas(unsigned int id);
    entity::Canvas* getCanvas(const std::string& name);
    int getCanvasLevel() const;
    int getPhotoLevel() const;

    bool setCanvasCurrent(entity::Canvas* cnv);
    bool setCanvasPrevious(entity::Canvas* cnv);
    entity::Canvas* getCanvasCurrent() const;
    entity::Canvas* getCanvasPrevious() const;

    void setTransformOffset(const osg::Vec3f& translate, const int mouse);
    void setTransformRotate(const osg::Vec3f& normal, const int mouse);

protected:
    ~UserScene();

    bool clearUserData();
    std::string getCanvasName();
    std::string getEntityName(const std::string& name, unsigned int id) const;

    void strokeStart();
    void strokeAppend(float u, float v);
    void strokeFinish();
    bool strokeValid() const;

private:
    QUndoStack* m_undoStack;
    osg::observer_ptr<entity::Canvas> m_canvasCurrent;
    osg::observer_ptr<entity::Canvas> m_canvasPrevious;
    osg::observer_ptr<entity::Canvas> m_canvasTarget; /* for push operations */
    osg::ref_ptr<entity::Stroke> m_strokeCurrent;
    unsigned int m_idCanvas;
    std::string m_filePath;
};

}

#endif // USERSCENE_H
