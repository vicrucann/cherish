#include "Stroke.h"

#include <assert.h>

#include <QDebug>
#include <QtGlobal>
#include <QFile>
#include <QWidgetList>
#include "MainWindow.h"

#include <osg/Program>
#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/BlendFunc>
#include <osg/Point>
#include <osg/Texture2D>

#include "ModelViewProjectionMatrixCallback.h"
#include "ViewportVectorCallback.h"
#include "CameraEyeCallback.h"
#include "CanvasTransformCallback.h"
#include "CurveFitting/libPathFitter/OsgPathFitter.h"

entity::Stroke::Stroke()
    : entity::Entity2D()
    , m_lines(new osg::DrawArrays(GL_LINE_STRIP_ADJACENCY))
    , m_program(new ProgramStroke)
    , m_color(cher::STROKE_CLR_NORMAL)
    , m_isCurved(false)
    , m_isShadered(false)
{
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(m_color);
    osg::Vec3Array* verts = new osg::Vec3Array;

    this->addPrimitiveSet(m_lines.get());
    this->setVertexArray(verts);
    this->setColorArray(colors);
    this->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(cher::STROKE_LINE_WIDTH);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setAttributeAndModes(new osg::Point(cher::STROKE_LINE_WIDTH));
    stateset->setTextureAttributeAndModes(0, new osg::Texture2D, osg::StateAttribute::OFF);
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    this->setStateSet(stateset);

    this->setDataVariance(osg::Object::DYNAMIC);
    this->setUseDisplayList(false);
    this->setUseVertexBufferObjects(true);
    this->setName("Stroke");

    qDebug("New stroke ctor complete");
}

