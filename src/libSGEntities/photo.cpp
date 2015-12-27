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
    , _width(dureu::PHOTO_MINW)
    , _height(dureu::PHOTO_MINH)
{
    osg::Image* image = osgDB::readImageFile(fname);
    float aspectRatio = static_cast<float>(image->s()) / static_cast<float>(image->t());
    outLogVal("Image height", image->t());
    outLogVal("Image width", image->s());

    // half sizes based on aspect ratio
    _width = 2.f * (dureu::PHOTO_MINW);
    _height = _width * aspectRatio;
    outLogVal("Photo(): width", _width);
    outLogVal("Photo(): height", _height);
    _vertices->push_back( osg::Vec3(_width/2.f, _height/2.f, 0.f) );
    _vertices->push_back( osg::Vec3(-_width/2.f, _height/2.f, 0.f) );
    _vertices->push_back( osg::Vec3(-_width/2.f, -_height/2.f, 0.f) );
    _vertices->push_back( osg::Vec3(_width/2.f, -_height/2.f, 0.f) );

    _normals->push_back( dureu::NORMAL );

    // we use W because the image was scaled to be normalized
    _texcoords->push_back( osg::Vec2(_width, _width) );
    _texcoords->push_back( osg::Vec2(_width, 0.0f) );
    _texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
    _texcoords->push_back( osg::Vec2(0.0f, _width) );

    this->setVertexArray(_vertices.get());
    this->setNormalArray(_normals.get());
    this->setNormalBinding(osg::Geometry::BIND_OVERALL);
    this->setTexCoordArray(0, _texcoords.get());
    this->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

    _texture->setImage(image);
    this->setFrameColor(dureu::PHOTO_CLR_REST);
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

void Photo::setModeEdit(bool edit)
{
    if (edit)
        this->setFrameColor(dureu::CANVAS_CLR_EDIT);
    else
        this->setFrameColor(dureu::PHOTO_CLR_REST);
}

void Photo::move(const double u, const double v)
{
    osg::Vec3f center = osg::Vec3f(u,v,0.f);
    (*_vertices)[0] = center + osg::Vec3(_width/2.f, _height/2.f, 0.f);
    (*_vertices)[1] = center + osg::Vec3(-_width/2.f, _height/2.f, 0.f);
    (*_vertices)[2] = center + osg::Vec3(-_width/2.f, -_height/2.f, 0.f);
    (*_vertices)[3] = center + osg::Vec3(_width/2.f, -_height/2.f, 0.f);
    this->dirtyDisplayList();
    this->dirtyBound();
}
