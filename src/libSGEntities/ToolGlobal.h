#ifndef TOOLGLOBAL_H
#define TOOLGLOBAL_H

#include <vector>
#include <iostream>
#include <stdio.h>

#include <osg/Group>
#include <osg/Switch>
#include <osg/AutoTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Camera>

#include "Settings.h"
#include "ProtectedGroup.h"

namespace entity {

/*! \class ToolGlobal
 * \brief An abstract class that denotes tool functionality and scene graph.
 * The base tool entity has a starter scene graph structure, however, it is assumed that inherited class
 * will redefine and fill the gaps within that structure.
 * The base structure is as follows:
 *
 *  ToolGlobal -> Switch -> ...     WireGeode
 *
 * While the wire geode is initialized, it is not a part of the scene graph yet. The inhereted class are assumed
 * to connect the dots between the switch and the wire geode.
 *
 *
*/
class ToolGlobal : public osg::ProtectedGroup
{
public:
    /*! Default constructor.
     * \param nVerts is the number of vertices that basic geometry will have
     * \param colorBind indicates whether the color bind is per vertex, or overall
     * \param primitiveSet is the type of geometry used, e.g., polilyne
     * \param linewidth is not mandatary, but indicates the line width of a geometry. It is set to 3.f by default. */
    ToolGlobal(int nVerts, osg::Array::Binding colorBind,
         osg::PrimitiveSet* primitiveSet, float linewidth=3.f);

protected:
    /*! A method that assures full initialization of a scene graph, i.e., provide a filling structure
     * between a switch and a wire geode variables. Must be re-defined in inhereted classes. */
    virtual void initializeSG() = 0;

public:
    /*! A setter method to set vertex array of wire geode. */
    virtual void setVertices(const std::vector<osg::Vec3f>& source);

    /*! A getter method to obtain a pointer on vertex array of a wire geode. */
    virtual const osg::Vec3Array* getVertices() const;

    /*! A method to set a specify color to a wire geometry. */
    virtual void setColor(const osg::Vec4f& color);

    /*! A method to obtain a color of the wire geometry. */
    virtual const osg::Vec4f& getColor() const;

    /*! A method to set visibility of the whole tool. */
    virtual void setVisibility(bool on) = 0;

    /*! A method to obtain visibility of the whole tool. */
    virtual bool getVisibility() const = 0;

    /*! A method to obtain a pointer on wire geode. */
    osg::Geode* getGeodeWire() const;

    /*! A method to obtain local 2D center of the wire geometry. It is assumed to be used within entity::Canvas tool mainly. */
    osg::Vec3f getCenterLocal() const;

protected:
    void updateGeometry();
    void updateGeometry(osg::Geometry* geom);

    osg::Switch* m_switch;  /*!< switch is responsible for visibility of the tool */
    osg::Geode* m_geodeWire;  /*!< contains all the geometry and color properties */
    osg::Geometry* m_geomWire; /*!< is contained in m_geodeWire and contains all the geometries */
};

/*! \class BookmarkTool
 * \brief Tool to depict the saved position of a camera and its associated scene state.
 * The BookmarkTool has the following scene graph structure:
 *
 *      BookmarkTool -> Switch -> AutoTransform -> GeodeWire
 *
 * The AutoTransform helps to make sure the bookmark's tool scale does not depend on the view point, i.e.,
 * the size remains constant disregard how far the current view is located. It is working in the same manner as
 * the entity::AxisGlobalTool.
*/
class BookmarkTool : public ToolGlobal
{
public:
    /*! Default constructor. In order to initialize auto-transform, it is necessary to pass the current view camera position
     * which has {eye, center, up} representation:
     * \param eye is the current view's eye parameter
     * \param center is the current view's center parameter
     * \param up is the current view's up parameter */
    BookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);

protected:
    /*! A method that provide scene graph initialization. */
    void initializeSG();

public:
    /*! A method to edit the tool's position. */
    void setPose(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);

    void setColorDefault();

    void setColorSelected();

protected:
    /*! A method that uses internal camer parameters (eye, center, up) and sets up the vertices position
     * for the tool.*/
    void updatePosition();

public:
    /*! A method to set visibility of a bookmark tool. */
    void setVisibility(bool on);

    /*! A method to obtain visibility of a bookmark tool. */
    bool getVisibility() const;

private:
    osg::AutoTransform* m_AT;

    osg::Vec3f m_eye, m_center, m_up;
};

