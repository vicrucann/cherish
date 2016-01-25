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

    META_Node(entity, UserScene)

    /* setters/getters for serialization */
    void setIdCanvas(unsigned int id);
    unsigned int getIdCanvas() const;

    void setIdPhoto(unsigned int id);
    unsigned int getIdPhoto() const;

    void setFilePath(const std::string& name);
    const std::string& getFilePath() const;
    bool isSetFilePath() const;

    /* other methods */
    void addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T);
    void addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T, const std::string& name);
    void addStroke(QUndoStack* stack, float u, float v, dureu::EVENT event);
    void addPhoto(QUndoStack* stack, const std::string& fname);
    void eraseStroke(QUndoStack* stack, entity::Stroke* stroke, const osg::Vec3d& hit, dureu::EVENT event);

    entity::Canvas* getCanvas(unsigned int id);
    entity::Canvas* getCanvas(const std::string& name);
    int getStrokeLevel() const;
    int getCanvasLevel() const;
    int getPhotoLevel() const;

    bool setCanvasCurrent(entity::Canvas* cnv);
    bool setCanvasPrevious(entity::Canvas* cnv);
    void setCanvasSelected(entity::Canvas* cnv);
    void setCanvasSelected(bool selected);
    entity::Canvas* getCanvasCurrent() const;
    entity::Canvas* getCanvasPrevious() const;
    entity::Canvas* getCanvasSelected() const;

    void editCanvasOffset(QUndoStack* stack, const osg::Vec3f& translate, dureu::EVENT event);
    void editCanvasRotate(QUndoStack* stack, const osg::Quat& rotation, dureu::EVENT event);
    void editPhotoMove(QUndoStack* stack, const double u, const double v, dureu::EVENT event);

    bool isEmptyScene() const;
    bool clearUserData();
    bool printScene();

protected:
    ~UserScene();

    std::string getCanvasName();
    std::string getPhotoName();
    std::string getEntityName(const std::string& name, unsigned int id) const;

    void strokeStart();
    void strokeAppend(float u, float v);
    void strokeFinish(QUndoStack* stack);
    bool strokeValid() const;

    void eraseStart(entity::Stroke* stroke, osg::Vec3d& hit);
    void eraseAppend(entity::Stroke* stroke, osg::Vec3d& hit);
    void eraseFinish(QUndoStack* stack, entity::Stroke* stroke);
    bool eraseValid(entity::Stroke* stroke) const;

    void canvasOffsetStart();
    void canvasOffsetAppend(const osg::Vec3f& t);
    void canvasOffsetFinish(QUndoStack* stack);
    bool canvasEditValid() const;

    void canvasRotateStart();
    void canvasRotateAppend(const osg::Quat& r);
    void canvasRotateFinish(QUndoStack* stack);

    void photoMoveStart();
    void photoMoveAppend(const double u, const double v);
    void photoMoveFinish(QUndoStack* stack, const double u, const double v);
    bool photoEditValid() const;


private:
    osg::observer_ptr<entity::Canvas> m_canvasCurrent;
    osg::observer_ptr<entity::Canvas> m_canvasPrevious;
    osg::observer_ptr<entity::Canvas> m_canvasSelected;
    osg::observer_ptr<entity::Canvas> m_canvasTarget; /* for push operations */
    osg::Vec3f m_deltaT; /* for edit operations: translate */
    osg::Quat m_deltaR; /* for edit operation: rotate */
    double m_u, m_v; /* move photo */
    unsigned int m_idCanvas;
    unsigned int m_idPhoto;
    std::string m_filePath;
};

}

#endif // USERSCENE_H
