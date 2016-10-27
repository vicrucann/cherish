#include <assert.h>
#include <iostream>
#include <algorithm>
#include <cstdint>

#include "Canvas.h"
#include "Settings.h"
#include "Stroke.h"
#include "FindNodeVisitor.h"
#include "Utilities.h"
#include "MainWindow.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/BoundingBox>
#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/Plane>
#include <osg/BlendFunc>
#include <osg/Point>

#include <QtGlobal>
#include <QDebug>

entity::Canvas::Canvas()
    : osg::ProtectedGroup()
    , m_mR(osg::Matrix::rotate(0, cher::NORMAL))
    , m_mT(osg::Matrix::translate(0,0,0))
    , m_transform(new osg::MatrixTransform(m_mR * m_mT))
    , m_switch(new osg::Switch)
    , m_groupData(new osg::Group)
    , m_geodeStrokes(new osg::Geode)
    , m_geodePhotos(new osg::Geode)
    , m_geodePolygons(new osg::Geode)

    , m_programStroke(new ProgramStroke)

    , m_strokeCurrent(0)
    , m_polygonCurrent(0)

    , m_center(osg::Vec3f(0.f,0.f,0.f)) // moves only when strokes are introduced so that to define it as centroid
    , m_normal(cher::NORMAL)
    , m_edit(false)
{
    qDebug("New Canvas ctor complete");
}

entity::Canvas::Canvas(const entity::Canvas& cnv, const osg::CopyOp& copyop)
    : osg::ProtectedGroup(cnv, copyop)
    , m_mR(cnv.m_mR)
    , m_mT(cnv.m_mT)
    , m_transform(cnv.m_transform)
    , m_switch(cnv.m_switch)
    , m_groupData(cnv.m_groupData)
    , m_geodeStrokes(cnv.m_geodeStrokes)
    , m_geodePhotos(cnv.m_geodePhotos)
    , m_geodePolygons(cnv.m_geodePolygons)

    , m_programStroke(cnv.m_programStroke)

    , m_toolFrame(cnv.m_toolFrame)

    , m_strokeCurrent(0)
    , m_polygonCurrent(0)

    , m_center(cnv.m_center)
    , m_normal(cnv.m_normal)
    , m_edit(cnv.m_edit)
{
    qDebug("new Canvas by copy ctor complete");
}

void entity::Canvas::initializeTools()
{
    m_toolFrame = new entity::FrameTool();
    if (!m_toolFrame) qFatal("Canvas::initializeTools() - pointers are NULL");

    if (!m_switch.get()) qFatal("Canvas::initializeTools() - pointers are NULL");

    if (m_switch->getNumChildren() == 2){
        int index = m_switch->getChild(0) == m_groupData.get() ? 1 : 0;
        m_switch->removeChild(index, 1);
    }

    m_switch->addChild(m_toolFrame, true);
}

void entity::Canvas::initializeSG()
{
    /* OpenGL settings */
    this->initializeStateMachine();

    /* scene graph elements */
    this->addChild(m_transform.get());
    m_transform->setName("Transform");
    m_transform->addChild(m_switch.get());
    m_switch->setName("Switch");
    m_switch->addChild(m_groupData.get(), true); // 1st child of m_switch
    m_groupData->addChild(m_geodeStrokes.get());
    m_groupData->addChild(m_geodePhotos.get());
    m_groupData->addChild(m_geodePolygons.get());
    this->initializeTools(); // 2nd child of m_switch

    /* update internal parameters */
    this->updateTransforms();
    this->setColor(cher::CANVAS_CLR_REST);
    this->setVertices(m_center, cher::CANVAS_MINW, cher::CANVAS_MINH, cher::CANVAS_CORNER, cher::CANVAS_AXIS);

    /* traversal masks */
    this->initializeMasks();

    /* shaders */
    this->initializeProgramStroke();
}

