#include "FindNodeVisitor.h"

FindNodeVisitor::FindNodeVisitor(const std::string &name):
    osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
    _name(name)
{
}

void FindNodeVisitor::apply(osg::Node &node)
{
    if (node.getName() == _name)
        _node = &node;
    traverse(node);
}

void FindNodeVisitor::setNameToFind(const std::string &name)
{
    _name = name;
}

osg::Node *FindNodeVisitor::getNode()
{
    return _node.get();
}
