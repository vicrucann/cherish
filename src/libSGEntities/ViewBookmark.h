#ifndef VIEWBOOKMARK_H
#define VIEWBOOKMARK_H

#include <osg/Node>
#include <osg/Object>
#include <osg/Geometry>
#include <osgDB/ObjectWrapper>

namespace entity {
class ViewBookmark : public osg::Object
{
public:
    ViewBookmark();
    ViewBookmark(const ViewBookmark& bm, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    ViewBookmark(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

    /* setters and getters for serialization */
    META_Object(entity, ViewBookmark)

    void setEye(const osg::Vec3d& eye);
    const osg::Vec3d& getEye() const;
    void setCenter(const osg::Vec3d& center);
    const osg::Vec3d& getCenter() const;
    void setUp(const osg::Vec3d& up);
    const osg::Vec3d& getUp() const;

    void getTransformations(osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up) const;

protected:
    ~ViewBookmark() {}

private:
    osg::Vec3d m_eye, m_center, m_up;
};
}

#endif // VIEWBOOKMARK_H