void entity::Canvas::initializeStateMachine()
{
    /* OpenGL state machine for the canvas */
    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(cher::CANVAS_LINE_WIDTH);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    //blendfunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ANTIALIAS);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    this->setStateSet(stateset);

    /* state sets for stroke and polygon geodes (will propagate to its children) */
    osg::StateSet* ss_strokes = m_geodeStrokes->getOrCreateStateSet();
    Q_ASSERT(ss_strokes);
    osg::LineWidth* lw_strokes = new osg::LineWidth;
    lw_strokes->setWidth(cher::STROKE_LINE_WIDTH);
    ss_strokes->setAttributeAndModes(lw_strokes, osg::StateAttribute::ON);
    ss_strokes->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);
    ss_strokes->setAttributeAndModes(new osg::Point(cher::STROKE_LINE_WIDTH));
    ss_strokes->setTextureAttributeAndModes(0, new osg::Texture2D, osg::StateAttribute::OFF);
    ss_strokes->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    ss_strokes->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss_strokes->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    m_geodeStrokes->setStateSet(ss_strokes);

    osg::StateSet* ss_poly = m_geodePolygons->getOrCreateStateSet();
    Q_ASSERT(ss_poly);
    osg::LineWidth* lw_poly = new osg::LineWidth;
    lw_poly->setWidth(cher::POLYGON_LINE_WIDTH);
    ss_poly->setAttributeAndModes(lw_poly, osg::StateAttribute::ON);
    ss_poly->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);
    ss_poly->setAttributeAndModes(new osg::Point(cher::POLYGON_LINE_WIDTH));
    ss_poly->setTextureAttributeAndModes(0, new osg::Texture2D, osg::StateAttribute::OFF);
    ss_poly->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    ss_poly->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss_poly->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    m_geodePolygons->setStateSet(ss_poly);
}

void entity::Canvas::initializeMasks()
{
    /*  set traversal masks for intersectors (to be used from EventHandler) */
    this->setNodeMask(cher::MASK_CANVAS_IN);
    if (m_groupData.get())
        m_groupData->setNodeMask(cher::MASK_CANVASDATA_IN);
    if (m_toolFrame.get())
        m_toolFrame->setNodeMask(cher::MASK_CANVASFRAME_IN);
}

