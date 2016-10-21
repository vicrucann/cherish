#include "Stroke.h"

#include <QDebug>
#include <QtGlobal>
#include "MainWindow.h"

#include <osg/Program>
#include <osg/StateSet>
#include <osg/Camera>

#include "ModelViewProjectionMatrixCallback.h"
#include "ViewportVectorCallback.h"
#include "CameraEyeCallback.h"
#include "CanvasTransformCallback.h"
#include "CurveFitting/libPathFitter/OsgPathFitter.h"

const GLenum STROKE_PHANTOM_TYPE = GL_LINE_STRIP;

entity::Stroke::Stroke()
    : entity::ShaderedEntity2D(STROKE_PHANTOM_TYPE, osg::Geometry::BIND_PER_VERTEX, "Stroke", cher::STROKE_CLR_NORMAL)
    , m_isCurved(false)
{
}

entity::Stroke::Stroke(const entity::Stroke& copy, const osg::CopyOp& copyop)
    : entity::ShaderedEntity2D(copy, copyop)
    , m_isCurved(copy.m_isCurved)
{
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

void entity::Stroke::setIsCurved(bool curved)
{
    m_isCurved = curved;
}

bool entity::Stroke::getIsCurved() const
{
    return m_isCurved;
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

bool entity::Stroke::copyFrom(const entity::ShaderedEntity2D *copy)
{
    const entity::Stroke* stroke = dynamic_cast<const entity::Stroke*>(copy);
    if (!stroke){
        qWarning("Could not dynamic cast to Stroke, no copy will be done");
        return false;
    }

    this->setIsCurved(stroke->getIsCurved());
    if (!entity::ShaderedEntity2D::copyFrom(copy))
        qCritical("Stroke copy has failed");

    return true;
}

bool entity::Stroke::redefineToShape(osg::MatrixTransform *t)
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
        float tolerance = -1.f;
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
    m_program->updateTransform(t);
//    m_program->initialize(this->getOrCreateStateSet(),
//                          MainWindow::instance().getCamera(),
//                          t,
//                          MainWindow::instance().getStrokeFogFactor());

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

cher::ENTITY_TYPE entity::Stroke::getEntityType() const
{
    return cher::ENTITY_STROKE;
}

ProgramStroke *entity::Stroke::getProgram() const
{
    return dynamic_cast<ProgramStroke*>(m_program.get());
}

void entity::Stroke::appendPoint(const float u, const float v)
{
    entity::ShaderedEntity2D::appendPoint(u,v, cher::STROKE_CLR_NORMAL);
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
