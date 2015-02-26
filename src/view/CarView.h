#ifndef RALLY_VIEW_CARVIEW_H_
#define RALLY_VIEW_CARVIEW_H_

#include "Rally.h"

namespace Ogre {
    class SceneManager;
    class Entity;
    class SceneNode;
}

namespace Rally { namespace View {

class CarView {
    public:
        CarView();
        virtual ~CarView();
        void attachTo(Ogre::SceneManager* sceneManager, const std::string& carName);

        void updateBody(Rally::Vector3 position, Rally::Quaternion orientation);
        void updateWheels(
            const Rally::Quaternion& rightFrontWheelOrientation,
            const Rally::Quaternion& leftFrontWheelOrientation,
            const Rally::Quaternion& rightBackWheelOrientation,
            const Rally::Quaternion& leftBackWheelOrientation);

    private:
        Ogre::SceneManager* sceneManager;
        Ogre::Entity* carEntity;
        Ogre::SceneNode* carNode;

        Ogre::Entity* rightFrontWheelEntity;
        Ogre::Entity* leftFrontWheelEntity;
        Ogre::Entity* rightBackWheelEntity;
        Ogre::Entity* leftBackWheelEntity;

        Ogre::SceneNode* rightFrontWheelNode;
        Ogre::SceneNode* leftFrontWheelNode;
        Ogre::SceneNode* rightBackWheelNode;
        Ogre::SceneNode* leftBackWheelNode;
};

} }

#endif // RALLY_VIEW_CARVIEW_H_