entity::Stroke::Stroke(const entity::Stroke& copy, const osg::CopyOp& copyop)
    : entity::Entity2D(copy, copyop)
    , m_lines(copy.m_lines)
    , m_program(copy.m_program)
    , m_color(copy.m_color)
    , m_isCurved(copy.m_isCurved)
    , m_isShadered(copy.m_isShadered)
{
    qDebug("stroke copy ctor done");
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

void entity::Stroke::setLines(osg::DrawArrays* lines)
{
    m_lines = lines;
}

const osg::DrawArrays*entity::Stroke::getLines() const
{
    return m_lines.get();
}

void entity::Stroke::setColor(const osg::Vec4f& color)
{
    m_color = color;
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    for (unsigned int i=0; i<colors->size(); ++i){
        (*colors)[i] = m_color;
    }
    //(*colors)[0] = color;
//    colors->front() = m_color;
    colors->dirty();
    this->dirtyDisplayList();
    this->dirtyBound();
}

const osg::Vec4f&entity::Stroke::getColor() const
{
    return m_color;
}

void entity::Stroke::setIsCurved(bool curved)
{
    m_isCurved = curved;
}

bool entity::Stroke::getIsCurved() const
{
    return m_isCurved;
}

void entity::Stroke::setIsShadered(bool shadered)
{
    m_isShadered = shadered;
}

bool entity::Stroke::getIsShadered() const
{
    return m_isShadered;
}

ProgramStroke *entity::Stroke::getProgram() const
{
    return m_program.get();
}

bool entity::Stroke::copyFrom(const entity::Stroke *copy)
{
    if (!copy){
        qWarning("Copy stroke is NULL");
        return false;
    }

    if (this->getColor() != cher::STROKE_CLR_NORMAL ||
            !this->getLines() ||
            static_cast<int>(this->getLines()->getMode()) != GL_LINE_STRIP_ADJACENCY) {
        qWarning("stroke::copyFrom() : stroke parameters check failed");
        return false;
    }

    if (this->getNumPoints() != 0 || copy->getNumPoints() == 0){
        qWarning("stroke::copyFrom() : stroke size check failed");
        return false;
    }

    if (copy->getIsShadered() && copy->getNumPoints() % 4 != 0){
        qWarning("Copy stroke vertex number must be divadable to 4");
        return false;
    }

    for (int i=0; i<copy->getNumPoints(); i++){
        osg::Vec2f p = copy->getPoint(i);
        this->appendPoint(p.x(), p.y());
    }
    this->setIsCurved(copy->getIsCurved());
    this->redefineToCurve(copy->getProgram()->getTransform());

    if (copy->getIsCurved() == this->getIsCurved() &&
            copy->getIsShadered() == this->getIsShadered() &&
            copy->getNumPoints() != this->getNumPoints()){
        qWarning("Stroke copy failed : number of points mismatch");
        return false;
    }

    return true;
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

void entity::Stroke::appendPoint(const float u, const float v)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    colors->push_back(cher::STROKE_CLR_NORMAL);
    colors->dirty();

    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    verts->push_back(osg::Vec3f(u,v,0.f));
    unsigned int sz = verts->size();

    m_lines->setFirst(0);
    m_lines->setCount(sz);

    verts->dirty();
    this->dirtyBound();
    // read more: http://forum.openscenegraph.org/viewtopic.php?t=2190&postdays=0&postorder=asc&start=15
}

osg::Vec2f entity::Stroke::getPoint(unsigned int i) const
{
    const osg::Vec3Array* verts = static_cast<const osg::Vec3Array*>(this->getVertexArray());
    if (!verts){
        qWarning("Stroke vertices are not initialized. Cannot obtain a point.");
        return osg::Vec2f(0.f, 0.f);
    }
    unsigned int sz = verts->size();
    if (i>=sz){
        qWarning("Stroke's index is out of range. Cannot obtain  a point");
        return osg::Vec2f(0.f, 0.f);
    }
    osg::Vec3f p = (*verts)[i];
    if (std::fabs(p.z()) > cher::EPSILON){
        qWarning("Stroke::getPoint : unexpected value of z-coordinate");
        qInfo() << p.z();
        return osg::Vec2f(0.f, 0.f);
    }

    return osg::Vec2f(p.x(), p.y());
}

bool entity::Stroke::redefineToCurve(osg::MatrixTransform *t, float tolerance)
{
    if (m_isCurved && m_isShadered) return true;

    if (!m_isCurved){
        osg::ref_ptr<osg::Vec3Array> path = static_cast<osg::Vec3Array*>(this->getVertexArray());
        if (!path.get()){
            qWarning("Vertex data is NULL");
            return false;
        }
        qDebug() << "path.samples=" << path->size();

        osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
        if (!colors){
            qWarning("Color data is NULL");
            return false;
        }

        /* set up auto threshold if necessary */
        if (tolerance < 0.f){
            /* auto threshold helps to avoid under-fitting or over-fitting of the curve
         * depending on the scale of drawn stroke. */
            float length = this->getLength();
            qDebug() << "length=" << length;

            tolerance = length * 0.001;
            qDebug() << "assume threshold=" << tolerance;
        }

        OsgPathFitter<osg::Vec3Array, osg::Vec3f, float> fitter;
        fitter.init(*(path.get()));
        osg::ref_ptr<osg::Vec3Array> curves = fitter.fit(tolerance);
        if (!curves.get()){
            qWarning("Curves is NULL");
            return false;
        }

        this->setVertexArray(curves.get());
        m_isCurved = true;
    }

    if (this->redefineToShader(t==0? MainWindow::instance().getCanvasCurrent()->getTransform() : t) ) {
        m_isShadered = true;
        qDebug() << "curves.number=" << this->getNumPoints()/4;
    }
    else{
        qWarning("Could not re-define to shader, re-defining to curve points instead");
        osg::Vec3Array* curves = static_cast<osg::Vec3Array*>(this->getVertexArray());
        osg::ref_ptr<osg::Vec3Array> points = this->getCurvePoints(curves);
        this->setVertexArray(points);

        qDebug() << "curves.points=" << points->size();
        m_isCurved = true;
        m_isShadered = false;
    }

    /* update sizing of vertices and color arrays */
    osg::Vec3Array* finalPts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    if (finalPts){
        m_lines->setFirst(0);
        m_lines->setCount(finalPts->size());
        finalPts->dirty();
        osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
        if (colors){
            colors->resize(finalPts->size(), m_color);
            colors->dirty();
        }
    }
    else
        qCritical("Unable to update geometry correctly");

    this->dirtyBound();

    return true;
}

// read more on why: http://stackoverflow.com/questions/36655888/opengl-thick-and-smooth-non-broken-lines-in-3d
bool entity::Stroke::redefineToShader(osg::MatrixTransform *t)
{
    /* use shader program */
    m_program->initialize(this->getOrCreateStateSet(),
                          MainWindow::instance().getCamera(),
                          t,
                          MainWindow::instance().getStrokeFogFactor());

    /* The used shader requires that each line segment is represented as GL_LINES_AJACENCY_EXT */
    osg::ref_ptr<osg::Vec3Array> bezierPts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    if (!bezierPts) return false;

    /* reset the primitive type */
    m_lines->set(GL_LINES_ADJACENCY_EXT, 0, bezierPts->size());

    /* set shader attributes */
    this->setVertexAttribArray(0, bezierPts, osg::Array::BIND_PER_VERTEX);
    osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(this->getColorArray());
    Q_ASSERT(colors);
    this->setVertexAttribArray(1, colors, osg::Array::BIND_PER_VERTEX);

    /* apply shader to the state set */
    Q_ASSERT(this->getOrCreateStateSet());
    this->getOrCreateStateSet()->setAttributeAndModes(m_program.get(), osg::StateAttribute::ON);

    m_isShadered = true;
    return true;
}

int entity::Stroke::getNumPoints() const
{
    const osg::Vec3Array* verts = static_cast<const osg::Vec3Array*>(this->getVertexArray());
    Q_ASSERT(verts);

    return static_cast<int>(verts->size());
}

float entity::Stroke::getLength() const
{
    osg::BoundingBox bb = this->getBoundingBox();
    if (std::fabs(bb.zMax()-bb.zMin()) > cher::EPSILON ){
        qWarning("Stroke->getLength(): z coordinates of a stroke are unexpected values");
        qDebug() << "zMax " << bb.zMax();
        qDebug() << "zMin " << bb.zMin();
        return 0;
    }
    return std::max(bb.xMax() - bb.xMin(), bb.yMax() - bb.yMin());
}

bool entity::Stroke::isLengthy() const
{
    return this->getLength()>cher::STROKE_MINL;
}

void entity::Stroke::moveDelta(double du, double dv)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i];
        (*verts)[i] = osg::Vec3f(du+vi.x(), dv+vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::Stroke::scale(double scaleX, double scaleY, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(scaleX*vi.x(), scaleY*vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::Stroke::scale(double scale, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(scale*vi.x(), scale*vi.y(), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

void entity::Stroke::rotate(double theta, osg::Vec3f center)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    for (unsigned int i=0; i<verts->size(); ++i){
        osg::Vec3f vi = (*verts)[i] - center;
        (*verts)[i] = center + osg::Vec3f(vi.x() * std::cos(theta) - vi.y() * std::sin(theta),
                                          vi.x() * std::sin(theta) + vi.y() * std::cos(theta), 0);
    }
    verts->dirty();
    this->dirtyBound();
}

cher::ENTITY_TYPE entity::Stroke::getEntityType() const
{
    return cher::ENTITY_STROKE;
}

osg::Vec3Array *entity::Stroke::getCurvePoints(const osg::Vec3Array *bezierPts) const
{
    Q_ASSERT(bezierPts->size() % 4 == 0);

    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;

    float delta = 1.f / cher::STROKE_SEGMENTS_NUMBER;
    for (unsigned int j=0; j<bezierPts->size(); j=j+4){
        auto b0 = bezierPts->at(j)
                , b1 = bezierPts->at(j+1)
                , b2 = bezierPts->at(j+2)
                , b3 = bezierPts->at(j+3) ;

        for (int i=0; i<=cher::STROKE_SEGMENTS_NUMBER; ++i){
            float t = delta * float(i);
            float t2 = t * t;
            float one_minus_t = 1.0 - t;
            float one_minus_t2 = one_minus_t * one_minus_t;
            osg::Vec3f p = (b0 * one_minus_t2 * one_minus_t + b1 * 3.0 * t * one_minus_t2 + b2 * 3.0 * t2 * one_minus_t + b3 * t2 * t);
            points->push_back(p);
        }
    }
    Q_ASSERT(points->size() == (cher::STROKE_SEGMENTS_NUMBER + 1) * (bezierPts->size() / 4));
    return points.release();
}

/* for serialization of stroke type
 * for more info, see OSG beginner's guide, or
 * OSG cookbook. In both, there is a section on serialization.
*/
REGISTER_OBJECT_WRAPPER(Stroke_Wrapper
                        , new entity::Stroke
                        , entity::Stroke
                        , "osg::Object osg::Geometry entity::Stroke")
{
    ADD_OBJECT_SERIALIZER(Lines, osg::DrawArrays, NULL);
    ADD_VEC4F_SERIALIZER(Color, osg::Vec4f());
    ADD_BOOL_SERIALIZER(IsCurved, false);
}
