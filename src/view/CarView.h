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
        virtual void attachTo(Ogre::SceneManager* sceneManager, const std::string& carName);
        virtual void detach();

        virtual void updateBody(const Rally::Vector3& position, const Rally::Quaternion& orientation);
        void updateWheels(
            const Rally::Quaternion& rightFrontWheelOrientation,
            const Rally::Quaternion& leftFrontWheelOrientation,
            const Rally::Quaternion& rightBackWheelOrientation,
            const Rally::Quaternion& leftBackWheelOrientation);

		void initParticleSystem();
		void activateParticles(Rally::Vector3 position, int intensity);
		void enableWheelParticles(bool rightBack, bool rightFront, bool leftBack, bool leftFront);

    protected:
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

		Ogre::ParticleSystem* particleSystem;
		Ogre::ParticleSystem* rightBackSystem;
		Ogre::ParticleSystem* rightFrontSystem;
		Ogre::ParticleSystem* leftBackSystem;
		Ogre::ParticleSystem* leftFrontSystem;
		Ogre::SceneNode* bodyParticleNode;
};

} }

#endif // RALLY_VIEW_CARVIEW_H_