/*! \class AxisGlobalTool
 * \brief A tool to depict position of global axis X, Y and Z.
 * The tool has the following scene graph structure:
 *
 *      AxisGlobalTool -> Switch -> Camera -> AutoTransform -> GeodeWire
 *
 * The AutoTransform is to assure the global axis tool size remains constand disregarding the current camera view.
 * The Camera node is necessary in order to set the rendering order to post order, so that the axis tool is
 * always rendered on the top of any other elements.
*/
class AxisGlobalTool : public ToolGlobal
{
public:
    /*! Default contructor. All the default sizing is taken from Settings file. */
    AxisGlobalTool();

protected:
    /*! A method to initialize the scene graph as shown on the class description. */
    void initializeSG();

public:
    /*! A method to set tool's visibility. */
    void setVisibility(bool on);

    /*! A method to obtain tool's visibility. */
    bool getVisibility() const;

    /*! A method to set color is re-defined since each axis has different color.
     * \param c1 is the color for axis X
     * \param c2 is the color for axis Y
     * \param c3 is the color for axis Z */
    virtual void setColor(const osg::Vec4f c1, const osg::Vec4f c2, const osg::Vec4f c3);

private:
    osg::AutoTransform* m_AT;
    osg::Camera* m_camera;
};

/*! \class ATGeode
 * \brief Helper class to create non-scalable geometries, e.g., AT -> Geode -> Geometry.
 * Used for pickable geometry.
*/
class ATGeode : public osg::AutoTransform
{
public:
    /*! Constructor initializes the internal scene graph structure and sets up the parameters of the auto scaling. */
    ATGeode();

protected:
    friend class FrameTool;

    osg::Geode* geode;
    osg::Geometry* geometry;
};

/*! \class ATRGeode
 * \brief Helper class to create non-scalable geometries that always render on top.
 * The node has the following scene graph structure: AT-> Camera -> Geode -> Geometry.
*/
class ATRGeode : public osg::AutoTransform
{
public:
    /*! Constructor initializes the internal scene graph structure and sets up parameters of auto scale and camera. */
    ATRGeode();

protected:
    friend class FrameTool;

    osg::Geode* geode;
    osg::Geometry* geometry;
    osg::Camera* camera;

};

/*! \class FrameTool
 * \brief A tool to depict canvas bounding box in both its non-editable and editable states.
 *
 * The canvas tool has three main modes:
 *
 * 1. Non-editable
 * 2. 2D selection editable
 * 3. 3D position editable
 *
 * Only certain scene graph elements are used within each of the canvas modes. The canvas frame tool has the following scene graph structure:
 *
 *      FrameTool -> Switch -> GeodeWire
 *                          |-> GeodeIntersect
 *                          |-> AT_Pickable
 *                          |-> AT_Center (scales, axis)
 *                          |-> GeodeNormal
 *                          |-> GeodeRotation
 *
 * Each geode represents a single functionality / element within the canvas frame:
 *
 * * GeodeWire is the non-editable canvas wire
 * * GeodeIntersect contains the geometry of two canvas intersection (normally would contained within the previous canvas).
 * * AT_Pickbale contains a selectable quad of a non-editable canvas, and it only allows changing the state of a canvas, e.g.,
 * from current to previous.
 * * AT_Center etc. are the quads which appear when the canvas is in 2D-editable mode and they allow to change the scale of the 2D
 * selection of the canvas.
 * * GeodeNormal is the canvas normal representation which appears when the canvas is in 3D-editable mode. Dragging along the
 * normals allows to edit canvas offset along the normal.
 * * GeodeRotation is a canvas wireframe representation which appears when the canvas is in 3D-editable mode. It allows changing
 * a canvas location by rotation along local U or local V axis.
 *
*/
class FrameTool : public ToolGlobal
{
public:
    /*! Default constructor that does scene graph initialization and pulls all the default values from cherish Settings file. */
    FrameTool();

protected:
    /*! Scene graph initialization is performed as shown in the class description. */
    void initializeSG();

public:
    /*! A method to set canvas frame visibility. When visibility is set to off, the visibility values are saved. When the
     * visibility is turned back on, the values are poped back to their original states.
     * \param on is true when canvas frame is visible, false - otherwise. */
    void setVisibility(bool on);

    /*! A method to obtain a canvas frame visibility. */
    bool getVisibility() const;

