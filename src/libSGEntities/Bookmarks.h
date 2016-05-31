#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <osg/Group>
#include <osgDB/ObjectWrapper>

/* Osg + Qt model of booksmarks
 * osg::Group is used in order to perform correct serialization, while
 * QListModel is used for display at the corresponding BookmarkWidget
*/

#include <vector>
#include <list>
#include <string>

#include <QObject>
#include <QModelIndex>
#include <QListWidgetItem>
#include <QPixmap>
#include <QVector>

#include "../libGUI/ListWidget.h"
#include "SceneState.h"

class BookmarkWidget;
namespace entity {
class SceneState;
}

namespace entity {

/*! \class Bookmarks
 * \brief A proxy class that helps to connect scene graph camera positions
 * (in format eye, center, up and name) with the corresponding BookmarkWidget.
 *
 * The need for the class to inherit osg::Group class is so that to include the
 * bookmarks into saving to file procedure which is done by OpenSceneGraph.
 *
 * Scene graph wise, the Bookmarks is an osg::Group node that contains STL data for camera
 * positions and names.
 *
 * The need to inherit QObject class is so that to use signals and slots functionality.
 * The signals and slots are connected to the BookmarkWidget and thus it assures
 * a simultaneous update whether the bookmark changed are made directly from the widget,
 * or trigerred by scene graph functions (e.g., on reload the scene from file).
*/
class Bookmarks : public QObject, public osg::Group
{
    Q_OBJECT

public:
    Bookmarks();

    /*! Constructor by copy.
     * This functions is only used by OSG serializer. It is never called for
     * within the application.
     * \param parent is the copy-from object
     * \param copyop is the copy method, e.g., deep copy vs. shallow copy */
    Bookmarks(const Bookmarks& parent, osg::CopyOp copyop = osg::CopyOp::SHALLOW_COPY);
    META_Node(entity, Bookmarks)

    /*! OSG serialization setters and getters - required to register the serialization wrapper */

    /*! Set the vector of eyes */
    void setEyes(const std::vector<osg::Vec3d>& eyes);
    const std::vector<osg::Vec3d>& getEyes() const;

    void setCenters(const std::vector<osg::Vec3d>& centers);
    const std::vector<osg::Vec3d>& getCenters() const;

    void setUps(const std::vector<osg::Vec3d>& ups);
    const std::vector<osg::Vec3d>& getUps() const;

    void setNames(const std::vector<std::string>& names);
    const std::vector<std::string>& getNames() const;

    void setFovs(const std::vector<double>& fovs);
    const std::vector<double>& getFovs() const;

    void setSceneState(entity::SceneState* state);
    const entity::SceneState* getSceneState() const;

    /* other methods */
    void addBookmark(BookmarkWidget* widget,
                     const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up, const std::string& name, const double& fov);
    void updateBookmark(BookmarkWidget* widget, int row);
    void deleteBookmark(BookmarkWidget* widget, const QModelIndex& index);
    void resetModel(BookmarkWidget* widget);
    void clearModel();
    std::string getBookmarkName(int row) const;
    int getNumBookmarks() const;

signals:
    void sendBookmark(int row);
    void requestScreenshot(QPixmap& pixmap, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

public slots:
    void onClicked(const QModelIndex& index);
    void onItemChanged(QListWidgetItem* item);
    void onRowsMoved(const QModelIndex&, int start, int end, const QModelIndex&, int row);
    void onRowsRemoved(const QModelIndex&, int first, int last);
    void onCurrentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);

private slots:

protected:
    ~Bookmarks() {}

private:
    template <typename T>
    bool moveItem(size_t from, size_t to, std::vector<T>& list);

    void deleteBookmarkData(int first, int last);

    std::vector<osg::Vec3d> m_eyes;
    std::vector<osg::Vec3d> m_centers;
    std::vector<osg::Vec3d> m_ups;
    std::vector<std::string> m_names;
    std::vector<double> m_fovs;

    osg::ref_ptr<entity::SceneState> m_state;

    int m_row;
};
}


#endif // BOOKMARKS_H
