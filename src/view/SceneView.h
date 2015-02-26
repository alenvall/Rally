#ifndef RALLY_VIEW_SCENEVIEW_H_
#define RALLY_VIEW_SCENEVIEW_H_

#include "view/CarView.h"
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
        bool renderFrame();
        void remoteCarUpdated(int carId, const Rally::Model::RemoteCar& remoteCar);
        void remoteCarRemoved(int carId, const Rally::Model::RemoteCar& remoteCar);
        void setDebugDrawEnabled(bool enabled);

    private:
        Ogre::Viewport* addViewport(Ogre::Camera* followedCamera);
        Ogre::Camera* addCamera(Ogre::String cameraName);
        void loadResourceConfig(Ogre::String resourceConfigPath);
        void updatePlayerCar();
        bool debugDrawEnabled;

        Rally::Model::World& world;

        Ogre::Camera* camera;
        Ogre::SceneManager* sceneManager;
        Ogre::RenderWindow* renderWindow;

        Rally::View::CarView playerCarView;
        std::map<int, Rally::View::CarView> remoteCarViews;

        //std::map<const Rally::Model::RemoteCar&, TheViewType&> remoteCarViews;

        Rally::Util::BulletDebugDrawer* bulletDebugDrawer;
};

#endif // RALLY_VIEW_SCENEVIEW_H_