ProgramStroke *entity::Canvas::getProgramStroke() const
{
    return m_programStroke.get();
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void entity::Canvas::setMatrixRotation(const osg::Matrix& R)
{
    m_mR = R;
    this->updateTransforms();
}

const osg::Matrix&entity::Canvas::getMatrixRotation() const
{
    return m_mR;
}

void entity::Canvas::setMatrixTranslation(const osg::Matrix& T)
{
    m_mT = T;
    this->updateTransforms();
}

const osg::Matrix&entity::Canvas::getMatrixTranslation() const
{
    return m_mT;
}

/* This method should never be called directly.
 * It is here only to comply with serializer interface.
*/
void entity::Canvas::setTransform(osg::MatrixTransform* t)
{
    m_transform = t;
}

const osg::MatrixTransform*entity::Canvas::getTransform() const
{
    return m_transform.get();
}

osg::MatrixTransform* entity::Canvas::getTransform()
{
    return m_transform.get();
}

void entity::Canvas::setSwitch(osg::Switch* sw)
{
    m_switch = sw;
}

const osg::Switch* entity::Canvas::getSwitch() const
{
    return m_switch.get();
}

void entity::Canvas::setGroupData(osg::Group *group)
{
    m_groupData = group;
}

const osg::Group *entity::Canvas::getGroupData() const
{
    return m_groupData.get();
}

void entity::Canvas::setGeodeStrokes(osg::Geode *geode)
{
    m_geodeStrokes = geode;
}

const osg::Geode *entity::Canvas::getGeodeStrokes() const
{
    return m_geodeStrokes.get();
}

void entity::Canvas::setGeodePhotos(osg::Geode *geode)
{
    m_geodePhotos = geode;
}

const osg::Geode *entity::Canvas::getGeodePhotos() const
{
    return m_geodePhotos.get();
}

void entity::Canvas::setGeodePolygons(osg::Geode *geode)
{
    m_geodePolygons = geode;
}

const osg::Geode *entity::Canvas::getGeodePolygons() const
{
    return m_geodePolygons.get();
}

void entity::Canvas::setCenter(const osg::Vec3f &center)
{
    m_center = center;
}
#endif

const osg::Vec3f& entity::Canvas::getCenter() const
{
    return m_center;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void entity::Canvas::setNormal(const osg::Vec3f &normal)
{
    m_normal = normal;
}

const osg::Vec3f& entity::Canvas::getNormal() const
{
    return m_normal;
}
#endif

void entity::Canvas::setColor(const osg::Vec4f &color, const osg::Vec4f &colorIntersection)
{
    m_toolFrame->setColor(color, colorIntersection);
}

const osg::Vec4f &entity::Canvas::getColor() const
{
    return m_toolFrame->getColor();
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void entity::Canvas::setVisibilityFrame(bool vis)
{
    m_switch->setChildValue(m_toolFrame, vis);
}

bool entity::Canvas::getVisibilityFrame() const{
    return m_switch->getChildValue(m_toolFrame);
}
#endif

void entity::Canvas::setVisibilityFrameInternal(bool vis)
{
    m_toolFrame->setVisibility(vis);
}

bool entity::Canvas::getVisibilityFrameInternal() const
{
    return m_toolFrame->getVisibility();
}

bool entity::Canvas::getVisibilityAll() const
{
    Q_ASSERT(this->getVisibilityFrame()==this->getVisibilityData());
    return this->getVisibilityData();
}

void entity::Canvas::setVisibilityAll(bool vis)
{
    this->setVisibilityData(vis);
    this->setVisibilityFrame(vis);
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
bool entity::Canvas::getVisibilityData() const
{
    return m_switch->getChildValue(m_groupData);
}

void entity::Canvas::setVisibilityData(bool vis)
{
    m_switch->setChildValue(m_groupData, vis);
}
#endif // DOXYGEN_SHOULD_SKIP_THIS

osg::Vec3f entity::Canvas::getGlobalAxisU() const
{
    osg::Vec3f P = cher::CENTER;
    bool success = Utilities::getGlobalFromLocal(osg::Vec3f(1,0,0), m_mR, P);
    Q_ASSERT(success);
    P.normalize();
    return P;
}

osg::Vec3f entity::Canvas::getGlobalAxisV() const
{
    osg::Vec3f P = cher::CENTER;
    bool success = Utilities::getGlobalFromLocal(osg::Vec3f(0,1,0), m_mR, P);
    Q_ASSERT(success);
    P.normalize();
    return P;
}

const osg::Geometry *entity::Canvas::getGeometryPickable() const
{
    return m_toolFrame->getPickable();
}

// translates the current params on mt matrix
void entity::Canvas::translate(const osg::Matrix& mt)
{
    if (mt.isNaN()){
        qWarning("canvas-translate: translate matrix is NAN");
        return;
    }
    m_mT = m_mT * mt;
    this->updateTransforms();
}

void entity::Canvas::rotate(const osg::Matrix& mr, const osg::Vec3f &c3d_new)
{
    if (mr.isNaN()){
        qWarning("canvas-rotate: matrix is NAN");
        return;
    }
    /* first perform transform so that rotation would be perform around the center */
    osg::Matrix M = m_transform->getMatrix();
    osg::Matrix invM;
    if (!invM.invert(M)){
        qWarning("canvas rotate: could not invert matrix");
        return;
    }
    osg::Vec3f c2d_new = c3d_new * invM;
    osg::Vec3f c2d_old = m_center * invM;
    if (m_center != c3d_new){
        if (std::fabs(c2d_old.z()) > cher::EPSILON){
            qCritical("updateFrame(): local central point z-coord is not close to zero");
            return;
        }
        if (std::fabs(c2d_new.z()) > cher::EPSILON){
            qCritical("updateFrame(): local central point z-coord is not close to zero");
            return;
        }
        /* move every child back in local delta translation (diff between old and new centers) */
        osg::Vec3f delta2d = c2d_old - c2d_new;
        for (size_t i=0; i<this->getNumEntities(); ++i){
            entity::Entity2D* entity = this->getEntity(i);
            if (!entity){
                qCritical("rotate canvas: could not dynamic_cast to Entity2D*");
                return;
            }
            entity->moveDelta(delta2d.x(), delta2d.y());
        }

        /* new global center coordinate and delta translate in 3D */
        osg::Vec3f delta3d = c3d_new - m_center;
        /* move whole canvas and its drawables to be positioned at new global center */
        this->translate(osg::Matrix::translate(delta3d.x(), delta3d.y(), delta3d.z()));
    }

    /* now rotate the canvas */
    m_mR = m_mR * mr;
    this->updateTransforms();
}

void entity::Canvas::unselectAll()
{
    this->setStrokeCurrent(false);
    this->unselectEntities();
    this->setModeEdit(false);
}

void entity::Canvas::unselectEntities()
{
    m_selectedGroup.resetAll();
}

void entity::Canvas::selectAllEntities()
{
    this->unselectEntities();
    for (unsigned int i = 0; i < this->getNumEntities(); i++){
        entity::Entity2D* entity = this->getEntity(i);
        if (!entity) continue;
        this->addEntitySelected(entity);
    }
}

void entity::Canvas::setStrokeCurrent(entity::Stroke *stroke)
{
    if (m_strokeCurrent.get() == stroke)
        return;
    if (m_strokeCurrent.get() !=0 )
        this->setStrokeCurrent(false);
    m_strokeCurrent = stroke;
}

void entity::Canvas::setStrokeCurrent(bool current)
{
    if (!current)
        m_strokeCurrent = NULL;
}

entity::Stroke *entity::Canvas::getStrokeCurrent() const
{
    return m_strokeCurrent.get();
}

// TODO replace set*Current by setEntityCurrent
void entity::Canvas::setPolygonCurrent(entity::Polygon *poly)
{
    if (m_polygonCurrent.get() == poly)
        return;
    if (m_polygonCurrent.get()!=0)
        this->setPolygonCurrent(false);
    m_polygonCurrent = poly;
}

void entity::Canvas::setPolygonCurrent(bool current)
{
    if (!current)
        m_polygonCurrent = NULL;
}

entity::Polygon *entity::Canvas::getPolygonCurrent() const
{
    return m_polygonCurrent.get();
}

/* whenever new entity is added to selection,
 * update the selection frame;
 * if the entity is the first, then create the selection frame and
 * add it to the scene graph
 */
void entity::Canvas::addEntitySelected(Entity2D *entity)
{
    switch (entity->getEntityType()){
    case cher::ENTITY_STROKE:
        m_selectedGroup.addEntity(entity, m_geodeStrokes.get());
        break;
    case cher::ENTITY_PHOTO:
        m_selectedGroup.addEntity(entity, m_geodePhotos.get());
        break;
    default:
        break;
    }
}

/* whenever an entity is substracted from selection,
 * update the selection frame;
 * if no entities left, remove the selection frame from scene graph
*/
void entity::Canvas::removeEntitySelected(Entity2D *entity)
{
    m_selectedGroup.removeEntity(entity);
}

const std::vector<entity::Entity2D* >& entity::Canvas::getEntitiesSelected() const
{
    return m_selectedGroup.getEntities();
}

int entity::Canvas::getEntitiesSelectedSize() const
{
    return m_selectedGroup.getSize();
}

bool entity::Canvas::isEntitiesSelected() const
{
    return m_toolFrame->isSelected();
}

osg::Vec3f entity::Canvas::getEntitiesSelectedCenter3D() const
{
    return (m_selectedGroup.isEmpty() ? this->getCenter() : m_selectedGroup.getCenter3D(m_transform->getMatrix()) );
}

osg::Vec3f entity::Canvas::getEntitiesSelectedCenter2D() const
{
    return m_selectedGroup.getCenter2DCustom();
}

osg::Vec3f entity::Canvas::getCenter2D() const
{
    osg::Matrix M = m_transform->getMatrix();
    osg::Matrix invM;
    bool invertable = invM.invert(M);
    Q_ASSERT(invertable);
    return invertable? m_center * invM : cher::CENTER;
}

osg::Vec3f entity::Canvas::getBoundingBoxCenter3D() const
{
    osg::Vec3f center3d = m_center;
    osg::BoundingBox bb = this->getBoundingBox();
    if (bb.valid()){
        center3d = bb.center() * m_transform->getMatrix();

        /* check validity of the center - whether it belongs to the plane */
        osg::Matrix M = m_transform->getMatrix();
        osg::Matrix invM;
        invM.invert(M);
        osg::Vec3f center2d = center3d * invM;
        Q_ASSERT(std::fabs(center2d.z()) <= cher::EPSILON);
    }
    return center3d;
}

osg::Vec3f entity::Canvas::getBoundingBoxCenter2D() const
{
    osg::Vec3f center2d = this->getCenter2D();
    osg::BoundingBox bb = this->getBoundingBox();
    if (bb.valid()){
        center2d = bb.center();
    }
    return center2d;
}

osg::BoundingBox entity::Canvas::getBoundingBox() const
{
    osg::BoundingBox result;
    /* run through all the geodes of groupData and fing max and min og each bound box */
    float xmin=FLT_MAX, ymin=FLT_MAX, zmin=FLT_MAX, xmax=-FLT_MAX, ymax=-FLT_MAX, zmax=-FLT_MAX;
    for (unsigned int i=0; i<m_groupData->getNumChildren(); ++i){
        osg::Geode* gi = dynamic_cast<osg::Geode*>( m_groupData->getChild(i));
        if (!gi) continue;
        osg::BoundingBox bi = gi->getBoundingBox();
        if (!bi.valid()) continue;
        xmin = std::min(bi.xMin(), xmin);
        ymin = std::min(bi.yMin(), ymin);
        zmin = std::min(bi.zMin(), zmin);
        xmax = std::max(bi.xMax(), xmax);
        ymax = std::max(bi.yMax(), ymax);
        zmax = std::max(bi.zMax(), zmax);
        result.set(xmin, ymin, zmin, xmax, ymax, zmax);
    }

    return result.valid()? result : this->getToolFrame()->getGeodeWire()->getBoundingBox();
}

void entity::Canvas::moveEntities(std::vector<entity::Entity2D *>& entities, double du, double dv)
{
    m_selectedGroup.move(entities, du, dv);
}

void entity::Canvas::moveEntitiesSelected(double du, double dv)
{
    m_selectedGroup.move(du, dv);
}

void entity::Canvas::scaleEntities(std::vector<Entity2D *> &entities, double sx, double sy, osg::Vec3f center)
{
    m_selectedGroup.scale(entities, sx,sy,center);
}

void entity::Canvas::scaleEntitiesSelected(double sx, double sy)
{
    m_selectedGroup.scale(sx,sy);
}

void entity::Canvas::rotateEntities(std::vector<Entity2D *> entities, double theta, osg::Vec3f center)
{
    m_selectedGroup.rotate(entities, theta, center);
}

void entity::Canvas::rotateEntitiesSelected(double theta)
{
    m_selectedGroup.rotate(theta);
    //m_toolFrame->rotate(theta, m_selectedGroup.getCenter2DCustom());
}

/* If no entities are selected, the update frame is performed
 * so that to take into account new mean canvas center
 * When we update the frame in normal mode, we have to re-calculate
 * the new canvas center and move the virtual plane correspondingly.
 * For that reason we first perform movement back (opposite to the
 * necessary direction); and then after we draw the new canvas vertices,
 * we move the whole canvas by using translate operator.
 * For the frame in edit mode (when there are selected items),
 * we do not have to take this into account since the center is fixed there
 * and it does not represent the virtual plane's center.
 */
void entity::Canvas::updateFrame(entity::Canvas* against)
{
    if (this->getNumEntities() > 0){
        /* if there is a selected group, draw edit frame around it
         * the frame will allow to perform move, scale, rotate */
        if (m_selectedGroup.getSize() > 0){
            osg::BoundingBox bb = m_selectedGroup.getBoundingBox();
            if (bb.valid()){
                float szX = bb.xMax() - bb.xMin();
                float szY = bb.yMax() - bb.yMin();
                this->setVertices(m_selectedGroup.getCenter2DCustom(),
                                  szX*0.5+cher::CANVAS_EDITSLACK, szY*0.5+cher::CANVAS_EDITSLACK,
                                  cher::CANVAS_EDITQUAD, cher::CANVAS_EDITAXIS);
            }
        }
        /* if there is no selection, draw a normal canvas frame */
        else{
            /* local bounding box */
            osg::BoundingBox bb = this->getBoundingBox();
            if (bb.valid()){
                /* new 2d local center */
                osg::Vec3f c2d_new = bb.center();
                if (std::fabs(c2d_new.z()) > cher::EPSILON){
                    qCritical("Warning: updateFrame(): local central point z-coord is not close to zero");
                    return;
                }

                /* adjust the size of the canvas drawables in old location */
                float dx = 0.5*(bb.xMax()-bb.xMin())+cher::CANVAS_CORNER;
                float dy = 0.5*(bb.yMax()-bb.yMin())+cher::CANVAS_CORNER;
                float szX = std::max(dx, cher::CANVAS_MINW);
                float szY = std::max(dy, cher::CANVAS_MINW);
                this->setVertices(c2d_new, szX, szY, cher::CANVAS_CORNER, cher::CANVAS_AXIS);
            } /* bb is valid */
        } /* else no selection */
    }
    else /* geodeData has no childer, empty canvas case */
        this->setVerticesDefault(m_toolFrame->getCenterLocal());
    this->setIntersection(against);
}

const osg::Vec3Array *entity::Canvas::getFrameVertices() const
{
    return m_toolFrame->getVertices();
}

bool entity::Canvas::detachFrame()
{
    return m_switch->removeChild(m_toolFrame.get());
}

bool entity::Canvas::attachFrame()
{
    if (!m_toolFrame.get()) return false;
    return m_switch->addChild(m_toolFrame.get());
}

void entity::Canvas::setModeEdit(bool on)
{
    m_toolFrame->setEditable(on);
    m_edit = on;
}

bool entity::Canvas::getModeEdit() const
{
    return m_edit;
}

osg::Plane entity::Canvas::getPlane() const
{
    osg::Plane plane(m_normal, m_center);
    return plane;
}

entity::Canvas *entity::Canvas::clone() const
{
    osg::ref_ptr<entity::Canvas> clone = new Canvas;
    if (!clone.get()) return NULL;
    clone->initializeSG();
    clone->setMatrixRotation(this->getMatrixRotation());
    clone->setMatrixTranslation(this->getMatrixTranslation());
    clone->setName(this->getName());

    for (unsigned int i=0; i<this->getNumEntities(); ++i){
        entity::Entity2D* entcopy = this->getEntity(i);
        if (!entcopy) continue;
        switch(entcopy->getEntityType()){
        case cher::ENTITY_STROKE:
        {
            entity::Stroke* stroke = dynamic_cast<entity::Stroke*>(entcopy);
            if (stroke){
                entity::Stroke* si = new entity::Stroke;
                if (si){
                    si->copyFrom(stroke);
                    Q_ASSERT(si->getIsCurved());
                    if (!clone->addEntity(si)) qWarning("canvas clone: could not add stroke as drawable");
                }
                else qWarning("canvas clone: coult not clone the stroke");
            }
            else qWarning("canvas clone: could not dynamic_cast<Stroke>");
            break;
        }
        case cher::ENTITY_PHOTO:
            break;
        case cher::ENTITY_POLYGON:
        {
            entity::Polygon* poly = dynamic_cast<entity::Polygon*>(entcopy);
            if (poly){
                entity::Polygon* po = new entity::Polygon;
                if (po){
                    po->copyFrom(poly);
                    po->redefineToShape();
                    Q_ASSERT(po->isPolygon());
                    if (!clone->addEntity(po)) qWarning("canvas clone: could not add polygon as drawable");
                }
            }
            break;
        }
        default:
            break;
        }
    }
    clone->updateFrame();

    return clone.release();
}

entity::Canvas *entity::Canvas::separate()
{
    osg::ref_ptr<entity::Canvas> clone = new Canvas;
    if (!clone.get()) return NULL;
    clone->initializeSG();
    clone->setMatrixRotation(this->getMatrixRotation());
    clone->setMatrixTranslation(this->getMatrixTranslation());
    clone->setName(this->getName());

    for (auto i=0; i<m_selectedGroup.getSize(); ++i){
        entity::Entity2D* entcopy = m_selectedGroup.getEntity(i);;
        if (!entcopy) continue;
        switch(entcopy->getEntityType()){
        case cher::ENTITY_STROKE:{
            // copy the stroke
            entity::Stroke* stroke = dynamic_cast<entity::Stroke*>(entcopy);
            if (stroke){
                if (!clone->addEntity(stroke))
                    qWarning("canvas separate: could not copy stroke");
            }
            break;
        }
        case cher::ENTITY_PHOTO:
            break;
        default:
            break;
        }
    }
    clone->updateFrame();

    return clone.release();
}

// updates internals from m_r and m_t
void entity::Canvas::updateTransforms()
{
    /* update how the drawables look */
    osg::Matrix M = m_mR * m_mT;
    m_transform->setMatrix(M);

    /* update plane parameters */
    m_normal = cher::NORMAL;
    m_center = cher::CENTER;
    osg::Plane plane(m_normal, m_center);
    m_center = m_center * M;
    plane.transform(M);
    m_normal = plane.getNormal();
    if (!plane.valid()){
        qWarning("updateTransforms: Error while transforming internal canvas data. Virtual plane is not valid."
                  "Resetting plane parameters.");
        outLogVec("normal", plane.getNormal().x(), plane.getNormal().y(), plane.getNormal().z());
        this->resetTransforms();
    }
}

void entity::Canvas::resetTransforms()
{
    /* reset transform params */
    m_mR = osg::Matrix::rotate(0, cher::NORMAL);
    m_mT = osg::Matrix::translate(0,0,0);
    m_transform->setMatrix(m_mR * m_mT);

    /* reset plane params */
    m_normal = cher::NORMAL;
    m_center = cher::CENTER;
    osg::Plane plane(m_normal, m_center);
    m_center = m_center * m_mR * m_mT;
    plane.transform(m_mR * m_mT);
    m_normal = plane.getNormal();
    if (!plane.valid()){
        qFatal("resetTrandforms: failed.");
    }
}

// FIXME: should also take m_selectedGroup->m_theta so that rotation axis are rotated
void entity::Canvas::setVertices(const osg::Vec3f &center, float szX, float szY, float szCr, float szAx)
{
    m_toolFrame->setVertices(center, szX, szY, szCr, szAx,
                             m_selectedGroup.getCenter2DCustom(),
                             m_selectedGroup.getRotationAngle(),
                             m_selectedGroup.isEmpty());
}

void entity::Canvas::setVerticesDefault(const osg::Vec3f &center)
{
    m_toolFrame->setVertices(center, cher::CANVAS_MINW, cher::CANVAS_MINH, cher::CANVAS_CORNER,
                             cher::CANVAS_AXIS);
}

void entity::Canvas::setIntersection(entity::Canvas *against)
{
    osg::Vec3f P1=cher::CENTER, P2=cher::CENTER, P3=cher::CENTER, P4=cher::CENTER;
    if (against){
        if (Utilities::getCanvasesIntersection(this, against, P1,P2,P3,P4) != 2) return;
    }
    m_toolFrame->setIntersection(P1,P2,P3,P4);
}

void entity::Canvas::initializeProgramStroke()
{
    if (!m_programStroke) throw std::runtime_error("initializeProgramStroke(): Stroke shader program is not initialized.");
    if (!m_geodeStrokes) throw std::runtime_error("initializeProgramStroke() : Stroke geode is not initialized");

    /* use shader program */
    m_programStroke->initialize(m_geodeStrokes->getOrCreateStateSet(),
                                MainWindow::instance().getCamera(),
                                this->getTransform(),
                                MainWindow::instance().getStrokeFogFactor());

    /* set up program as state set attribute for stroke geode
     * This allows us to turn on or off the shader if neeeded, and it also requries only 1 shader per canvas vs. 1 shader per stroke. */
    m_geodeStrokes->getOrCreateStateSet()->setAttributeAndModes(m_programStroke.get(),
                                                                osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
}

const entity::FrameTool *entity::Canvas::getToolFrame() const
{
    return m_toolFrame;
}

unsigned int entity::Canvas::getNumEntities() const
{
    return m_geodeStrokes->getNumChildren() + m_geodePhotos->getNumChildren() + m_geodePolygons->getNumChildren();
}

/* returnds total number of photos that belong to the canvas */
unsigned int entity::Canvas::getNumPhotos() const
{
    return m_geodePhotos->getNumChildren();
}

unsigned int entity::Canvas::getNumStrokes() const
{
    return m_geodeStrokes->getNumChildren();
}

unsigned int entity::Canvas::getNumPolygons() const
{
    return m_geodePolygons->getNumChildren();
}

entity::Photo *entity::Canvas::getPhoto(int row) const
{
    if (row<0 || row>=static_cast<int>(m_geodePhotos->getNumChildren())) return NULL;
    return dynamic_cast<entity::Photo*>(m_geodePhotos->getChild(row));
}

entity::Stroke *entity::Canvas::getStroke(int i) const
{
    if (i<0 || i>=static_cast<int>(this->getNumStrokes())) return NULL;
    return dynamic_cast<entity::Stroke*>(m_geodeStrokes->getChild(i));
}

entity::Entity2D *entity::Canvas::getEntity(unsigned int i) const
{
    if (i>=this->getNumEntities()) return NULL;

    /* requested entity is a stroke */
    if (i<m_geodeStrokes->getNumChildren())
        return dynamic_cast<entity::Entity2D*> (m_geodeStrokes->getDrawable(i));
    /* requested entity is a photo */
    else if (i>=m_geodeStrokes->getNumChildren() &&  i<m_geodeStrokes->getNumChildren() + m_geodePhotos->getNumChildren()){
        Q_ASSERT(int(i)-int(m_geodeStrokes->getNumChildren()) >= 0 && int(i)-int(m_geodeStrokes->getNumChildren()) < int(m_geodeStrokes->getNumChildren()));
        return dynamic_cast<entity::Entity2D*>(m_geodePhotos->getDrawable(i-m_geodeStrokes->getNumChildren()));
    }
    /* rquested entity is a polygon */
    else {
        return dynamic_cast<entity::Entity2D*>(m_geodePolygons->getDrawable(i-m_geodeStrokes->getNumChildren()-m_geodePhotos->getNumChildren()));
    }
}

bool entity::Canvas::addEntity(entity::Entity2D *entity)
{
    bool result = false;
    if (!entity) return result;

    switch(entity->getEntityType()){
    case cher::ENTITY_STROKE:
        result = m_geodeStrokes->addDrawable(entity);
        break;
    case cher::ENTITY_PHOTO:        
        result = m_geodePhotos->addDrawable(entity);
        break;
    case cher::ENTITY_POLYGON:
        result = m_geodePolygons->addDrawable(entity);
        break;
    default:
        break;
    }

    return result;
}

bool entity::Canvas::removeEntity(entity::Entity2D *entity)
{
    bool result = false;
    if (!entity) return result;

    switch(entity->getEntityType()){
    case cher::ENTITY_STROKE:
        /* remove from scene graph */
        result = m_geodeStrokes->removeDrawable(entity);
        break;
    case cher::ENTITY_PHOTO:
        result = m_geodePhotos->removeDrawable(entity);
        break;
    case cher::ENTITY_POLYGON:
        result = m_geodePolygons->removeDrawable(entity);
        break;
    default:
        break;
    }

    return result;
}

bool entity::Canvas::containsEntity(entity::Entity2D *entity) const
{
    return (m_geodeStrokes->containsDrawable(entity) || m_geodePhotos->containsDrawable(entity) || m_geodePolygons->containsDrawable(entity));
}

REGISTER_OBJECT_WRAPPER(Canvas_Wrapper
                        , new entity::Canvas
                        , entity::Canvas
                        , "osg::Object osg::Group entity::Canvas")
{
    ADD_MATRIX_SERIALIZER(MatrixRotation, osg::Matrix());
    ADD_MATRIX_SERIALIZER(MatrixTranslation, osg::Matrix());

    ADD_OBJECT_SERIALIZER(Transform, osg::MatrixTransform, NULL);
    ADD_OBJECT_SERIALIZER(Switch, osg::Switch, NULL);
    ADD_OBJECT_SERIALIZER(GroupData, osg::Group, NULL);
    ADD_OBJECT_SERIALIZER(GeodeStrokes, osg::Geode, NULL);
    ADD_OBJECT_SERIALIZER(GeodePhotos, osg::Geode, NULL);
    ADD_OBJECT_SERIALIZER(GeodePolygons, osg::Geode, NULL);

    ADD_VEC3F_SERIALIZER(Center, osg::Vec3f());
    ADD_VEC3F_SERIALIZER(Normal, osg::Vec3f());
}

