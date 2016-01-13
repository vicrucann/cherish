#include "Photo.h"
#include "settings.h"

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/Image>

entity::Photo::Photo()
    : osg::Geometry()
    , m_texture(new osg::Texture2D)
    , m_center(osg::Vec3f(0.f,0.f,0.f))
    , m_width(0)
    , m_height(0)
{
    this->setName("Photo");
    outLogMsg("New Photo ctor complete");
}

entity::Photo::Photo(const entity::Photo& photo, const osg::CopyOp& copyop)
    : osg::Geometry(photo, copyop)
    , m_texture(photo.m_texture)
    , m_center(photo.m_center)
    , m_width(photo.m_width)
    , m_height(photo.m_height)
{
}

/*entity::Photo::Photo(const std::string &fname)
    : osg::Geometry()
    , m_texture(new osg::Texture2D)
    , m_center(osg::Vec3f(0.f,0.f,0.f))
    , m_width(dureu::PHOTO_MINW)
    , m_height(dureu::PHOTO_MINH)
{
    osg::Image* image = osgDB::readImageFile(fname);
    float aspectRatio = static_cast<float>(image->s()) / static_cast<float>(image->t());
    outLogVal("Image height", image->t());
    outLogVal("Image width", image->s());

    // half sizes based on aspect ratio
    m_width = 2.f * (dureu::PHOTO_MINW);
    m_height = m_width * aspectRatio;
    outLogVal("Photo(): width", m_width);
    outLogVal("Photo(): height", m_height);

    osg::Vec3Array* verts = new osg::Vec3Array;
    verts->push_back(m_center + osg::Vec3(m_width/2.f, m_height/2.f, 0.f));
    verts->push_back(m_center + osg::Vec3(-m_width/2.f, m_height/2.f, 0.f));
    verts->push_back(m_center + osg::Vec3(-m_width/2.f, -m_height/2.f, 0.f));
    verts->push_back(m_center + osg::Vec3(m_width/2.f, -m_height/2.f, 0.f));

    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(dureu::NORMAL);

    // we use W because the image was scaled to be normalized
    osg::Vec2Array* texcoords = new osg::Vec2Array;
    texcoords->push_back( osg::Vec2(m_width, m_width) );
    texcoords->push_back( osg::Vec2(m_width, 0.0f) );
    texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
    texcoords->push_back( osg::Vec2(0.0f, m_width) );

    this->setVertexArray(verts);
    this->setNormalArray(normals);
    this->setTexCoordArray(0, texcoords);
    this->setNormalBinding(osg::Geometry::BIND_OVERALL);
    this->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

    m_texture->setImage(image);
    this->setFrameColor(dureu::PHOTO_CLR_REST);
}*/

void entity::Photo::setTexture(osg::Texture2D* texture)
{
    m_texture = texture;
}

const osg::Texture2D*entity::Photo::getTexture() const
{
    return m_texture.get();
}

void entity::Photo::setWidth(float w)
{
    m_width = w;
}

float entity::Photo::getWidth() const
{
    return m_width;
}

void entity::Photo::setHeight(float h)
{
    m_height = h;
}

float entity::Photo::getHeight() const
{
    return m_height;
}

void entity::Photo::setCenter(const osg::Vec3f& c)
{
    m_center = c;
}

const osg::Vec3f& entity::Photo::getCenter() const
{
    return m_center;
}

void entity::Photo::loadImage(const std::string& fname)
{
    osg::Image* image = osgDB::readImageFile(fname);
    m_texture->setImage(image);

    float aspectRatio = static_cast<float>(image->s()) / static_cast<float>(image->t());
    outLogVal("Image height", image->t());
    outLogVal("Image width", image->s());
    // half sizes based on aspect ratio
    m_width = 2.f * (dureu::PHOTO_MINW);
    m_height = m_width * aspectRatio;
    outLogVal("Photo(): width", m_width);
    outLogVal("Photo(): height", m_height);

    osg::Vec3Array* verts = new osg::Vec3Array;
    verts->push_back(m_center + osg::Vec3(m_width/2.f, m_height/2.f, 0.f));
    verts->push_back(m_center + osg::Vec3(-m_width/2.f, m_height/2.f, 0.f));
    verts->push_back(m_center + osg::Vec3(-m_width/2.f, -m_height/2.f, 0.f));
    verts->push_back(m_center + osg::Vec3(m_width/2.f, -m_height/2.f, 0.f));

    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(dureu::NORMAL);

    osg::Vec2Array* texcoords = new osg::Vec2Array;
    texcoords->push_back( osg::Vec2(m_width, m_width) );
    texcoords->push_back( osg::Vec2(m_width, 0.0f) );
    texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
    texcoords->push_back( osg::Vec2(0.0f, m_width) );

    this->setVertexArray(verts);
    this->setNormalArray(normals);
    this->setTexCoordArray(0, texcoords);
    this->setNormalBinding(osg::Geometry::BIND_OVERALL);
    this->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(dureu::PHOTO_CLR_REST);
    colors->push_back(dureu::PHOTO_CLR_REST);
    colors->push_back(dureu::PHOTO_CLR_REST);
    colors->push_back(dureu::PHOTO_CLR_REST);
    this->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}

osg::StateAttribute* entity::Photo::getTextureAsAttribute() const
{
    return dynamic_cast<osg::StateAttribute*>(m_texture.get());
}

void entity::Photo::setFrameColor(const osg::Vec4 color)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    (*colors)[0] = color;
    (*colors)[1] = color;
    (*colors)[2] = color;
    (*colors)[3] = color;
    this->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    this->dirtyDisplayList();
    this->dirtyBound();
}

void entity::Photo::setModeEdit(bool edit)
{
    if (edit)
        this->setFrameColor(dureu::CANVAS_CLR_EDIT);
    else
        this->setFrameColor(dureu::PHOTO_CLR_REST);
}

void entity::Photo::move(const double u, const double v)
{
    m_center = osg::Vec3f(u,v,0.f);

    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    (*verts)[0] = m_center + osg::Vec3(m_width/2.f, m_height/2.f, 0.f);
    (*verts)[1] = m_center + osg::Vec3(-m_width/2.f, m_height/2.f, 0.f);
    (*verts)[2] = m_center + osg::Vec3(-m_width/2.f, -m_height/2.f, 0.f);
    (*verts)[3] = m_center + osg::Vec3(m_width/2.f, -m_height/2.f, 0.f);

    this->dirtyDisplayList();
    this->dirtyBound();
}

REGISTER_OBJECT_WRAPPER(Photo_Wrapper
                        , new entity::Photo
                        , entity::Photo
                        , "osg::Object osg::Geometry entity::Photo")
{
    ADD_OBJECT_SERIALIZER(Texture, osg::Texture2D, NULL);
    ADD_VEC3F_SERIALIZER(Center, osg::Vec3f());
    ADD_FLOAT_SERIALIZER(Width, 0.f);
    ADD_FLOAT_SERIALIZER(Height, 0.f);
}
