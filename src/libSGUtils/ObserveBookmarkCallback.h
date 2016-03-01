#ifndef OBSERVEBOOKMARKCALLBACK_H
#define OBSERVEBOOKMARKCALLBACK_H

#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/NodeCallback>
#include <osg/observer_ptr>
#include <osg/Group>

#include "Bookmarks.h"

class ObserveBookmarkCallback : public  osg::NodeCallback
{
public:
    ObserveBookmarkCallback(entity::Bookmarks* bookmarks);
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
    osg::observer_ptr<entity::Bookmarks> m_bookmarks;
};

#endif // OBSERVEBOOKMARKCALLBACK_H
