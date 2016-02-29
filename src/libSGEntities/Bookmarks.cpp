#include "Bookmarks.h"

entitylist::Bookmarks::Bookmarks()
    : osg::Group()
    , m_model(new QStandardItemModel)
{
}

entitylist::Bookmarks::Bookmarks(const Bookmarks &parent, osg::CopyOp copyop)
    : osg::Group(parent, copyop)
{

}

bool entitylist::Bookmarks::addBookmark(entity::ViewBookmark *vb)
{
    if (!vb) return false;
    return this->addChild(vb);
}

REGISTER_OBJECT_WRAPPER(Bookmarks_Wrapper
                        , new entitylist::Bookmarks
                        , entitylist::Bookmarks
                        , "osg::Object osg::Group entitylist::Bookmarks")
{

}
