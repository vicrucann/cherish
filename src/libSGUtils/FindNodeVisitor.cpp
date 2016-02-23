#include "FindNodeVisitor.h"

FindNodeVisitor::FindNodeVisitor(const std::string &name):
    osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
    m_name(name)
{
}

void FindNodeVisitor::apply(osg::Node &node)
{
    if (node.getName() == m_name)
        m_node = &node;
    traverse(node);
}

void FindNodeVisitor::setNameToFind(const std::string &name)
{
    m_name = name;
}

osg::Node *FindNodeVisitor::getNode()
{
    return m_node.get();
}
