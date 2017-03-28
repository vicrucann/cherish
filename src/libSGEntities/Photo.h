/* Photo
 * for more info on textures, see OpenSceneGraph beginner's guide,
 * Chapter 6 "The basis of texture mapping"
 */

#ifndef PHOTO
#define PHOTO

#include "Entity2D.h"

#include <osg/Geometry>
#include <osg/ref_ptr>
#include <osg/Texture2D>
#include <osgDB/ObjectWrapper>
#include <SVMData.h>

namespace entity {

/*! \class Photo
 * \brief Quad that uses texture to represent a 2D photo in 3D space.
*/
class Photo: public entity::Entity2D{
public:
    /*! Default constructor, creates empty object and initializes OpenGL states. */
    Photo();

    /*! Constructor which is only used for OSG factorization. Not used otherwise. */
    Photo(const Photo& photo, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Node(entity, Photo)

    void setTexture(osg::Texture2D* texture);
    const osg::Texture2D* getTexture() const;

    void setWidth(float w);
    float getWidth() const;

    void setHeight(float h);
    float getHeight() const;

    void setCenter(const osg::Vec3f& c);
    const osg::Vec3f& getCenter() const;

    void setAngle(float a);
    float getAngle() const;

    void loadImage(const std::string& fname);
    osg::StateAttribute* getTextureAsAttribute() const;

    /*! A method to change location of the Photo center.
     * \param u is a new local U coordinate,
     * \param v is a new local V coordinate. */
    void move(const double u, const double v);

    /*! A method to change photo location within a canvas by using delta movement.
     * \param du is delta movement in local U coordinate,
     * \param dv is delta movement in local V coordinate. */
    void moveDelta(double du, double dv);

    /*! A method to change photo orientation within its canvas.
     * \param angle is the new rotation angle from positive U coordinate and around photo center, in radians. */
    void rotate(double angle);

    /*! A method to change photo orientation around a specified photo center.
     * \param theta is the rotation angle, in radians.
     * \param center is the local coordinates of new photo center around which the rotation will be performed. */
    void rotate(double theta, osg::Vec3f center);

    /*! A method to perform a horizontal flip. */
    void flipH();

    /*! A method to perform a vertical flip. */
    void flipV();

    /*! A method to scale photo quad uniformly around specified center. */
    void scale(double scale, osg::Vec3f center);

    /*! A method to scale both directions of photo quad independently around specified center. */
    void scale(double scaleX, double scaleY, osg::Vec3f center);

    /*! A method to perform automatic re-scaling so that whole photo is fit within current camera view.
     * \param normal is the photo plane's normal vector.
     * \param C is an arbitrary point within the photo plane.
     * \return true if re-scaling was performed, false - otherwire (e.g., when eye direction is parallel to photo plane).*/
    bool scaleWithinViewport(const osg::Plane& plane, const osg::Vec3f& C, const osg::Matrix& invM);

    /*! A method to perform auto re-scaling and re-positionning by using SVMData and specifed camera position. */
    void scaleAndPositionWith(const entity::SVMData* svm, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

    /*! A method to set up quad color, e.g., when performing selection. */
    void setColor(const osg::Vec4f& color);

    /*! \return geometry color. Assumed it is binded per geometry. */
    const osg::Vec4f& getColor() const;

    /*! A method to set up quad transparency. */
    void setTransparency(float alpha);

    /*! \return quad's transparency level. */
    float getTransparency() const;

    /*! \return type of entity::Entity2D. */
    cher::ENTITY_TYPE getEntityType() const;

protected:
    void updateVertices();

private:
    osg::ref_ptr<osg::Texture2D> m_texture; /*!< Photo texture container. */
    osg::Vec3f m_center; /*!< local center. */
    float m_width, m_height; /*!< half-width and half-height of the photo quad. */
    float m_angle;
    osg::Vec4f m_color;
};
}

#endif // PHOTO

