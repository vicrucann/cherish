#include "EditableWire.h"

#include <QtGlobal>
#include <QDebug>
#include "Settings.h"
#include "Utilities.h"

entity::EditableWire::EditableWire(double fov2)
    : osg::MatrixTransform()
    , m_geode(new osg::Geode())
    , m_eye(new osg::Geometry())
    , m_center(new osg::Geometry())
    , m_focal(new osg::Geometry())
{
    this->addChild(m_geode);
    m_geode->addDrawable(m_eye);
    m_geode->addDrawable(m_center);
    m_geode->addDrawable(m_center);
    m_geode->addDrawable(m_focal);

    m_eye->setUseDisplayList(false);
    m_eye->setName(cher::NAME_CAM_EYE);
    m_center->setUseDisplayList(false);
    m_center->setName(cher::NAME_CAM_CENTER);
    m_focal->setUseDisplayList(false);
    m_focal->setName(cher::NAME_CAM_FOCAL);

    osg::Vec3Array* verts_focal = new osg::Vec3Array();
    verts_focal->push_back(cher::CENTER);
    verts_focal->push_back(osg::Vec3f(cher::SVMDATA_HALFWIDTH, cher::SVMDATA_HALFWIDTH, 0.f));
    verts_focal->push_back(osg::Vec3f(-cher::SVMDATA_HALFWIDTH, cher::SVMDATA_HALFWIDTH, 0));
    osg::Vec4Array* clr_focal = new osg::Vec4Array(3);
    m_focal->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 0, verts_focal->size()));
    m_focal->setVertexArray(verts_focal);
    m_focal->setColorArray(clr_focal, osg::Array::BIND_PER_VERTEX);
    this->setColorFocal(Utilities::getOsgColor(cher::CAMPOSE_CLR_FOCAL));
    m_focal->getOrCreateStateSet()->setAttribute(new osg::LineWidth(3.5f), osg::StateAttribute::ON);

    osg::Vec3Array* verts_eye = new osg::Vec3Array();
    verts_eye->push_back(cher::CENTER);
    osg::Vec4Array* clr_eye = new osg::Vec4Array(1);
    m_eye->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, verts_eye->size()));
    m_eye->setVertexArray(verts_eye);
    m_eye->setColorArray(clr_eye, osg::Array::BIND_OVERALL);
    this->setColorEyeDefaults();
    m_eye->getOrCreateStateSet()->setAttribute(new osg::Point(20.f), osg::StateAttribute::ON);

    osg::Vec3Array* verts_center = new osg::Vec3Array();
    verts_center->push_back(cher::CENTER);
    verts_center->push_back(osg::Vec3f(0.f, cher::SVMDATA_HALFWIDTH*1.5f, 0.f));
    osg::Vec4Array* clr_center = new osg::Vec4Array(2);
    m_center->addPrimitiveSet(new osg::DrawArrays(GL_LINE, 0, verts_center->size()));
    m_center->setVertexArray(verts_center);
    m_center->setColorArray(clr_center);
    this->setColorCenterDefaults();
    m_center->getOrCreateStateSet()->setAttribute(new osg::LineWidth(3.5), osg::StateAttribute::ON);

}

osg::Vec3f entity::EditableWire::getEye3D() const
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(m_eye->getVertexArray());
    Q_CHECK_PTR(verts);
    Q_ASSERT(verts->size() == 1);
    osg::Vec3f local = (*verts)[0];
    return local * this->getMatrix();
}

osg::Vec3f entity::EditableWire::getEye2D() const
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(m_eye->getVertexArray());
    Q_CHECK_PTR(verts);
    Q_ASSERT(verts->size() == 1);
    osg::Vec3f local = (*verts)[0];
    return local;
}

osg::Vec3f entity::EditableWire::getCenter3D() const
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(m_center->getVertexArray());
    Q_CHECK_PTR(verts);
    Q_ASSERT(verts->size() == 2);
    osg::Vec3f local = (*verts)[1];
    return local * this->getMatrix();
}

const osg::Geode *entity::EditableWire::getGeode() const
{
    return m_geode;
}

void entity::EditableWire::editEye(double u, double v)
{
    this->move(u, v);
}

void entity::EditableWire::editCenter(double theta)
{
    this->rotate(theta);
}

void entity::EditableWire::setColorFocal(const osg::Vec4f &color)
{
    osg::Vec4Array* clr = static_cast<osg::Vec4Array*>(m_focal->getColorArray());
    Q_CHECK_PTR(clr);
    Q_ASSERT(clr->size() == 3);
    for (unsigned int i=0; i<clr->size(); ++i)
        (*clr)[i] = color;
    this->updateGeometry(m_focal);
}

