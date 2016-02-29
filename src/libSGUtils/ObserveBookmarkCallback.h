#ifndef OBSERVEBOOKMARKCALLBACK_H
#define OBSERVEBOOKMARKCALLBACK_H

#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/NodeCallback>
#include <osg/observer_ptr>
#include <osg/Group>

class ObserveBookmarkCallback : public  osg::NodeCallback
{
public:
    ObserveBookmarkCallback(osg::Group* bookmarks);
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
    osg::observer_ptr<osg::Group> m_bookmarks;
};

#endif // OBSERVEBOOKMARKCALLBACK_H
