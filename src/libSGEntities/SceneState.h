#ifndef SCENESTATE_H
#define SCENESTATE_H

#include <vector>
#include <osg/Group>
#include <RootScene.h>

namespace entity{

/*! \class SceneState
 * \brief A class to describe the current state of the scene, e.g., state of switches.
 *
 * It is mostly used when dealing with Bookmarks and taking bookmark snapshots from GlWidget.
 * The scene state helps to preserve and use later values like switches values that define
 * visibility flags for data, construction tools; and also photo transparencies.
 *
 * It inherits osg::Group so that to be able to take advantage of OSG serialization since
 * the scene state is saved together with each bookmark data.
*/
class SceneState : public osg::Group
{
public:
    SceneState();
    /*! Constructor by copy.
     * This method is used solely by OSG serializer and should never be called from within the
     * application.
     * \param parent is the copy-from object
     * \param copyop is the copy method, i.e., deep vs. shallow copy */
    SceneState(const SceneState& parent, osg::CopyOp copyop = osg::CopyOp::SHALLOW_COPY);
    META_Node(entity, SceneState)

    void setAxisFlag(bool flag);
    bool getAxisFlag() const;

    void setBookmarksFlag(bool flag);
    bool getBookmarksFlag() const;

    void setCanvasDataFlags(const std::vector<bool>& flags);
    const std::vector<bool>& getCanvasDataFlags() const;

    void setCanvasToolFlags(const std::vector<bool>& flags);
    const std::vector<bool>& getCanvasToolFlags() const;

    void setPhotoTransparencies(const std::vector<double>& transparencies);
    const std::vector<double>& getPhotoTransparencies() const;

private:
    bool m_axisFlag;
    bool m_bookmarksFlag;
    std::vector<bool> m_canvasDataFlags;
    std::vector<bool> m_canvasToolFlags;
    std::vector<double> m_photoTransparencies;

}; // class SceneState

} // namespace entity

#endif // SCENESTATE_H
