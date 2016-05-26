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

    /*! A setter method to be used within OSG serialization procedures. */
    void setAxisFlag(bool flag);
    /*! A getter method to be used within OSG serialization procedures. */
    bool getAxisFlag() const;

    /*! A setter method to be used within OSG serialization procedures. */
    void setBookmarksFlag(bool flag);
    /*! A getter method to be used within OSG serialization procedures. */
    bool getBookmarksFlag() const;

    /*! A setter method to be used within OSG serialization procedures. */
    void setCanvasDataFlags(const std::vector<bool>& flags);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<bool>& getCanvasDataFlags() const;

    /*! A setter method to be used within OSG serialization procedures. */
    void setCanvasToolFlags(const std::vector<bool>& flags);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<bool>& getCanvasToolFlags() const;

    /*! A setter method to be used within OSG serialization procedures. */
    void setPhotoTransparencies(const std::vector<double>& transparencies);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<double>& getPhotoTransparencies() const;

private:
    bool m_axisFlag;  /*!< Boolean flag indicating whether global axis visibility is on (true) or off (false). */
    bool m_bookmarksFlag; /*!< Boolean flag indicating whether bookmarks visibility is on (true) or off (false). */
    std::vector<bool> m_canvasDataFlags; /*!< Vector of boolean flags indicating whether canvas content visibility is on (true) or off (false). */
    std::vector<bool> m_canvasToolFlags; /*!< Vector of boolean flags indicating whether canvas tools visibility is on (true) or off (false). */
    std::vector<double> m_photoTransparencies; /*!< Vector of values indicating photo transparency level - from 0 (invisible) to 1 (visibile). */

}; // class SceneState

} // namespace entity

#endif // SCENESTATE_H
