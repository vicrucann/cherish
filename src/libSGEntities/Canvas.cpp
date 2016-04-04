#include <assert.h>
#include <iostream>

#include "Canvas.h"
#include "Settings.h"
#include "Stroke.h"
#include "FindNodeVisitor.h"
#include "Utilities.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/BoundingBox>
#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/Plane>
#include <osg/BlendFunc>
#include <osg/BoundingSphere>

entity::Canvas::Canvas()
    : osg::Group()
    , m_mR(osg::Matrix::rotate(0, dureu::NORMAL))
    , m_mT(osg::Matrix::translate(0,0,0))
    , m_transform(new osg::MatrixTransform(m_mR * m_mT))
    , m_switch(new osg::Switch)
    , m_geodeData(new osg::Geode)

    , m_toolFrame(new entity::FrameTool)

    , m_strokeCurrent(0)
//    , m_selectedGroup(0)

    , m_center(osg::Vec3f(0.f,0.f,0.f)) // moves only when strokes are introduced so that to define it as centroid
    , m_normal(dureu::NORMAL)
    , m_edit(false)
    , m_rotaxis(osg::Vec3f(0.f, 1.f, 0.f))
{
    /* OpenGL state machine for the canvas */
    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(dureu::CANVAS_LINE_WIDTH);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    //blendfunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ANTIALIAS);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    this->setStateSet(stateset);

    /* name settings */
    m_transform->setName("Transform");
    m_switch->setName("Switch");

    outLogMsg("New Canvas ctor complete");
}

entity::Canvas::Canvas(const entity::Canvas& cnv, const osg::CopyOp& copyop)
    : osg::Group(cnv, copyop)
    , m_mR(cnv.m_mR)
    , m_mT(cnv.m_mT)
    , m_transform(cnv.m_transform)
    , m_switch(cnv.m_switch)
    , m_geodeData(cnv.m_geodeData)

    , m_toolFrame(cnv.m_toolFrame)

    , m_strokeCurrent(0)
//    , m_selectedGroup(0)

    , m_center(cnv.m_center)
    , m_normal(cnv.m_normal)
    , m_edit(cnv.m_edit)
    , m_rotaxis(osg::Vec3f(0.f, 1.f, 0.f))
{
    outLogMsg("new Canvas by copy ctor complete");
}

void entity::Canvas::initializeTools()
{
    outLogMsg("Canvas tools initialization");
    if (m_switch->getNumChildren() == 0){
        outLogMsg("  from scratch");
        m_switch->addChild(m_toolFrame, true);
        outLogMsg("canvas tools added");
    }
    /* remove all but geode data */
    else if (m_switch->getNumChildren() == 2 || m_switch->getNumChildren() == 1 ){
        osg::Node* tfn = this->getTool("groupFrame");
        if (tfn) m_switch->replaceChild(tfn, m_toolFrame);
        else outErrMsg("framel node is null, construction tools are not initialized");
        this->updateFrame();
    }
    else
        outErrMsg("Canvas tool init: switch does not contain neither 0 nor 4 children. "
                  "Tools were not initialized.");
}

/* Method to initialize canvases' geometrical properties
 * must be called from AddCanvasCommand right after the canvas
 * allocated.
*/
void entity::Canvas::initializeSG()
{
    this->addChild(m_transform.get());
    m_transform->setName("Transform");
    m_transform->addChild(m_switch.get());
    m_switch->setName("Switch");

    this->initializeTools();
    /* _geodeData is  empty, it is for user input: strokes */
    m_switch->addChild(m_geodeData.get(), true);

    this->updateTransforms();
    this->setColor(dureu::CANVAS_CLR_REST);
    this->setVertices(m_center, dureu::CANVAS_MINW, dureu::CANVAS_MINH, dureu::CANVAS_CORNER, dureu::CANVAS_AXIS);
}

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

void entity::Canvas::setGeodeData(osg::Geode* geode)
{
    m_geodeData = geode;
}

const osg::Geode* entity::Canvas::getGeodeData() const
{
    return m_geodeData.get();
}

osg::Geode*entity::Canvas::getGeodeData()
{
    return m_geodeData.get();
}

void entity::Canvas::setCenter(const osg::Vec3f &center)
{
    m_center = center;
}

const osg::Vec3f& entity::Canvas::getCenter() const
{
    return m_center;
}

void entity::Canvas::setNormal(const osg::Vec3f &normal)
{
    m_normal = normal;
}

