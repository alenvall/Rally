#ifndef RALLY_VIEW_SCENEVIEW_H_
#define RALLY_VIEW_SCENEVIEW_H_

#include "view/RemoteCarView.h"
#include "view/CheckpointView.h"
#include "view/PlayerCarView.h"
#include "view/PortalView.h"
#include "model/World.h"

#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include "InputInit.h"

#include <map>

namespace Rally { namespace Util {
    class BulletDebugDrawer;
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

    private:
        Ogre::Viewport* addViewport(Ogre::Camera* followedCamera);
        Ogre::Camera* addCamera(Ogre::String cameraName);
        void loadResourceConfig(Ogre::String resourceConfigPath);
        void updatePlayerCar(float deltaTime);
        void updateRemoteCars();
		
        void updateCheckPoints();

        bool debugDrawEnabled;

        Rally::Model::World& world;

        Ogre::Camera* camera;
        Ogre::SceneManager* sceneManager;
        Ogre::RenderWindow* renderWindow;

        Rally::View::CheckpointView goalView;
        std::list<Rally::View::CheckpointView> checkPointViews;

        Rally::View::PlayerCarView playerCarView;
        std::map<int, Rally::View::RemoteCarView> remoteCarViews;

        Rally::View::PortalView tunnelPortalView;

        //std::map<const Rally::Model::RemoteCar&, TheViewType&> remoteCarViews;

        Rally::Util::BulletDebugDrawer* bulletDebugDrawer;
};

#endif // RALLY_VIEW_SCENEVIEW_H_
