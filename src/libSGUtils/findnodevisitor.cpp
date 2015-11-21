#include "findnodevisitor.h"

findNodeVisitor::findNodeVisitor(const std::string &name):
    osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
    _name(name)
{
}

void findNodeVisitor::apply(osg::Node &node)
{
    if (node.getName() == _name)
        _node = &node;
    traverse(node);
}

void findNodeVisitor::setNameToFind(const std::string &name)
{
    _name = name;
}

osg::Node *findNodeVisitor::getNode()
{
    return _node.get();
}
