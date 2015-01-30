#ifndef RALLY_VIEW_SCENEVIEW_H_
#define RALLY_VIEW_SCENEVIEW_H_

#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>

class SceneView {
    public:
        SceneView();
        virtual ~SceneView();
        bool initialize(std::string resourceConfigPath, std::string pluginConfigPath);
        bool renderFrame();

    private:
        Ogre::Viewport* initializeViewport();
        Ogre::Camera* addCamera(Ogre::Viewport* viewport, Ogre::String cameraName);
        void loadResourceConfig(Ogre::String resourceConfigPath);

        Ogre::Camera* camera;
        Ogre::SceneManager* sceneManager;
        Ogre::RenderWindow* renderWindow;
};

#endif // RALLY_VIEW_SCENEVIEW_H_