    /*! A method that was re-defined from the abstract one. It is due to the fact that canvas geometry is not defined as straightforward as in other tools.
     * That is, the canvas bounding box has different look depending on the canvas mode: non-editable, 2D-editable or 3D-editable. This method is also used
     * when canvas bounding box dimensions are updated (called from entity::Canvas::updateFrame()).
     * \param center is the updated canvas center
     * \param szX is the updated canvas half width
     * \param szY is the updated canvas half height
     * \param szCr is the pickable size, normally it would stay constant
     * \param szAx is the length value of the canvas local axis (when canvas is in 2D-editable mode)
     * \param centerCustom is used when the canvas is in 2D editable mode to locate GeodeAxis position
     * \param thera is used when the canvas is in 2D-editable mode to locate GeodeAxis rotation
     * \param selectionIsEmpty is to denote what type of frame (non-editable/3D-editable vs. 2D-editable) will be used */
    void setVertices(const osg::Vec3f &center, float szX, float szY, float szCr, float szAx,
                     const osg::Vec3f& centerCustom = cher::CENTER ,
                     double theta = 0,
                     bool selectionIsEmpty = true);

    /*! A method to set color frame and canvas intersection color. */
    void setColor(const osg::Vec4f& color, const osg::Vec4f& colorIntersect = cher::CANVAS_CLR_PREVIOUS);

    /*! A mthod to set intersection geometry */
    void setIntersection(const osg::Vec3f &P1, const osg::Vec3f &P2, const osg::Vec3f &P3, const osg::Vec3f &P4);

protected:
    /*! A separate method to set intersection color */
    void setColorIntersection(const osg::Vec4f& colorIntersect);

public:
    /*! A method to set if the canvas frame is 3D-editable or not. */
    void setEditable(bool editable);

    /*! A getter method to obtain a pointer on pickable geometry. Used from EventHandler when user is trying to pick current canvas by its pickable. */
    const osg::Geometry* getPickable() const;

    /*! \return true if there are any selected 2D entities within the canvas (whether the canvas is in 2D-editable mode). */
    bool isSelected() const;

    /*! A method to perform delta movement when the canvas is in 2D-editable mode. */
    virtual void moveDelta(double du, double dv);

    /*! A method to preform non-uniform scale of selected entities when the canvas is in 2D-editable mode. */
    virtual void scale(double scaleX, double scaleY, osg::Vec3f center);

    /*! A method to perform uniform scale around fixed center when the canvas is in 2D-editable mode. */
    virtual void scale(double scale, osg::Vec3f center);

    /*! A methog to perform a rotation of selected 2D entities within a canvas when the canvas is in 2D-editable mode. */
    virtual void rotate(double theta, osg::Vec3f center);

protected:
    void initQuadGeometry(osg::Geometry* geom, const std::string& name = "");
    void initLineGeometry(osg::Geometry* geom, float lineWidth, const std::string& name = "");

    void setQuadGeometry(osg::Geometry* geom, const osg::Vec3f& P, float szX, float szY, float theta = 0, const osg::Vec3f& center = cher::CENTER);

    /*! A method to set up m_AT_scale geometries. */
    void setScaleGeometry(osg::Geometry* geom, const osg::Vec3f& P0, const osg::Vec3f& P1, const osg::Vec3f& P2, const osg::Vec3f& P3);

    void setLineGeometry(osg::Geometry* geom, const osg::Vec3f& P1, const osg::Vec3f& P2);

    void setColorGeometry(osg::Geometry* geom, const osg::Vec4f& color);

    void moveDeltaWireGeometry(osg::Geometry*geometry, double du, double dv);
    void scaleWireGeometry(osg::Geometry* geometry, double scaleX, double scaleY, osg::Vec3f center);
    void scaleWireGeometry(osg::Geometry* geometry, double scale, osg::Vec3f center);
    void rotateWireGeometry(osg::Geometry* geometry, double theta, osg::Vec3f center);

private:
    osg::Geode* m_geodeIntersect, * m_geodeNormal, * m_geodeRotation;
    osg::Geometry* m_geomIntersect;

    /* canvas offset and 3d rotation drawables */
    osg::Geometry * m_geomNormal1, * m_geomNormal2; /*!< canvas offset geomtries */
    osg::Geometry * m_geomRotateX1, * m_geomRotateX2, * m_geomRotateY1, * m_geomRotateY2; /*!< canvas 3d rotation geometries */

    ATGeode* m_AT_pick; /*!< pickable scene graph: AT -> geode -> geometry */
    ATRGeode* m_AT_scaleUV1, * m_AT_scaleUV2, * m_AT_scaleUV3, * m_AT_scaleUV4; /*!< tools to scale 2D entities */
    ATRGeode* m_AT_center; /*!< 2D entities translation */
    ATRGeode* m_AT_axisU, * m_AT_axisV; /*!< 2D entities rotations */

    bool m_selected; /*!< if there is selection present on scene */
    bool m_editable; /*!< if canvas is in edit mode: offset and rotation */
    bool m_visible; /*!< canvas frame visibility flag */

    std::vector<bool> m_visibilityState;
};

}


#endif // TOOLGlobal_H
