#include "Bookmarks.h"
#include <assert.h>

#include "Settings.h"

entity::Bookmarks::Bookmarks()
    : QObject()
    , osg::Group()
{
    this->setName("Bookmarks");
}

entity::Bookmarks::Bookmarks(const Bookmarks &parent, osg::CopyOp copyop)
    : QObject()
    , osg::Group(parent, copyop)
    , m_eyes(parent.m_eyes)
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
    //QStandardItem* item = new QStandardItem(QString(name.c_str()));
    //this->appendRow(item);
}

void entity::Bookmarks::onClicked(const QModelIndex &index)
{
    int row = index.row();
    outLogVal("Selected bookmark", m_names[row]);
    emit this->sendBookmark(row);
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
