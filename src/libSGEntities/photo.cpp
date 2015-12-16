#include "photo.h"

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/Image>

Photo::Photo(const std::string &fname)
    : _vertices(new osg::Vec3Array)
    , _normals(new osg::Vec3Array)
    , _texcoords(new osg::Vec2Array)
    , _texture(new osg::Texture2D)
{
    _vertices->push_back( osg::Vec3(-0.5f, -0.5f, 0.f) );
    _vertices->push_back( osg::Vec3( 0.5f, -0.5f, 0.f) );
    _vertices->push_back( osg::Vec3( 0.5f, 0.5f, 0.f) );
    _vertices->push_back( osg::Vec3(-0.5f, 0.5f, 0.f) );

    _normals->push_back( osg::Vec3(0.0f, 0.0f, -1.f) );

    _texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
    _texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
    _texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
    _texcoords->push_back( osg::Vec2(1.0f, 0.0f) );

    this->setVertexArray(_vertices.get());
    this->setNormalArray(_normals.get());
    this->setNormalBinding(osg::Geometry::BIND_OVERALL);
    this->setTexCoordArray(0, _texcoords.get());
    this->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

    osg::Image* image = osgDB::readImageFile(fname);
    _texture->setImage(image);
}

osg::Texture2D *Photo::getTexture() const
{
    return _texture.get();
}
