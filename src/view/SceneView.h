#ifndef RALLY_VIEW_SCENEVIEW_H_
#define RALLY_VIEW_SCENEVIEW_H_

#include "view/RemoteCarView.h"
#include "view/PlayerCarView.h"
#include "view/PortalView.h"
#include "view/BloomView.h"
#include "view/LensFlareView.h"
#include "model/World.h"

#include <OgreCamera.h>
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
        void remoteCarUpdated(int carId, const Rally::Model::RemoteCar& remoteCar);
        void remoteCarRemoved(int carId, const Rally::Model::RemoteCar& remoteCar);
        void setDebugDrawEnabled(bool enabled);
        void toggleReflections();
        void addLogicListener(Rally::View::SceneView_LogicListener& logicListener);

    private:
        Ogre::Viewport* addViewport(Ogre::Camera* followedCamera);
        Ogre::Camera* addCamera(Ogre::String cameraName);
        void loadResourceConfig(Ogre::String resourceConfigPath);
        void updatePlayerCar(float deltaTime);
        void updateRemoteCars();

        bool debugDrawEnabled;

        Rally::Model::World& world;

        Ogre::Camera* camera;
        Ogre::SceneManager* sceneManager;
        Ogre::RenderWindow* renderWindow;

        Rally::View::PlayerCarView playerCarView;
        std::map<int, Rally::View::RemoteCarView> remoteCarViews;

        Rally::View::PortalView tunnelPortalView;

        //std::map<const Rally::Model::RemoteCar&, TheViewType&> remoteCarViews;

        Rally::Util::BulletDebugDrawer* bulletDebugDrawer;

        Rally::View::BloomView bloomView;
		Rally::View::LensFlareView* lensflare;
};

#endif // RALLY_VIEW_SCENEVIEW_H_
