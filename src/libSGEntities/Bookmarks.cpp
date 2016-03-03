#include "Bookmarks.h"
#include <assert.h>

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
    , m_row(0)
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

void entity::Bookmarks::addBookmark(BookmarkWidget *widget, const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up, const std::string &name)
{
    m_eyes.push_back(eye);
    m_centers.push_back(center);
    m_ups.push_back(up);
    m_names.push_back(name);
    widget->addItem(QString(name.c_str()));
    QListWidgetItem* item = widget->item(m_names.size()-1);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    //QStandardItem* item = new QStandardItem(QString(name.c_str()));
    //this->appendRow(item);
}

void entity::Bookmarks::deleteBookmark(size_t row)
{
    if (row >= m_names.size()){
        outErrMsg("deleteBookmark: row is out of range");
        return;
    }
    m_eyes.erase(m_eyes.begin()+row);
    m_centers.erase(m_centers.begin()+row);
    m_ups.erase(m_ups.begin()+row);
    m_names.erase(m_names.begin()+row);
}

/* resetModel is needed to be called only on loading scene from file */
void entity::Bookmarks::resetModel(BookmarkWidget *widget)
{
    widget->clear();
    for (unsigned int i=0; i<m_names.size(); ++i){
        widget->addItem(QString((m_names[i]).c_str()));
        QListWidgetItem* item = widget->item(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
}

void entity::Bookmarks::onClicked(const QModelIndex &index)
{
    m_row = index.row();
    if (m_row >=0 ){
        outLogVal("Selected bookmark", m_names[m_row]);
        emit this->sendBookmark(m_row);
    }
    else
        outErrMsg("onClicked: m_row is out of range");
}

void entity::Bookmarks::onItemChanged(QListWidgetItem *item)
{
    outLogVal("onItemChanged: bookmark name edited to", item->text().toStdString());
    if (m_row >= 0 && m_row < (int)m_names.size()){
        m_names[m_row] = (item->text()).toStdString();
    }
    else
        outErrMsg("onItemChaged: could not update internal data, m_row is out of range");
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
}

void entity::Bookmarks::onRowsRemoved(const QModelIndex &, int first, int)
{
    this->deleteBookmark(first);
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
}
