#include "Bookmarks.h"

#include <QDir>
#include <QDebug>
#include <QtGlobal>

#include "Settings.h"
#include "MainWindow.h"

entity::Bookmarks::Bookmarks()
    : QObject()
    , osg::Group()
    , m_row(0)
{
    this->setName("Bookmarks");
    // TODO make sure this mask transfers after the scene is saved to file,
    // and then opened from the file.
    this->setNodeMask(cher::MASK_SVMDATA_IN); /* when using SVM methods */
}

entity::Bookmarks::Bookmarks(const Bookmarks &parent, osg::CopyOp copyop)
    : QObject()
    , osg::Group(parent, copyop)
    , m_eyes(parent.m_eyes)
    , m_centers(parent.m_centers)
    , m_ups(parent.m_ups)
    , m_names(parent.m_names)
    , m_fovs(parent.m_fovs)
    , m_row(parent.m_row)
{
}

void entity::Bookmarks::setEyes(const std::vector<osg::Vec3d> &eyes)
{
    m_eyes = eyes;
}

const std::vector<osg::Vec3d> &entity::Bookmarks::getEyes() const
{
    return m_eyes;
}

void entity::Bookmarks::setCenters(const std::vector<osg::Vec3d> &centers)
{
    m_centers = centers;
}

const std::vector<osg::Vec3d> &entity::Bookmarks::getCenters() const
{
    return m_centers;
}

void entity::Bookmarks::setUps(const std::vector<osg::Vec3d> &ups)
{
    m_ups = ups;
}

const std::vector<osg::Vec3d> &entity::Bookmarks::getUps() const
{
    return m_ups;
}

void entity::Bookmarks::setNames(const std::vector<std::string> &names)
{
    m_names = names;
}

const std::vector<std::string> &entity::Bookmarks::getNames() const
{
    return m_names;
}

void entity::Bookmarks::setFovs(const std::vector<double> &fovs)
{
    m_fovs = fovs;
}

const std::vector<double> &entity::Bookmarks::getFovs() const
{
    return m_fovs;
}

const entity::SceneState *entity::Bookmarks::getSceneState(int row) const
{
    if (row <0 || row >= static_cast<int>(this->getNumChildren())){
        qWarning("getSceneState: row index is out of range");
        return 0;
    }
    return dynamic_cast<const entity::SceneState*>(this->getChild(row));
}

entity::SceneState *entity::Bookmarks::getSceneState(int row)
{
    if (row <0 || row >= static_cast<int>(this->getNumChildren())){
        qWarning("getSceneState: row index is out of range");
        return 0;
    }
    return dynamic_cast<entity::SceneState*>(this->getChild(row));
}

entity::SceneState *entity::Bookmarks::getLastSceneState()
{
    int num = this->getNumBookmarks();
    if (num <= 0) return 0;
    return this->getSceneState(num-1);
}

void entity::Bookmarks::addBookmark(BookmarkWidget *widget, const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up, const std::string &name, const double &fov)
{
    // push to vector structures
    m_eyes.push_back(eye);
    m_centers.push_back(center);
    m_ups.push_back(up);
    m_names.push_back(name);
    m_fovs.push_back(fov);

    // add scene state as a child of this
    osg::ref_ptr<entity::SceneState> state = new entity::SceneState;
    if (!state) return;
    emit this->requestSceneData(state.get());
    if (state->isEmpty()){
        qWarning("addBookmark: could not initialize StateScene");
        return;
    }
    this->addChild(state.get());
    qDebug() << "After bookrmak added, num of children " << this->getNumChildren();

    // now can add data to the widget
    widget->addItem(QString(name.c_str()));
    qDebug() << "number of items " << widget->count();
    QListWidgetItem* item = widget->item(widget->count()-1);
    if (!item) return;
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    // take snapshot of the bookmark
    int idx = m_eyes.size()-1;
    QPixmap pmap = MainWindow::instance().getScreenshot(m_eyes[idx], m_centers[idx], m_ups[idx]);
    item->setIcon(QIcon(pmap));
}

/* update the look of the bookmakr's screenshot */
void entity::Bookmarks::updateBookmark(BookmarkWidget *widget, int row)
{
    if (!widget){
        qWarning("BookmarkWidget pointer is null. Screenshot will not be updated");
        return;
    }
    if (row >=0 && row < static_cast<int>(m_eyes.size())){
        QListWidgetItem* item = widget->item(row);
        QPixmap pmap = MainWindow::instance().getScreenshot(m_eyes[row],
                                                            m_centers[row],
                                                            m_ups[row]);;
        emit this->requestSceneStateSet(this->getSceneState(row));
        item->setIcon(QIcon(pmap));
    }
}

void entity::Bookmarks::deleteBookmark(BookmarkWidget *widget, const QModelIndex &index)
{
    qDebug("deleteBookmark: remove item widget");
    qDebug() << "deleteBookmars: total count of items " << widget->count();
    QListWidgetItem* item = widget->takeItem(index.row());
    if (item){
        delete item;
    }
}

