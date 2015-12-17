#include "photo.h"
#include "settings.h"

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/Image>

Photo::Photo(const std::string &fname)
    : _vertices(new osg::Vec3Array)
    , _normals(new osg::Vec3Array)
    , _texcoords(new osg::Vec2Array)
    , _texture(new osg::Texture2D)
{
    osg::Image* image = osgDB::readImageFile(fname);
    float aspectRatio = static_cast<float>(image->s()) / static_cast<float>(image->t());
    debugLogVal("Image height", image->t());
    debugLogVal("Image width", image->s());

    // half sizes based on aspect ratio
    float W = 2.f * (dureu::PHOTO_MINH);
    float H = W * aspectRatio;
    debugLogVal("Photo(): width", W);
    debugLogVal("Photo(): height", H);
    _vertices->push_back( osg::Vec3(W/2.f, H/2.f, 0.f) );
    _vertices->push_back( osg::Vec3(-W/2.f, H/2.f, 0.f) );
    _vertices->push_back( osg::Vec3(-W/2.f, -H/2.f, 0.f) );
    _vertices->push_back( osg::Vec3(W/2.f, -H/2.f, 0.f) );

    _normals->push_back( dureu::NORMAL );

    // we use W because the image was scaled to be normalized
    _texcoords->push_back( osg::Vec2(W, W) );
    _texcoords->push_back( osg::Vec2(W, 0.0f) );
    _texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
    _texcoords->push_back( osg::Vec2(0.0f, W) );

    this->setVertexArray(_vertices.get());
    this->setNormalArray(_normals.get());
    this->setNormalBinding(osg::Geometry::BIND_OVERALL);
    this->setTexCoordArray(0, _texcoords.get());
    this->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
    //this->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

    _texture->setImage(image);
}

osg::Texture2D *Photo::getTexture() const
{
    return _texture.get();
}

void Photo::setFrameColor(const osg::Vec4 color)
{
    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = color;
    (*colors)[1] = color;
    (*colors)[2] = color;
    (*colors)[3] = color;
    this->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}
