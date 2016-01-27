#include "EditEntityCommand.h"

#include <QObject>

EditCanvasOffsetCommand::EditCanvasOffsetCommand(entity::Canvas *canvas, const osg::Vec3f &translate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_canvas(canvas)
    , m_translate(translate)
{
    this->setText(QObject::tr("Offset %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

EditCanvasOffsetCommand::~EditCanvasOffsetCommand()
{
}

void EditCanvasOffsetCommand::undo()
{
    m_canvas->translate(osg::Matrix::translate(-m_translate.x(), -m_translate.y(), -m_translate.z()));
}

void EditCanvasOffsetCommand::redo()
{
    m_canvas->translate(osg::Matrix::translate(m_translate.x(), m_translate.y(), m_translate.z()));
}

/* =====================*/

EditCanvasRotateCommand::EditCanvasRotateCommand(entity::Canvas *canvas, const osg::Quat &rotate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_canvas(canvas)
    , m_rotate(rotate)
{
    this->setText(QObject::tr("Rotate %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

EditCanvasRotateCommand::~EditCanvasRotateCommand()
{

}

void EditCanvasRotateCommand::undo()
{
    double angle;
    osg::Vec3d axis;
    m_rotate.getRotate(angle, axis);
    m_canvas->rotate(osg::Matrix::rotate(-angle, axis));
}

void EditCanvasRotateCommand::redo()
{
    m_canvas->rotate(osg::Matrix::rotate(m_rotate));
}



EditPhotoMoveCommand::EditPhotoMoveCommand(entity::Canvas *canvas, const double u, const double v, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_canvas(canvas)
    , m_u0(m_canvas->getPhotoCurrent()->getCenter().x())
    , m_v0(m_canvas->getPhotoCurrent()->getCenter().y())
    , m_u1(u)
    , m_v1(v)
{
    this->setText(QObject::tr("Move photo %1 within canvas %2")
                  .arg(QString(m_canvas->getPhotoCurrent()->getName().c_str()),
                       QString(m_canvas->getName().c_str())));
}

EditPhotoMoveCommand::~EditPhotoMoveCommand()
{

}

void EditPhotoMoveCommand::undo()
{
    m_canvas->getPhotoCurrent()->move(m_u0, m_v0);
    m_canvas->updateFrame();
}

void EditPhotoMoveCommand::redo()
{
    m_canvas->getPhotoCurrent()->move(m_u1, m_v1);
    m_canvas->updateFrame();
}

EditStrokesPushCommand::EditStrokesPushCommand(const std::vector<entity::Stroke *> &strokes,
                                               entity::Canvas *current, entity::Canvas *target,
                                               const osg::Vec3f &eye,
                                               QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_strokes(strokes)
    , m_canvasCurrent(current)
    , m_canvasTarget(target)
    , m_eye(eye)
{
    this->setText(QObject::tr("Push set of strokes from %1 to %2")
                  .arg(QString(m_canvasCurrent->getName().c_str()),
                       QString(m_canvasTarget->getName().c_str())));
}

void EditStrokesPushCommand::undo()
{
    this->doPushStrokes(*(m_canvasTarget.get()), *(m_canvasCurrent.get()));
}

void EditStrokesPushCommand::redo()
{
    this->doPushStrokes(*(m_canvasCurrent.get()), *(m_canvasTarget.get()));
}

void EditStrokesPushCommand::doPushStrokes(entity::Canvas& source, entity::Canvas& target)
{
    osg::Matrix M = source.getTransform()->getMatrix();
    osg::Matrix invM = osg::Matrix::inverse(target.getTransform()->getMatrix());

    const osg::Plane plane = target.getPlane();
    const osg::Vec3f center = target.getCenter();

    for (unsigned int i=0; i<m_strokes.size(); ++i){
        entity::Stroke* s = m_strokes.at(i);
        osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(s->getVertexArray());
        for (unsigned int j=0; j<verts->size(); ++j){
            osg::Vec3f p = (*verts)[j];
            osg::Vec3f P = p * M;
            osg::Vec3f dir = P - m_eye;
            double len = plane.dotProductNormal(center-P) / plane.dotProductNormal(dir);
            osg::Vec3f P_ = dir * len + P;
            osg::Vec3f p_ = P_ * invM;
            (*verts)[j] = osg::Vec3f(p_.x(), p_.y(), 0.f);
        }
        target.getGeodeData()->addDrawable(s);
        source.getGeodeData()->removeDrawable(s);

        verts->dirty();
        s->dirtyBound();
    }
    target.updateFrame();
}
