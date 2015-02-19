#ifndef RALLY_VIEW_SCENEVIEW_H_
#define RALLY_VIEW_SCENEVIEW_H_

#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include "controller/BaseInputManager.h"

class SceneView : public BaseInputManager{
    public:
        SceneView();
        virtual ~SceneView();
        void initialize(std::string resourceConfigPath, std::string pluginConfigPath);
        bool renderFrame();


	protected:
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    private:
        Ogre::Viewport* addViewport(Ogre::Camera* followedCamera);
        Ogre::Camera* addCamera(Ogre::String cameraName);
        void loadResourceConfig(Ogre::String resourceConfigPath);
		bool processUnbufferedInput(const Ogre::FrameEvent& evt);

        Ogre::Camera* camera;
        Ogre::SceneManager* sceneManager;
		Ogre::Root* root;
		Ogre::RenderWindow* renderWindow;

};

#endif // RALLY_VIEW_SCENEVIEW_H_
