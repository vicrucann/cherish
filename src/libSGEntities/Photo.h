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

    void move(const double u, const double v);
    void moveDelta(double du, double dv);
    void rotate(double angle);
    void rotate(double theta, osg::Vec3f center);
    void flipH();
    void flipV();
    void scale(double scale, osg::Vec3f center);
    void scale(double scaleX, double scaleY, osg::Vec3f center);
    void scaleAndPositionWith(const entity::SVMData* svm, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);
    void setColor(const osg::Vec4f& color);
    void setTransparency(float alpha);
    float getTransparency() const;

    cher::ENTITY_TYPE getEntityType() const;

protected:
    void updateVertices();

private:
    osg::ref_ptr<osg::Texture2D> m_texture; /*!< Photo texture container. */
    osg::Vec3f m_center; /*!< local center. */
    float m_width, m_height; /*!< half-width and half-height of the photo quad. */
    float m_angle;
};
}

#endif // PHOTO