const osg::Vec3f& entity::Canvas::getNormal() const
{
    return m_normal;
}

void entity::Canvas::setColor(const osg::Vec4f &color)
{
    m_toolFrame->setColor(color);
}

const osg::Vec4f &entity::Canvas::getColor() const
{
    return m_toolFrame->getColor();
}

void entity::Canvas::setRotationAxis(const osg::Vec3f &axis)
{
    outLogVec("canvas setting rotation axis to", axis.x(), axis.y(), axis.z());
    m_rotaxis = axis;
}

const osg::Vec3f &entity::Canvas::getRotationAxis() const
{
    return m_rotaxis;
}

void entity::Canvas::setVisibilityFrame(bool vis)
{
    m_toolFrame->setVisibility(vis);
}

bool entity::Canvas::getVisibility() const{
    return m_toolFrame->getVisibility();
}

void entity::Canvas::setVisibilityAll(bool vis)
{
    m_switch->setChildValue(m_geodeData, vis);
    this->setVisibilityFrame(vis);
}

bool entity::Canvas::getVisibilityData() const
{
    return m_switch->getChildValue(m_geodeData);
}

/*  V
 *  |
 *  |____ U
 *  /
 * normal
 */
osg::Vec3f entity::Canvas::getGlobalAxisU() const
{
    osg::Vec3f u_loc = osg::Vec3f(1,0,0);
    osg::Matrix M = m_transform->getMatrix();
    return M * u_loc;
}

/*  V
 *  |
 *  |____ U
 *  /
 * normal
 */
osg::Vec3f entity::Canvas::getGlobalAxisV() const
{
    osg::Vec3f v_loc = osg::Vec3f(0,1,0);
    osg::Matrix M = m_transform->getMatrix();
    return M * v_loc;
}

osg::Geometry *entity::Canvas::getGeometryPickable() const
{
    return m_toolFrame->getPickable();
}

// translates the current params on mt matrix
void entity::Canvas::translate(const osg::Matrix& mt)
{
    if (mt.isNaN()){
        outErrMsg("canvas-translate: translate matrix is NAN");
        return;
    }
    m_mT = m_mT * mt;
    this->updateTransforms();
}

// rotates the current params on mr matrix
void entity::Canvas::rotate(const osg::Matrix& mr)
{
    if (mr.isNaN()){
        outErrMsg("canvas-rotate: matrix is NAN");
        return;
    }
    m_mR = m_mR * mr;
    this->updateTransforms();
}

void entity::Canvas::unselectAll()
{
    this->setStrokeCurrent(false);
    this->unselectEntities();
}

void entity::Canvas::unselectEntities()
{
    m_selectedGroup.resetAll();
}

