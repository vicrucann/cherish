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
 * positions and names. Also
 * osg::Group helps to maintain the SceneState as its children for each corresponding
 * bookmark without the need of direct serialization of each scene state.
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

    /*! A setter method to be used within OSG serialization procedures. */
    void setEyes(const std::vector<osg::Vec3d>& eyes);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<osg::Vec3d>& getEyes() const;

    /*! A setter method to be used within OSG serialization procedures. */
    void setCenters(const std::vector<osg::Vec3d>& centers);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<osg::Vec3d>& getCenters() const;

    /*! A setter method to be used within OSG serialization procedures. */
    void setUps(const std::vector<osg::Vec3d>& ups);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<osg::Vec3d>& getUps() const;

    /*! A setter method to be used within OSG serialization procedures. */
    void setNames(const std::vector<std::string>& names);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<std::string>& getNames() const;

    /*! A setter method to be used within OSG serialization procedures. */
    void setFovs(const std::vector<double>& fovs);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<double>& getFovs() const;

    /*! \param row is the index of SceneState
     * \return Pointer on the corresponding SceneState. */
    const entity::SceneState* getSceneState(int row) const;
    entity::SceneState* getSceneState(int row);

    /*! A method that performs an addition of a bookmark to the data structure. It also requests
     * a screenshot of the bookmark from GLWidget, and SceneState to add as a child to the bookmakr
     * which strips its data from the current state of RootScene.
     * \param widget is the BookmarkWidget to which a name is added
     * \param eye is the camera eye vector
     * \param center is the camera center vector
     * \param up is the camera up vector
     * \param fov is the camera FOV value */
    void addBookmark(BookmarkWidget* widget, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up, const std::string& name, const double& fov);

    /*! This method might be obsolete in the future */
    void updateBookmark(BookmarkWidget* widget, int row);

    /*! This method perform deletion of the indexed item from the provided widget. Note: it does not
     * erase elements from internal vector data structures since they are handled by the corresponding
     * slot onRowsRemoved() (same for the SceneState children).
     * \param widget is the widget from where the item is removed
     * \param index is the item's index */
    void deleteBookmark(BookmarkWidget* widget, const QModelIndex& index);

    /*! This method is called only when a scene is loaded from file. It resets the widget's content,
     * updates the screenshots.
     * \param widget is the widget to update */
    void resetModel(BookmarkWidget* widget);

    /*! The method to manually clear all the vector data as well as remove all SceneState children. */
    void clearModel();

    /*! \param row is the index of which name we want to obtain
     * \return The name of specified row. */
    std::string getBookmarkName(int row) const;

    /*! \return The number of bookmarks. */
    int getNumBookmarks() const;

signals:
    void requestSceneStateSet(entity::SceneState* state);

public slots:
    void onClicked(const QModelIndex& index);
    void onItemChanged(QListWidgetItem* item);
    void onRowsMoved(const QModelIndex&, int start, int end, const QModelIndex&, int row);
    void onRowsRemoved(const QModelIndex&, int first, int last);

private:
    template <typename T>
    bool moveItem(size_t from, size_t to, std::vector<T>& list);

    void deleteBookmarkData(int first, int last);

    std::vector<osg::Vec3d> m_eyes;
    std::vector<osg::Vec3d> m_centers;
    std::vector<osg::Vec3d> m_ups;
    std::vector<std::string> m_names;
    std::vector<double> m_fovs;

    int m_row;
};
}


#endif // BOOKMARKS_H
