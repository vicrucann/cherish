/* Photo
 * for more info on textures, see OpenSceneGraph beginner's guide,
 * Chapter 6 "The basis of texture mapping"
 */

#ifndef PHOTO
#define PHOTO

#include <osg/Geometry>
#include <osg/ref_ptr>
#include <osg/Texture2D>

class Photo: public osg::Geometry{
public:
    Photo(const std::string& fname);
    osg::Texture2D* getTexture() const;
    void setFrameColor(const osg::Vec4 color);
    void setModeEdit(bool edit);
    void move(const double u, const double v);
private:
    osg::ref_ptr<osg::Vec3Array> _vertices;
    osg::ref_ptr<osg::Vec3Array> _normals;
    osg::ref_ptr<osg::Vec2Array> _texcoords;
    osg::ref_ptr<osg::Texture2D> _texture;

    float _width, _height;
};

#endif // PHOTO

