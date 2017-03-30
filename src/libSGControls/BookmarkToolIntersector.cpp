#include "BookmarkToolIntersector.h"

#include "Utilities.h"
#include <QDebug>

BookmarkToolIntersector::BookmarkToolIntersector()
    : PolyLineIntersector()
{
    this->setOffset(threshold);
}

BookmarkToolIntersector::BookmarkToolIntersector(const osg::Vec3 &start, const osg::Vec3 &end)
    : PolyLineIntersector(start, end)
{
    this->setOffset(threshold);
}

BookmarkToolIntersector::BookmarkToolIntersector(osgUtil::Intersector::CoordinateFrame cf, double x, double y)
    : PolyLineIntersector(cf, x, y)
{
    this->setOffset(threshold);
}

BookmarkToolIntersector::BookmarkToolIntersector(osgUtil::Intersector::CoordinateFrame cf, const osg::Vec3d &start, const osg::Vec3d &end)
    : PolyLineIntersector(cf, start, end)
{
    this->setOffset(threshold);
}

osgUtil::Intersector *BookmarkToolIntersector::clone(osgUtil::IntersectionVisitor &iv)
{
    if ( _coordinateFrame==MODEL && iv.getModelMatrix()==0 )
    {
        osg::ref_ptr<BookmarkToolIntersector> cloned = new BookmarkToolIntersector( _start, _end );
        cloned->_parent = this;
        cloned->m_offset = m_offset;
        return cloned.release();
    }

    osg::Matrix matrix;
    switch ( _coordinateFrame )
    {
    case WINDOW:
        if (iv.getWindowMatrix()) matrix.preMult( *iv.getWindowMatrix() );
        if (iv.getProjectionMatrix()) matrix.preMult( *iv.getProjectionMatrix() );
        if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
        if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
        break;
    case PROJECTION:
        if (iv.getProjectionMatrix()) matrix.preMult( *iv.getProjectionMatrix() );
        if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
        if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
        break;
    case VIEW:
        if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
        if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
        break;
    case MODEL:
        if (iv.getModelMatrix()) matrix = *iv.getModelMatrix();
        break;
    }

    osg::Matrix inverse = osg::Matrix::inverse(matrix);
    osg::ref_ptr<BookmarkToolIntersector> cloned = new BookmarkToolIntersector( _start*inverse, _end*inverse );
    cloned->_parent = this;
    cloned->m_offset = m_offset;
    return cloned.release();
}

void BookmarkToolIntersector::intersect(osgUtil::IntersectionVisitor &iv, osg::Drawable *drawable)
{
    osg::BoundingBox bb = drawable->getBoundingBox();
    bb.xMin() -= m_offset; bb.xMax() += m_offset;
    bb.yMin() -= m_offset; bb.yMax() += m_offset;
    bb.zMin() -= m_offset; bb.zMax() += m_offset;

    osg::Vec3d s(_start), e(_end);
    if (!intersectAndClip(s, e, bb)) return;
    if (iv.getDoDummyTraversal()) return;

    osg::Geometry* geometry = drawable->asGeometry();
    if (geometry)
    {
        if (!this->isRightPrimitive(geometry)) return;

        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
        if (!vertices) return;

        for (unsigned int i=0; i<vertices->size(); ++i)
        {
            if (i==6 || i==8 || i ==10) continue;

            int j = (i==0)? vertices->size()-1 : i-1;
            double distance = Utilities::getSkewLinesDistance(s,e,(*vertices)[i], (*vertices)[j]);
            if (m_offset<distance) continue;

            Intersection hit;
            hit.ratio = distance;
            hit.nodePath = iv.getNodePath();
            hit.drawable = drawable;
            hit.matrix = iv.getModelMatrix();
            hit.localIntersectionPoint = (*vertices)[i];
            m_hitIndices.push_back(i);
            insertIntersection(hit);
        }
    }
}
