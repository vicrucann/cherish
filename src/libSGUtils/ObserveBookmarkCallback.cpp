#include "ObserveBookmarkCallback.h"

#include "Settings.h"
#include "ViewBookmark.h"

ObserveBookmarkCallback::ObserveBookmarkCallback(entity::Bookmarks *bookmarks)
    : osg::NodeCallback()
    , m_bookmarks(bookmarks)
{
}

void ObserveBookmarkCallback::operator()(osg::Node*, osg::NodeVisitor*)
{
    outLogMsg("Bookmark callback: traversal");
    if (m_bookmarks.valid()){
        for (unsigned int i=0; i<m_bookmarks->getNumChildren(); ++i){
            //entity::ViewBookmark* vb = dynamic_cast<entity::ViewBookmark*>(m_bookmarks->getChild(i));
            //if (vb){
                /* updata Qt model content: bookmark name and thumbnail */
           // }
        }
    }
}
