#include "Bookmarks.h"
#include <assert.h>

#include <QDir>

#include "Settings.h"

entity::Bookmarks::Bookmarks()
    : QObject()
    , osg::Group()
    , m_row(0)
{
    this->setName("Bookmarks");
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

void entity::Bookmarks::addBookmark(BookmarkWidget *widget, const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up, const std::string &name, const double &fov)
{
    m_eyes.push_back(eye);
    m_centers.push_back(center);
    m_ups.push_back(up);
    m_names.push_back(name);
    m_fovs.push_back(fov);
    widget->addItem(QString(name.c_str()));
    outLogVal("number of items", widget->count());
    QListWidgetItem* item = widget->item(widget->count()-1);
    if (!item) return;
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    QPixmap pmap;
    int idx = m_eyes.size()-1;
    emit this->requestScreenshot(pmap, m_eyes[idx], m_centers[idx], m_ups[idx]);
    item->setIcon(QIcon(pmap));
}

/* update the look of the bookmakr's screenshot - is performed on every click */
void entity::Bookmarks::updateBookmark(BookmarkWidget *widget, int row)
{
    if (row >=0 && row < (int)m_eyes.size()){
        QListWidgetItem* item = widget->item(row);
        QPixmap pmap;
        emit this->requestScreenshot(pmap, m_eyes[row], m_centers[row], m_ups[row]);
        item->setIcon(QIcon(pmap));
    }
}

void entity::Bookmarks::deleteBookmark(BookmarkWidget *widget, const QModelIndex &index)
{
    outLogMsg("deleteBookmark: remove item widget");
    outLogVal("deleteBookmars: total count of items", widget->count());
    QListWidgetItem* item = widget->takeItem(index.row());
    if (item){
        delete item;
        //this->deleteBookmarkData(index.row());
    }
}

/* resetModel is needed to be called only on loading scene from file */
void entity::Bookmarks::resetModel(BookmarkWidget *widget)
{
//    widget->clear();
    // to load old version data successfully, have to set FOV to deafult since it's missing
    if (m_fovs.size() != m_names.size()) m_fovs.resize(m_names.size(), 60.);

    for (unsigned int i=0; i<m_names.size(); ++i){
        widget->addItem(QString((m_names[i]).c_str()));
        QListWidgetItem* item = widget->item(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        QPixmap pmap;
        emit this->requestScreenshot(pmap, m_eyes[i], m_centers[i], m_ups[i]);
        item->setIcon(QIcon(pmap));
    }
}

/* to manually clear all vectors data */
void entity::Bookmarks::clearModel()
{
    m_eyes.clear();
    m_centers.clear();
    m_ups.clear();
    m_names.clear();
    m_fovs.clear();
    m_row = -1;
}

std::string entity::Bookmarks::getBookmarkName(int row) const
{
    if (row>=0 && row < (int)m_names.size())
        return m_names[row];
    else
        return "";
}

int entity::Bookmarks::getNumBookmarks() const
{
    assert(m_eyes.size() == m_ups.size() && m_ups.size() == m_centers.size()
           && m_centers.size() == m_names.size() && m_names.size() == m_fovs.size());
    return m_eyes.size();
}

void entity::Bookmarks::onClicked(const QModelIndex &index)
{
    outLogVal("Bookmarks: on clicked", index.row());
    if (index.row() >=0 && index.row() < (int) m_names.size() ){
        m_row = index.row();
        outLogVal("Selected bookmark", m_names[m_row]);
        emit this->sendBookmark(m_row);
    }
    else
        outLogMsg("onClicked: m_row is out of range");
}

void entity::Bookmarks::onItemChanged(QListWidgetItem *item)
{
    outLogVal("onItemChanged: bookmark name edited to", item->text().toStdString());
    m_row = item->listWidget()->row(item);
    if (m_row >= 0 && m_row < (int)m_names.size()){
        m_names[m_row] = (item->text()).toStdString();
    }
    else{
        outErrMsg("onItemChaged: could not update internal data, m_row is out of range");
        m_row=0;
    }
}

void entity::Bookmarks::onRowsMoved(const QModelIndex &, int start, int end, const QModelIndex &, int row)
{
    outLogMsg("Rows moved");
    outLogVal("start", start);
    outLogVal("row", row);
    this->moveItem<osg::Vec3d>(start, row, m_eyes);
    this->moveItem<osg::Vec3d>(start, row, m_centers);
    this->moveItem<osg::Vec3d>(start, row, m_ups);
    this->moveItem<std::string>(start, row, m_names);
    this->moveItem<double>(start, row, m_fovs);
}

void entity::Bookmarks::onRowsRemoved(const QModelIndex &, int first, int last)
{
    outLogVal("onRowsRemoved atBookmarks", first);
    this->deleteBookmarkData(first, last);
}

void entity::Bookmarks::onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    outLogMsg("currentItemChanged");
}

void entity::Bookmarks::deleteBookmarkData(int first, int last)
{
    outLogVal("deleting bookmark data at idx", first);
    if (first >= (int)m_names.size() || first < 0){
        outErrMsg("deleteBookmark: first is out of range");
        return;
    }
    if (last >= (int)m_names.size() || last < 0){
        outErrMsg("deleteBookmark: last is out of range");
        return;
    }
    m_eyes.erase(m_eyes.begin()+first, m_eyes.begin()+last+1);
    m_centers.erase(m_centers.begin()+first, m_centers.begin()+last+1);
    m_ups.erase(m_ups.begin()+first, m_ups.begin()+last+1);
    m_names.erase(m_names.begin()+first, m_names.begin()+last+1);
    m_fovs.erase(m_fovs.begin()+first, m_fovs.begin()+last+1);
    outLogVal("after removal of row[s], the size is", this->getNumBookmarks());
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
