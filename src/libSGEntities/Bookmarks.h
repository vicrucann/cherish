#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <osg/Group>
#include <osgDB/ObjectWrapper>

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
 * Scene graph wise, the entity::Bookmarks is an osg::Group node that contains STL data for camera
 * positions and names. Also
 * osg::Group helps to maintain the entity::SceneState as its children for each corresponding
 * bookmark without the need of direct serialization of each scene state.
 *
 * The need to inherit QObject class is so that to use signals and slots functionality.
 * The signals and slots are connected to the BookmarkWidget and thus it assures
 * a simultaneous update whether the bookmark changed are made directly from the widget,
 * or trigerred by scene graph functions (e.g., on reload the scene from file).
 *
 * Note: there can only be one entity::Bookmarks object in a scene graph, a child of entity::UserScene. This
 * class servers as a container for a list of entity::SceneState -s and a list of camera parameters for each
 * scene state.
*/
class Bookmarks : public QObject, public osg::Group
{
    Q_OBJECT

public:
    /*! Default constructor. Creates an empty bookmark group. */
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

    /*! A method to extract scene state given a row index of the BookmarkWidget. */
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
    /*! A signal to request GLWidget to be set with correspondance of the passed bookmark
     * \param row is the bookmark index */
    void requestBookmarkSet(int row);

    /*! A singal to request bookmarks scene state to be updated.
     * \param state is the pointer on SceneState to be updated */
    void requestSceneData(entity::SceneState* state);

    /*! A signal is sent whenever it is requested to change current state with the requested state associated with
     * certain bookmark.
     * \param state is the requested state that will be applied to the scene. */
    void requestSceneStateSet(entity::SceneState* state);

public slots:
    /*! A slot is called whenever user performs a click on a bookmark from GUI. The slot sets up the requested bookmark and its
     * corresponding scene state.
     * \param index is the row index of the clicked bookmark on BookmarkWidget. */
    void onClicked(const QModelIndex& index);

    /*! A slot is called when user does editing of the bookmark's name from GUI. It edits the corresponding name of the bookrmark.
     * \param item is the pointer on the BookmarkWidget item from which the row index is derived to get the indexation for internal
     * vectors of Bookmarks. */
    void onItemChanged(QListWidgetItem* item);

    /*! A slot is called whenever user performs a change of order of the bookmarks from GUI. */
    void onRowsMoved(const QModelIndex&, int start, int end, const QModelIndex&, int row);

    /*! A slot is called whenever the rows are removed, e.g. by user from GUI or when closing current file.
     * \param first is the index of the first bookmark to remove
     * \param last is the index of the last bookmark to remove. The removal is done from the first till the last
     * item inclusive. */
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