void entity::EditableWire::setColorEyeDefaults()
{
    osg::Vec4Array* clr = static_cast<osg::Vec4Array*>(m_eye->getColorArray());
    Q_CHECK_PTR(clr);
    Q_ASSERT(clr->size() == 1);
    (*clr)[0] = Utilities::getOsgColor(cher::CAMPOSE_CLR_EYE);
    this->updateGeometry(m_eye);
}

void entity::EditableWire::setColorCenterDefaults()
{
    osg::Vec4Array* clr = static_cast<osg::Vec4Array*>(m_center->getColorArray());
    Q_CHECK_PTR(clr);
    Q_ASSERT(clr->size() == 2);
    for (unsigned int i=0; i<clr->size(); ++i)
        (*clr)[i] = Utilities::getOsgColor(cher::CAMPOSE_CLR_CENTER);
    this->updateGeometry(m_center);
}

void entity::EditableWire::updateGeometry(osg::Geometry *geom)
{
    geom->dirtyDisplayList();
    geom->dirtyBound();
}

void entity::EditableWire::move(double u, double v)
{
    osg::Vec3f localEye = this->getEye2D();
    Q_ASSERT(localEye.z() == 0);
    // delta movement in [u, v]
    double du = u - localEye.x();
    double dv = v - localEye.y();

    // edit vertices of eye, center and focal
    osg::Vec3Array* verts_eye = static_cast<osg::Vec3Array*>(m_eye->getVertexArray());
    Q_CHECK_PTR(verts_eye);
    Q_ASSERT(verts_eye->size() == 1);
    (*verts_eye)[0] = osg::Vec3f(u, v, 0);
    this->updateGeometry(m_eye);

    osg::Vec3Array* verts_center = static_cast<osg::Vec3Array*>(m_center->getVertexArray());
    Q_CHECK_PTR(verts_center);
    Q_ASSERT(verts_center->size() == 2);
    for (unsigned int i=0; i<verts_center->size(); ++i)
        (*verts_center)[i] = (*verts_center)[i] + osg::Vec3f(du, dv, 0);
    this->updateGeometry(m_center);

    osg::Vec3Array* verts_focal = static_cast<osg::Vec3Array*>(m_focal->getVertexArray());
    Q_CHECK_PTR(verts_focal);
    Q_ASSERT(verts_focal->size() == 3);
    for (unsigned int i=0; i<verts_focal->size(); ++i)
        (*verts_focal)[i] = (*verts_focal)[i] + osg::Vec3f(du, dv, 0);
    this->updateGeometry(m_focal);
}

void entity::EditableWire::rotate(double theta)
{
    // eye remain on the same position, but we have to take its coords
    osg::Vec3Array* verts_eye = static_cast<osg::Vec3Array*>(m_eye->getVertexArray());
    Q_CHECK_PTR(verts_eye);
    Q_ASSERT(verts_eye->size() == 1);
    osg::Vec3f eye = (*verts_eye)[0];

    // rotate the center geometry
    // p_theta = vec3(x0*cos(theta) - y0*sin(theta), x0*sin(theta) + y0*cos(theta), 0);
    osg::Vec3Array* verts_center = static_cast<osg::Vec3Array*>(m_center->getVertexArray());
    Q_CHECK_PTR(verts_center);
    Q_ASSERT(verts_center->size() == 2);
    float x0 = (*verts_center)[1].x();
    float y0 = (*verts_center)[1].y();
    (*verts_center)[1] = osg::Vec3f(x0*std::cos(theta)-y0*std::sin(theta),
                                    x0*std::sin(theta)+y0*std::cos(theta),
                                    0.f);
    this->updateGeometry(m_center);

    // rotate the focal wire
    osg::Vec3Array* verts_focal = static_cast<osg::Vec3Array*>(m_focal->getVertexArray());
    Q_CHECK_PTR(verts_focal);
    Q_ASSERT(verts_focal->size() == 3);
    for (unsigned int i=1; i<verts_focal->size(); ++i){
        float x0 = (*verts_focal)[i].x();
        float y0 = (*verts_focal)[i].y();
        (*verts_focal)[i] = osg::Vec3f(x0*std::cos(theta)-y0*std::sin(theta),
                                       x0*std::sin(theta)+y0*std::cos(theta),
                                       0.f);
    }
    this->updateGeometry(m_focal);

}
