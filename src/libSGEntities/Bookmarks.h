#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <osg/Group>
#include <osgDB/ObjectWrapper>

/* Osg + Qt model of booksmarks
 * osg::Group is used in order to perform correct serialization, while
 * QListModel is used for display at the corresponding BookmarkWidget
*/

#include <QStandardItemModel>
#include "ViewBookmark.h"

namespace entitylist {
class Bookmarks : public osg::Group
{
public:
    Bookmarks();
    Bookmarks(const Bookmarks& parent, osg::CopyOp copyop = osg::CopyOp::SHALLOW_COPY);
    META_Node(entitylist, Bookmarks)

    bool addBookmark(entity::ViewBookmark* vb);

protected:
    ~Bookmarks() {}

private:
    QStandardItemModel* m_model;
};
}


#endif // BOOKMARKS_H