void entity::Canvas::selectAllStrokes()
{
    this->unselectEntities();
    for (unsigned int i = 0; i < m_geodeData->getNumChildren(); i++){
        entity::Entity2D* entity = dynamic_cast<entity::Entity2D*>(m_geodeData->getChild(i));
        if (!entity) continue;
        this->addEntitySelected(entity);
//        entity::Entity2D* entity = dynamic_cast<entity::Entity2D>(m_geodeData->getChild(i));
//        if (!entity) continue;
//        this->addEntitySelected(entity);
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

/* whenever new entity is added to selection,
 * update the selection frame;
 * if the entity is the first, then create the selection frame and
 * add it to the scene graph
 */
void entity::Canvas::addEntitySelected(Entity2D *entity)
{
    m_selectedGroup.addEntity(entity, m_geodeData.get());
}

/* whenever an entity is substracted from selection,
 * update the selection frame;
 * if no entities left, remove the selection frame from scene graph
*/
void entity::Canvas::removeEntitySelected(Entity2D *entity)
{
    m_selectedGroup.removeEntity(entity);
}

const std::vector<entity::Entity2D* >& entity::Canvas::getStrokesSelected() const
{
    return m_selectedGroup.getEntities();
}

int entity::Canvas::getStrokesSelectedSize() const
{
    return m_selectedGroup.getSize();
}

/* returns global center of selected entities;
 * if there is no selected entities, returns canvas center
*/
osg::Vec3f entity::Canvas::getStrokesSelectedCenter() const
{
    return m_selectedGroup.getCenter3D(m_transform->getMatrix());
}

void entity::Canvas::moveEntities(std::vector<entity::Entity2D *>& entities, double du, double dv)
{
    m_selectedGroup.move(entities, du, dv);
}

void entity::Canvas::moveEntitiesSelected(double du, double dv)
{
    m_selectedGroup.move(du, dv);
}

void entity::Canvas::scaleEntities(std::vector<Entity2D *> &entities, double s, osg::Vec3f center)
{
    m_selectedGroup.scale(entities, s,s,center);
}

void entity::Canvas::scaleEntitiesSelected(double s, osg::Vec3f center)
{
    m_selectedGroup.scale(s,s);
}

void entity::Canvas::rotateEntities(std::vector<Entity2D *> entities, double theta, osg::Vec3f center)
{
    m_selectedGroup.rotate(entities, theta, center);
}

void entity::Canvas::rotateEntitiesSelected(double theta, osg::Vec3f center)
{
    m_selectedGroup.rotate(theta);
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
    if (m_geodeData->getNumChildren() > 0){
        /* if there is a selected group, draw edit frame around it
         * the frame will allow to perform move, scale, rotate */
        if (m_selectedGroup.getSize() > 0){
            osg::BoundingBox bb = m_selectedGroup.getBoundingBox();
            if (bb.valid()){
                float szX = bb.xMax() - bb.xMin();
                float szY = bb.yMax() - bb.yMin();
                this->setVertices(m_selectedGroup.getCenter2D(),
                                  szX*0.5+dureu::CANVAS_EDITSLACK, szY*0.5+dureu::CANVAS_EDITSLACK,
                                  dureu::CANVAS_EDITQUAD, dureu::CANVAS_EDITAXIS);
            }
        }
        /* if there is no selection, draw a normal canvas frame */
        else{
            /* local bounding box */
            osg::BoundingBox bb = m_geodeData->getBoundingBox();
            if (bb.valid()){
                /* 2d <--> 2d transform matrices */
                osg::Matrix M = m_transform->getMatrix();
                osg::Matrix invM;
                if (invM.invert(M)){
                    /* old 2d local canvas center */
                    osg::Vec3f c2d_old = m_center * invM;
                    if (std::fabs(c2d_old.z()) > dureu::EPSILON){
                        outErrMsg("Warning: updateFrame(): local central point z-coord is not close to zero");
                        return;
                    }

                    /* new 2d local center */
                    osg::Vec3f c2d_new = bb.center();
                    if (std::fabs(c2d_new.z()) > dureu::EPSILON){
                        outErrMsg("Warning: updateFrame(): local central point z-coord is not close to zero");
                        return;
                    }

                    /* move every child back in local delta translation (diff between old and new centers) */
                    osg::Vec3f delta2d = c2d_old - c2d_new;
                    for (unsigned int i=0; i<m_geodeData->getNumChildren(); ++i){
                        entity::Entity2D* entity = dynamic_cast<entity::Entity2D*>(m_geodeData->getChild(i));
                        if (!entity){
                            outErrMsg("updateFrame: could not dynamic_cast to Entity2D*");
                            return;
                        }
                        entity->moveDelta(delta2d.x(), delta2d.y());
                    }

                    /* adjust the size of the canvas drawables in old location */
                    float dx = 0.5*(bb.xMax()-bb.xMin())+dureu::CANVAS_CORNER;
                    float dy = 0.5*(bb.yMax()-bb.yMin())+dureu::CANVAS_CORNER;
                    float szX = std::max(dx, dureu::CANVAS_MINW);
                    float szY = std::max(dy, dureu::CANVAS_MINW);
                    this->setVertices(c2d_old, szX, szY, dureu::CANVAS_CORNER, dureu::CANVAS_AXIS);

                    /* new global center coordinate and delta translate in 3D */
                    osg::Vec3f c3d_new = c2d_new * M;
                    osg::Vec3f delta3d = c3d_new - m_center;

                    /* move whole canvas and its drawables to be positioned at new global center */
                    this->translate(osg::Matrix::translate(delta3d.x(), delta3d.y(), delta3d.z()));
                } // M is invertable
            } // bb is valid
        } // else no selection

    } // geodeData has children
    this->setIntersection(against);
}

const osg::Vec3Array *entity::Canvas::getFrame() const
{
    return m_toolFrame->getVertices();
}

void entity::Canvas::setModeEdit(bool on)
{
    if (on){
        std::cout << "setModeEdit(): ON - " << on << std::endl;
        this->setColor(dureu::CANVAS_CLR_EDIT);
        this->setVisibilityAll(true);
    }
    else{
        std::cout << "setModeEdit(): OFF - " << on << std::endl;
        this->setColor(dureu::CANVAS_CLR_CURRENT);
    }
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

osg::MatrixTransform* entity::Canvas::getMatrixTransform() const
{
    return m_transform.get();
}

entity::Canvas *entity::Canvas::clone() const
{
    osg::ref_ptr<entity::Canvas> clone = new Canvas;
    if (!clone.get()) return NULL;
    clone->initializeSG();
    clone->setMatrixRotation(this->getMatrixRotation());
    clone->setMatrixTranslation(this->getMatrixTranslation());
    clone->setName(this->getName());

    for (unsigned int i=0; i<m_geodeData->getNumChildren(); ++i){
        entity::Entity2D* entcopy = dynamic_cast<entity::Entity2D*>(m_geodeData->getChild(i));
        switch(entcopy->getEntityType()){
        case dureu::ENTITY_STROKE:
        {
            entity::Stroke* stroke = dynamic_cast<entity::Stroke*>(m_geodeData->getChild(i));
            if (stroke){
                entity::Stroke* si = new entity::Stroke(*stroke, osg::CopyOp::DEEP_COPY_ALL);
                if (si){
                    if (!clone->getGeodeData()->addDrawable(si)) outErrMsg("canvas clone: could not add stroke as drawable");
                }
                else outErrMsg("canvas clone: coult not clone the stroke");
            }
            else outErrMsg("canvas clone: could not dynamic_cast<Stroke>");
            break;
        }
        case dureu::ENTITY_PHOTO:
            break;
        default:
            break;
        }
    }
    clone->updateFrame();

    return clone.release();
}

entity::Canvas::~Canvas()
{
}

// updates internals from m_r and m_t
void entity::Canvas::updateTransforms()
{
    /* update how the drawables look */
    osg::Matrix M = m_mR * m_mT;
    m_transform->setMatrix(M);

    /* update plane parameters */
    m_normal = dureu::NORMAL;
    m_center = osg::Vec3(0,0,0);
    osg::Plane plane(m_normal, m_center);
    m_center = m_center * M;
    plane.transform(M);
    m_normal = plane.getNormal();
    if (!plane.valid()){
        outErrMsg("updateTransforms: Error while transforming internal canvas data. Virtual plane is not valid."
                  "Resetting plane parameters.");
        outLogVec("normal", plane.getNormal().x(), plane.getNormal().y(), plane.getNormal().z());
        this->resetTransforms();
    }
}

void entity::Canvas::resetTransforms()
{
    /* reset transform params */
    m_mR = osg::Matrix::rotate(0, dureu::NORMAL);
    m_mT = osg::Matrix::translate(0,0,0);
    m_transform->setMatrix(m_mR * m_mT);

    /* reset plane params */
    m_normal = dureu::NORMAL;
    m_center = osg::Vec3(0,0,0);
    osg::Plane plane(m_normal, m_center);
    m_center = m_center * m_mR * m_mT;
    plane.transform(m_mR * m_mT);
    m_normal = plane.getNormal();
    if (!plane.valid()){
        outErrMsg("resetTrandforms: failed. Exiting application");
        exit(1);
    }
}

void entity::Canvas::setVertices(const osg::Vec3f &center, float szX, float szY, float szCr, float szAx)
{
    m_toolFrame->setVertices(center, szX, szY, szCr, szAx,
                             m_selectedGroup.getCenter2DCustom(), m_selectedGroup.isEmpty());
}

void entity::Canvas::setIntersection(entity::Canvas *against)
{
    osg::Vec3f P1=dureu::CENTER, P2=dureu::CENTER, P3=dureu::CENTER, P4=dureu::CENTER;
    if (against){
        if (Utilities::getCanvasesIntersection(this, against, P1,P2,P3,P4) != 2) return;
    }
    m_toolFrame->setIntersection(P1,P2,P3,P4);
}

osg::Node* entity::Canvas::getTool(const std::string &name)
{
    if (!m_switch.get()){
        outErrMsg("getTool: switch is NULL");
        return 0;
    }

    FindNodeVisitor fnv(name);
    m_switch->accept(fnv);
    if (fnv.getNode() == NULL){
        outErrMsg("getTool: FNV returned NULL");
        return 0;
    }
    return fnv.getNode();
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
    ADD_OBJECT_SERIALIZER(GeodeData, osg::Geode, NULL);

    ADD_VEC3F_SERIALIZER(Center, osg::Vec3f());
    ADD_VEC3F_SERIALIZER(Normal, osg::Vec3f());
}

