#ifndef PRIVATEGROUP_H
#define PRIVATEGROUP_H

#include <osg/Group>

/*! \namespace osg
 * \brief An extentsion to OpenSceneGraph classes used within Cherish. */
namespace osg{

/*! \class ProtectedGroup
 * \brief A convinience class that inherits full functionality of osg::Group except
 * the methods that allow direct scene modification such as addition and deletion of children.
 */
class ProtectedGroup : public osg::Group
{
public:
    ProtectedGroup() : osg::Group() {}
    /*! \param group is the copy group, \param copyop is the copy method, e.g., deep or shallow (default). */
    ProtectedGroup(const ProtectedGroup& group, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
        : osg::Group(group, copyop) {}

protected:
    using osg::Group::addChild;
    using osg::Group::insertChild;
    using osg::Group::removeChild;
    using osg::Group::removeChildren;
    using osg::Group::replaceChild;
    using osg::Group::setChild;
    using osg::Group::asNode;
    using osg::Group::asDrawable;
    using osg::Group::asGeometry;
    using osg::Group::asGeode;
    using osg::Group::asGroup;
    using osg::Group::asTransform;
    using osg::Group::asCamera;
    using osg::Group::asSwitch;
}; // class
} // namespace

#endif // PRIVATEGROUP_H
