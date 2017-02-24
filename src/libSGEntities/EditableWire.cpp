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
    , m_fov2(fov2)
    , m_selection(-1)
{
    this->addChild(m_geode);
    m_geode->addDrawable(m_eye);
    m_geode->addDrawable(m_center);
    m_geode->addDrawable(m_focal);

    m_eye->setUseDisplayList(false);
    m_eye->setName(cher::NAME_CAM_EYE);
    m_center->setUseDisplayList(false);
    m_center->setName(cher::NAME_CAM_CENTER);
    m_focal->setUseDisplayList(false);
    m_focal->setName(cher::NAME_CAM_FOCAL);

    double theta = (m_fov2 * 0.5f)*cher::PI/180.f;
    float X = cher::SVMDATA_HALFWIDTH * std::tan(theta);
    osg::Vec3Array* verts_focal = new osg::Vec3Array();
    verts_focal->push_back(cher::CENTER);
    verts_focal->push_back(osg::Vec3f(X, cher::SVMDATA_HALFWIDTH, 0.f));
    verts_focal->push_back(osg::Vec3f(-X, cher::SVMDATA_HALFWIDTH, 0));
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
    verts_center->push_back(osg::Vec3f(0.f, cher::SVMDATA_HALFWIDTH, 0.f));
    osg::Vec4Array* clr_center = new osg::Vec4Array(2);
    m_center->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, verts_center->size()));
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

osg::Vec3f entity::EditableWire::getUp() const
{
    return cher::UP;
}

void entity::EditableWire::getCenter2D(osg::Vec2f &p1, osg::Vec2f &p2) const
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(m_center->getVertexArray());
    Q_CHECK_PTR(verts);
    Q_ASSERT(verts->size() == 2);
    p1 = osg::Vec2f((*verts)[0].x(), (*verts)[0].y());
    p2 = osg::Vec2f((*verts)[1].x(), (*verts)[1].y());
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

void entity::EditableWire::editFocal(double distance)
{
    this->translate(distance);
}

void entity::EditableWire::pick(int index)
{

}

void entity::EditableWire::unpick()
{

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
    float x0 = (*verts_center)[1].x() - eye.x();
    float y0 = (*verts_center)[1].y() - eye.y();
    (*verts_center)[1] = eye +  osg::Vec3f(x0*std::cos(theta)-y0*std::sin(theta),
                                    x0*std::sin(theta)+y0*std::cos(theta),
                                    0.f);
    this->updateGeometry(m_center);

    // rotate the focal wire
    osg::Vec3Array* verts_focal = static_cast<osg::Vec3Array*>(m_focal->getVertexArray());
    Q_CHECK_PTR(verts_focal);
    Q_ASSERT(verts_focal->size() == 3);
    for (unsigned int i=1; i<verts_focal->size(); ++i){
        float x0 = (*verts_focal)[i].x() - eye.x();
        float y0 = (*verts_focal)[i].y() - eye.y();
        (*verts_focal)[i] = eye + osg::Vec3f(x0*std::cos(theta)-y0*std::sin(theta),
                                       x0*std::sin(theta)+y0*std::cos(theta),
                                       0.f);
    }
    this->updateGeometry(m_focal);

}

void entity::EditableWire::translate(double d)
{
    // if distance is smaller than threshold, do nothing
    if (d<0.2) return;

    // obtain eye position
    // eye remain on the same position, but we have to take its coords
    osg::Vec3Array* verts_eye = static_cast<osg::Vec3Array*>(m_eye->getVertexArray());
    Q_CHECK_PTR(verts_eye);
    Q_ASSERT(verts_eye->size() == 1);
    osg::Vec3f eye = (*verts_eye)[0];

    // use fov2 to calculate size with the given distanve
    // also scale up the center geometry as focal size changes
    osg::Vec3Array* verts_center = static_cast<osg::Vec3Array*>(m_center->getVertexArray());
    Q_CHECK_PTR(verts_center);
    Q_ASSERT(verts_center->size() == 2);
    osg::Vec3f center = (*verts_center)[1];
    osg::Vec3f dir = center-eye;
    dir.normalize();

    osg::Vec3Array* verts_focal = static_cast<osg::Vec3Array*>(m_focal->getVertexArray());
    Q_CHECK_PTR(verts_focal);
    Q_ASSERT(verts_focal->size() == 3);
    osg::Vec3f foc = (*verts_focal)[1] - (*verts_focal)[2];
    foc.normalize(); // foc is always perpendicular to dir

    double alpha = (m_fov2 * 0.5)*cher::PI/180.f; // half angle of fov
    double x05 = d * std::tan(alpha); // half distance of the focal length

    // center extends 20% more than position of focal
    osg::Vec3f new_center = eye + dir*d*1.2;
    (*verts_center)[1] = new_center;
    this->updateGeometry(m_center);

    // edit size and location of focal geometry
    osg::Vec3f new_middle = eye + dir * d;
    (*verts_focal)[1] = new_middle + foc * x05;
    (*verts_focal)[2] = new_middle - foc * x05;
    this->updateGeometry(m_focal);
}
