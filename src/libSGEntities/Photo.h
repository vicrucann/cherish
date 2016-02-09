/* Photo
 * for more info on textures, see OpenSceneGraph beginner's guide,
 * Chapter 6 "The basis of texture mapping"
 */

#ifndef PHOTO
#define PHOTO

#include <osg/Geometry>
#include <osg/ref_ptr>
#include <osg/Texture2D>
#include <osgDB/ObjectWrapper>

namespace entity {
class Photo: public osg::Geometry{
public:
    Photo();
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

    void setFrameColor(const osg::Vec4 color);
    void setModeEdit(bool edit);
    bool getModeEdit() const;

    void move(const double u, const double v);
    void rotate(double angle);
    void flipH();
    void flipV();
    void scale(double timesX, double timesY);

protected:
    void updateVertices();

private:
    osg::ref_ptr<osg::Texture2D> m_texture;
    osg::Vec3f m_center;
    float m_width, m_height;
    float m_angle;
    bool m_edit;
};
}

#endif // PHOTO