void entity::Bookmarks::resetModel(BookmarkWidget *widget)
{
//    widget->clear();
    // to load old version data successfully, have to set FOV to deafult since it's missing
    if (m_fovs.size() != m_names.size()) m_fovs.resize(m_names.size(), 60.);

    for (unsigned int i=0; i<m_names.size(); ++i){
        widget->addItem(QString((m_names[i]).c_str()));
        QListWidgetItem* item = widget->item(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        emit requestSceneStateSet(this->getSceneState(i));
        QPixmap pmap = MainWindow::instance().getScreenshot(m_eyes[i], m_centers[i], m_ups[i]);
        item->setIcon(QIcon(pmap));
    }
}

void entity::Bookmarks::clearModel()
{
    m_eyes.clear();
    m_centers.clear();
    m_ups.clear();
    m_names.clear();
    m_fovs.clear();
    m_row = -1;
    this->removeChildren(0, this->getNumChildren()-1);
}

std::string entity::Bookmarks::getBookmarkName(int row) const
{
    if (row>=0 && row < static_cast<int>(m_names.size()))
        return m_names[row];
    else
        return "";
}

int entity::Bookmarks::getNumBookmarks() const
{
    Q_ASSERT(m_eyes.size() == m_ups.size() && m_ups.size() == m_centers.size()
           && m_centers.size() == m_names.size() && m_names.size() == m_fovs.size());
    return m_eyes.size();
}

bool entity::Bookmarks::editBookmarkPose(int index, const osg::Vec3f &eye, const osg::Vec3f &center, const osg::Vec3f &up, double fov)
{
    if (index<0 || index >= this->getNumBookmarks()){
        qWarning("editBookmarkPose: index exceeds dimensions");
        return false;
    }
    m_eyes[index] = eye;
    m_centers[index] = center;
    m_ups[index] = up;
    m_fovs[index] = fov;
    return true;
}

void entity::Bookmarks::onClicked(const QModelIndex &index)
{
    qDebug() << "Bookmarks: on clicked " << index.row();
    if (index.row() >=0 && index.row() < static_cast<int>( m_names.size()) ){
        m_row = index.row();
        qDebug() << "Selected bookmark " << QString(m_names[m_row].c_str());
        emit this->requestBookmarkSet(m_row);
    }
    else
        qDebug("onClicked: m_row is out of range");
}

void entity::Bookmarks::onItemChanged(QListWidgetItem *item)
{
    qDebug() << "onItemChanged: bookmark name edited to " << item->text();
    m_row = item->listWidget()->row(item);
    if (m_row >= 0 && m_row < static_cast<int>(m_names.size())){
        m_names[m_row] = (item->text()).toStdString();
    }
    else{
        qWarning("onItemChaged: could not update internal data, m_row is out of range");
        m_row=0;
    }
}

void entity::Bookmarks::onRowsMoved(const QModelIndex &, int start, int end, const QModelIndex &, int row)
{
    qDebug("Rows moved");
    qDebug() << "start " << start;
    qDebug() << "row " << row;
    this->moveItem<osg::Vec3d>(start, row, m_eyes);
    this->moveItem<osg::Vec3d>(start, row, m_centers);
    this->moveItem<osg::Vec3d>(start, row, m_ups);
    this->moveItem<std::string>(start, row, m_names);
    this->moveItem<double>(start, row, m_fovs);

    // get pointer in the state to move
    entity::SceneState* state = this->getSceneState(start);
    this->removeChild(state);
    this->insertChild(row, state); // make sure the index is correct since we removed
}

void entity::Bookmarks::onRowsRemoved(const QModelIndex &, int first, int last)
{
    qDebug() << "onRowsRemoved atBookmarks " << first;
    this->deleteBookmarkData(first, last);
}

void entity::Bookmarks::deleteBookmarkData(int first, int last)
{
    qDebug() << "deleting bookmark data at idx " << first;
    if (first >= static_cast<int>(m_names.size()) || first < 0){
        qWarning("deleteBookmark: first is out of range");
        return;
    }
    if (last >= static_cast<int>(m_names.size()) || last < 0){
        qWarning("deleteBookmark: last is out of range");
        return;
    }
    m_eyes.erase(m_eyes.begin()+first, m_eyes.begin()+last+1);
    m_centers.erase(m_centers.begin()+first, m_centers.begin()+last+1);
    m_ups.erase(m_ups.begin()+first, m_ups.begin()+last+1);
    m_names.erase(m_names.begin()+first, m_names.begin()+last+1);
    m_fovs.erase(m_fovs.begin()+first, m_fovs.begin()+last+1);

    if (first >= static_cast<int>(this->getNumChildren()) || first < 0){
        qWarning("deleteBookmarkData: first index is out of range");
        return;
    }
    if (last >= static_cast<int>(this->getNumChildren()) || last < 0){
        qWarning("deleteBookmarkData: last index is out of range");
        return;
    }
    this->removeChildren(first, last-first+1);

    qDebug() << "after removal of row[s], the size is " << this->getNumBookmarks();
    qDebug() << "after removal, scene state number " << this->getNumChildren();
}

template <typename T>
bool entity::Bookmarks::moveItem(size_t from, size_t to, std::vector<T> &list)
{
    if (from >= list.size() || to > list.size()) return false;
    if (from == to) return true;
    const size_t final = to > from ? to-1 : to;
    T tmp = list[from];
    list.erase(list.begin()+from, list.begin()+from+1);
    list.insert(list.begin()+final, tmp);
    return true;
}

REGISTER_OBJECT_WRAPPER(Bookmarks_Wrapper
                        , new entity::Bookmarks
                        , entity::Bookmarks
                        , "osg::Object osg::Group entity::Bookmarks")
{
    ADD_LIST_SERIALIZER(Eyes, std::vector<osg::Vec3d>);
    ADD_LIST_SERIALIZER(Centers, std::vector<osg::Vec3d>);
    ADD_LIST_SERIALIZER(Ups, std::vector<osg::Vec3d>);
    ADD_LIST_SERIALIZER(Names, std::vector<std::string>);
    ADD_LIST_SERIALIZER(Fovs, std::vector<double>);
}
