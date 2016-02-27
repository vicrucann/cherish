#include "ViewBookmark.h"

entity::ViewBookmark::ViewBookmark()
    : osg::Node()
    , m_eye(osg::Vec3d(0,0,0))
    , m_center(osg::Vec3d(0,0,0))
    , m_up(osg::Vec3d(0,0,0))
{
}

entity::ViewBookmark::ViewBookmark(const ViewBookmark &bm, const osg::CopyOp &copyop)
    : osg::Node(bm, copyop)
    , m_eye(bm.m_eye)
    , m_center(bm.m_center)
    , m_up(bm.m_up)
{
}

entity::ViewBookmark::ViewBookmark(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up, const std::string &name)
    : osg::Node()
    , m_eye(eye)
    , m_center(center)
    , m_up(up)
{
    this->setName(name);
}

void entity::ViewBookmark::setEye(const osg::Vec3d &eye)
{
    m_eye = eye;
}

const osg::Vec3d &entity::ViewBookmark::getEye() const
{
    return m_eye;
}

void entity::ViewBookmark::setCenter(const osg::Vec3d &center)
{
    m_center = center;
}

const osg::Vec3d &entity::ViewBookmark::getCenter() const
{
    return m_center;
}

void entity::ViewBookmark::setUp(const osg::Vec3d &up)
{
    m_up = up;
}

const osg::Vec3d &entity::ViewBookmark::getUp() const
{
    return m_up;
}

void entity::ViewBookmark::getTransformations(osg::Vec3d &eye, osg::Vec3d &center, osg::Vec3d &up) const
{
    eye = m_eye;
    center = m_center;
    up = m_up;
}

REGISTER_OBJECT_WRAPPER(ViewBookmark_Wrapper
                        , new entity::ViewBookmark
                        , entity::ViewBookmark
                        , "osg::Object osg::Object entity::ViewBookmark")
{
    ADD_VEC3D_SERIALIZER(Eye, osg::Vec3d());
    ADD_VEC3D_SERIALIZER(Center, osg::Vec3d());
    ADD_VEC3D_SERIALIZER(Up, osg::Vec3d());
}
