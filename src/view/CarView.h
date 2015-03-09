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
		void enableWheelParticles(Rally::Vector3 rightBack, Rally::Vector3 rightFront, Rally::Vector3 leftBack, Rally::Vector3 leftFront);
		
		void initSkidmarks();
		void updateSkidmarks(Rally::Vector3 position, Rally::Vector3 normal, float speed);

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

		Ogre::ParticleSystem* rightBackSystem;
		Ogre::ParticleSystem* rightFrontSystem;
		Ogre::ParticleSystem* leftBackSystem;
		Ogre::ParticleSystem* leftFrontSystem;
		Ogre::SceneNode* bodyParticleNode;

        Ogre::SceneNode* skidmarkNode;
		Ogre::BillboardSet* skidmarkBillboards;
};

} }

#endif // RALLY_VIEW_CARVIEW_H_
