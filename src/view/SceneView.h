#ifndef RALLY_VIEW_SCENEVIEW_H_
#define RALLY_VIEW_SCENEVIEW_H_

#include "view/RemoteCarView.h"
#include "view/CheckpointView.h"
#include "view/PlayerCarView.h"
#include "view/PortalView.h"
#include "view/GBufferView.h"
#include "view/SSAOView.h"
#include "view/BloomView.h"
#include "view/MotionBlurView.h"
#include "view/LensFlareView.h"
#include "model/World.h"
#include "view/OgreText.h"

#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include "InputInit.h"

#include <map>

namespace Rally { namespace Util {
    class BulletDebugDrawer;
} }

namespace Rally { namespace View {

class SceneView_LogicListener : public Ogre::FrameListener {
    public:
        virtual ~SceneView_LogicListener() {
        }
        virtual void updateLogic(float deltaTime) = 0;
        virtual bool frameRenderingQueued(const Ogre::FrameEvent& event) {
            updateLogic(event.timeSinceLastFrame);
            return true;
        }
};

} }

class SceneView {
    public:
        SceneView(Rally::Model::World& world);
        virtual ~SceneView();
        void initialize(std::string resourceConfigPath, std::string pluginConfigPath);
        bool renderFrame(float deltaTime);
        void remoteCarUpdated(int carId, const Rally::Model::RemoteCar& remoteCar, bool carTypeChanged);
        void remoteCarRemoved(int carId, const Rally::Model::RemoteCar& remoteCar);
        void toggleDebugDraw();
        void toggleReflections();
        void togglePostProcessing();
		void toggleKeyMenu();
		void toggleTrack();
        void addLogicListener(Rally::View::SceneView_LogicListener& logicListener);

        void playerCarTypeUpdated() {
            playerCarView.changeCar(world.getPlayerCar().getCarType());
        }

    private:
        Ogre::Viewport* addViewport(Ogre::Camera* followedCamera);
        Ogre::Camera* addCamera(Ogre::String cameraName);
        void loadResourceConfig(Ogre::String resourceConfigPath);
        void updatePlayerCar(float deltaTime);
        void updateRemoteCars();

        void updateCheckPoints();

        bool debugDrawEnabled;
        bool postProcessingEnabled;
		bool showKeyMenu;

		void updateParticles();

		void updateOverlay();

		void updateSkidmarks();

        Rally::Model::World& world;

        Ogre::Camera* camera;
        Ogre::Viewport* viewport;
        Ogre::SceneManager* sceneManager;
        Ogre::RenderWindow* renderWindow;
		Ogre::ParticleSystemManager* particleManager;

        Rally::View::CheckpointView goalView;
        std::list<Rally::View::CheckpointView> checkPointViews;

        Rally::View::PlayerCarView playerCarView;
        std::map<int, Rally::View::RemoteCarView> remoteCarViews;

        //Rally::View::PortalView tunnelPortalView;

        //std::map<const Rally::Model::RemoteCar&, TheViewType&> remoteCarViews;

        Rally::Util::BulletDebugDrawer* bulletDebugDrawer;

        Rally::View::GBufferView gbufferView;
        Rally::View::SSAOView ssaoView;
        Rally::View::BloomView bloomView;
        Rally::View::MotionBlurView motionBlurView;
		Rally::View::LensFlareView* lensflare;

		Rally::View::OgreText* timeText;
		Rally::View::OgreText* speedText;
		Rally::View::OgreText* lastTimeText;
		Rally::View::OgreText* highScoreText;
		Rally::View::OgreText* kmhText;
		Rally::View::OgreText* keyText;
		Rally::View::OgreText* titleText;
		Rally::View::OgreText* trackText;
};

#endif // RALLY_VIEW_SCENEVIEW_H_
