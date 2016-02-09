#ifndef FINDNODEVISITOR
#define FINDNODEVISITOR

#include <osg/NodeVisitor>
#include <osg/Node>

/* FindNodeVisitor
 * It is used in order to find a pointer to an entity with
 * a given name. It is done by traversing the scene graph and
 * comparing the name of each node. When the names are matched,
 * the pointer is updated. Otherwise, it remains NULL.
 * Modifying from examples:
 * <http://trac.openscenegraph.org/projects/osg//wiki/Support/Tutorials/FindingNodes>
 * or OSG Quick Start Guide, Listing 3-4 of Section 3.2.3 "NodeVisitors"
 */

class FindNodeVisitor : public osg::NodeVisitor
{
public:
    // Constructor that accepts string argument
    // Initializes _name to user string
    // set the traversal mode to TRAVERSE_ALL_CHILDREN
    FindNodeVisitor(const std::string& name);

    // The 'apply' method for 'node' type instances.
    // Compare the '_name' data member against the node's name.
    // If the strings match, assign it the _node
    virtual void apply(osg::Node& node);

    // Set the _name to user-defined string
    void setNameToFind(const std::string& name);

    osg::Node* getNode();
private:
    // the name we are looking for
    std::string _name;
    osg::ref_ptr<osg::Node> _node;
};

#endif // FINDNODEVISITOR

