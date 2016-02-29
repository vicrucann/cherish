#ifndef USERSCENE_H
#define USERSCENE_H

#include <string>

#include <QUndoStack>
#include <QObject>

#include <osg/Group>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgDB/ObjectWrapper>

#include "Canvas.h"
#include "Stroke.h"
#include "Photo.h"
#include "ViewBookmark.h"

// multiple inheritance requires QObject to be first,
// for more, see:
// http://www.setnode.com/blog/qt-staticmetaobject-is-not-a-member-of/
namespace entity {
class UserScene : public QObject, public osg::Group
{
    Q_OBJECT
public:
    /* ctors and initializers */
    UserScene();
    UserScene(const UserScene& scene, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, UserScene)

    /* setters/getters for serialization */
    void setBookmarks(osg::Group* group);
    const osg::Group* getBookmarks() const;

    void setIdCanvas(unsigned int id);
    unsigned int getIdCanvas() const;

    void setIdPhoto(unsigned int id);
    unsigned int getIdPhoto() const;

    void setIdBookmark(unsigned int id);
    unsigned int getIdBookmark() const;

    void setFilePath(const std::string& name);
    const std::string& getFilePath() const;
    bool isSetFilePath() const;

    /* other methods */
    void addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T);
    void addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T, const std::string& name);
    void addStroke(QUndoStack* stack, float u, float v, dureu::EVENT event);
    void addPhoto(QUndoStack* stack, const std::string& fname);
    void addBookmark(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);
    void eraseStroke(QUndoStack* stack, entity::Stroke* stroke, int first, int last, dureu::EVENT event);

    entity::Canvas* getCanvas(unsigned int id);
    entity::Canvas* getCanvas(const std::string& name);
    int getStrokeLevel() const;
    int getCanvasLevel() const;
    int getPhotoLevel() const;

    bool setCanvasCurrent(entity::Canvas* cnv);
    bool setCanvasPrevious(entity::Canvas* cnv);
    void setCanvasSelected(entity::Canvas* cnv);
    void setCanvasSelected(bool selected);
    void setCanvasesButCurrent(bool enabled);
    entity::Canvas* getCanvasCurrent() const;
    entity::Canvas* getCanvasPrevious() const;
    entity::Canvas* getCanvasSelected() const;

    void editCanvasOffset(QUndoStack* stack, const osg::Vec3f& translate, dureu::EVENT event);
    void editCanvasRotate(QUndoStack* stack, const osg::Quat& rotation, dureu::EVENT event);
    void editCanvasClone(QUndoStack* stack, const osg::Vec3f& translate, dureu::EVENT event);
    void editCanvasDelete(QUndoStack* stack, entity::Canvas* canvas);

    void editPhotoMove(QUndoStack* stack, entity::Photo* photo, const double u, const double v, dureu::EVENT event);
    void editPhotoScale(QUndoStack* stack, entity::Photo* photo, const double u, const double v, dureu::EVENT event);
    void editPhotoRotate(QUndoStack* stack, entity::Photo* photo, const double u, const double v, dureu::EVENT event);
    void editPhotoFlip(QUndoStack* stack, entity::Photo* photo, bool horizontal);
    void editPhotoDelete(QUndoStack* stack, entity::Photo* photo);

    void editStrokesPush(QUndoStack* stack, osg::Camera* camera);
    void editStrokesMove(QUndoStack* stack, double u, double v, dureu::EVENT event);
    void editStrokesScale(QUndoStack* stack, double u, double v, dureu::EVENT event);
    void editStrokesRotate(QUndoStack* stack, double u, double v, dureu::EVENT event);
    void editStrokeDelete(QUndoStack* stack, entity::Stroke* stroke);

    bool isEmptyScene() const;
    bool clearUserData();
    bool printScene();
    void updateWidgets();

signals:
    void sendRequestUpdate();
    void sendAddBookmark(const std::string& name);

protected:
    ~UserScene();

    std::string getCanvasName();
    std::string getPhotoName();
    std::string getBookmarkName();
    std::string getEntityName(const std::string& name, unsigned int id) const;

    void strokeStart();
    void strokeAppend(float u, float v);
    void strokeFinish(QUndoStack* stack);
    bool strokeValid() const;

    void strokesMoveStart(double u, double v);
    void strokesMoveAppend(double u, double v);
    void strokesMoveFinish(QUndoStack* stack);
    bool strokesSelectedValid() const;

    void strokesScaleStart(double u, double v);
    void strokesScaleAppend(double u, double v);
    void strokesScaleFinish(QUndoStack* stack);

    void strokesRotateStart(double u, double v);
    void strokesRotateAppend(double u, double v);
    void strokesRotateFinish(QUndoStack* stack);

    void eraseStart(entity::Stroke* stroke, osg::Vec3d& hit);
    void eraseAppend(entity::Stroke* stroke, osg::Vec3d& hit);
    void eraseFinish(QUndoStack* stack, entity::Stroke* stroke);
    bool eraseValid(entity::Stroke* stroke) const;

    void canvasOffsetStart();
    void canvasOffsetAppend(const osg::Vec3f& t);
    void canvasOffsetFinish(QUndoStack* stack);
    bool canvasEditValid() const;

    void canvasCloneStart();
    void canvasCloneAppend(const osg::Vec3f& t);
    void canvasCloneFinish(QUndoStack* stack);
    bool canvasCloneValid() const;

    void canvasRotateStart();
    void canvasRotateAppend(const osg::Quat& r);
    void canvasRotateFinish(QUndoStack* stack);

    void photoMoveStart(entity::Photo* photo);
    void photoMoveAppend(const double u, const double v);
    void photoMoveFinish(QUndoStack* stack, const double u, const double v);
    bool photoEditValid() const;

    void photoScaleStart(entity::Photo* photo);
    void photoScaleAppend(double u, double v);
    void photoScaleFinish(QUndoStack* stack, double u, double v);

    void photoRotateStart(entity::Photo* photo, double u, double v);
    void photoRotateAppend(double u, double v);
    void photoRotateFinish(QUndoStack* stack, double u, double v);

private:
    osg::ref_ptr<osg::Group> m_bookmarks;
    osg::observer_ptr<entity::Canvas> m_canvasCurrent;
    osg::observer_ptr<entity::Canvas> m_canvasPrevious;
    osg::observer_ptr<entity::Canvas> m_canvasSelected;
    osg::observer_ptr<entity::Canvas> m_canvasTarget; /* for push operations */
    osg::observer_ptr<entity::Canvas> m_canvasClone; /* for clone current canvas */
    osg::Vec3f m_deltaT; /* for edit operations: translate */
    osg::Quat m_deltaR; /* for edit operation: rotate */
    double m_u, m_v; /* move photo */
    bool m_inits;
    double m_du, m_dv; /* move strokes */
    double m_scale; /* scale photo */
    double m_rotate;
    unsigned int m_idCanvas;
    unsigned int m_idPhoto;
    unsigned int m_idBookmark;
    std::string m_filePath;
};

}

#endif // USERSCENE_H
