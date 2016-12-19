#ifndef SCENESTATE_H
#define SCENESTATE_H

#include <vector>
#include <osg/Group>
#include <RootScene.h>

#include "ProtectedGroup.h"
#include "SVMData.h"

class RootScene;

namespace entity{

/*! \class SceneState
 * \brief A class to describe the current state of the scene, e.g., state of switches.
 *
 * It is mostly used when dealing with entity::Bookmarks and taking bookmark snapshots from GlWidget.
 * The scene state helps to preserve and use later values like switches values that define
 * visibility flags for data, construction tools; and also photo transparencies.
 *
 * It inherits osg::ProtectedGroup so that to be able to take advantage of OSG serialization since
 * the scene state is saved together with each bookmark data.
 *
 * The SceneState normally does not form any scene graphs, i.e., no children are added. However, when
 * later entity::SVMData added, the user can add them as a child when using SVM methods.
*/
class SceneState : public osg::ProtectedGroup
{
public:
    /*! Default constructor. Creates an empty scene state. */
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
    void setPhotoTransparencies(const std::vector<float>& transparencies);
    /*! A getter method to be used within OSG serialization procedures. */
    const std::vector<float>& getPhotoTransparencies() const;

    /*! A method that translates all the necessary RootScene settings into internals of SceneState.
     * \param scene is the pointer on RootScene variable
     * \sa RootScene::createSceneState() */
    void stripDataFrom(RootScene* scene);

    /*! A method to check whether internals are already loaded or not.
     * \return true if internals are set, and false - otherwise. */
    bool isEmpty() const;

    /*! A method that clear all the vector containers. */
    void clear();

    /*! A method to push into vector container a canvas data flag.
     * \param flag is true for visible and false for invisible */
    void pushDataFlag(bool flag);

    /*! A method to pop out from vector with canvas data flags.*/
    void popBackDataFlag();

    /*! A method to push into vector container a canvas tool flag.
     * \param flag is true for visible and false for invisible */
    void pushToolFlag(bool flag);

    /*! A method to pop out from vector with canvas tool flags.*/
    void popBackToolFlag();

    /*! A method to push into vector container a photo transparency value.
     * \param t is within range from 0 to 1 */
    void pushTransparency(float t);

    /*! A method to pop out from the vector container of photo transparency values. */
    void popBackTransparency();

    /*! A method to insert into vector with canvas data flags.
     * \param index is specified index at which needed to be inserted
     * \param flag is the value to be inserted */
    void insertDataFlag(int index, bool flag);

    /*! A method to erase from vector with canvas data flags.
     * \param index is specified index which needs to be erased */
    void eraseDataFlag(int index);

    /*! A method to insert into vector with canvas tool flags.
     * \param index is specified index at which needed to be inserted
     * \param flag is the value to be inserted */
    void insertToolFlag(int index, bool flag);

    /*! A method to erase from vector with canvas tool flags.
     * \param index is specified index which needs to be erased */
    void eraseToolFlag(int index);

    /*! A method to insert into vector with photo transparency values.
     * \param index is specified index at which needed to be inserted
     * \param t is the value to be inserted */
    void insertTransparency(int index, float t);

    /*! A method to erase from vector with photo transparencies.
     * \param start is specified start index  from which needs to be erased (inclusive)
     * \param number is the number of consequtive elements to erase */
    void eraseTransparency(int start, int number);

    /*! A method to reset a vector value with photo transparencies.
     * \param index is specified index  which needs to be reset
     * \param t is the new transparency value */
    void resetTransparency(int index, float t);

    bool addSVMData(const osg::Matrix& wall, const osg::Matrix& floor);

private:
    bool m_axisFlag;  /*!< Boolean flag indicating whether global axis visibility is on (true) or off (false). */
    bool m_bookmarksFlag; /*!< Boolean flag indicating whether bookmarks visibility is on (true) or off (false). */
    std::vector<bool> m_canvasDataFlags; /*!< Vector of boolean flags indicating whether canvas content visibility is on (true) or off (false). */
    std::vector<bool> m_canvasToolFlags; /*!< Vector of boolean flags indicating whether canvas tools visibility is on (true) or off (false). */
    std::vector<float> m_photoTransparencies; /*!< Vector of values indicating photo transparency level - from 0 (invisible) to 1 (visibile). */

}; // class SceneState

} // namespace entity

#endif // SCENESTATE_H
