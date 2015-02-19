#include "SceneView.h"
#include "controller/InputManager.h"

#include <iostream>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include "controller/BaseInputManager.h"

SceneView::SceneView()
    : camera(NULL),
	root(0),
    sceneManager(NULL),
    renderWindow(NULL)
	/*mCameraMan(0)*/{
}

SceneView::~SceneView(void) {
    root = Ogre::Root::getSingletonPtr();
    delete root;
}

void SceneView::initialize(std::string resourceConfigPath, std::string pluginConfigPath) {
	root = new Ogre::Root(pluginConfigPath);

    this->loadResourceConfig(resourceConfigPath);
    // (The actual precaching is done below, once there is a render context)

    if(!root->restoreConfig() && !root->showConfigDialog()) {
        throw std::runtime_error("Could neither restore Ogre config, nor read it from the user.");
    }

    renderWindow = root->initialise(
        true, // auto-create the render window now
        "Rally Sport Racing Game");
	std::cout << "renderwindow initialised";

    sceneManager = root->createSceneManager(Ogre::ST_GENERIC); // Todo: Research a good scene manager

    // This should be done after creating a scene manager, so that there is a render context (GL/D3D)
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    camera = this->addCamera("MainCamera");
    Ogre::Viewport* viewport = this->addViewport(camera);
    camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

//	mCameraMan = new OgreBites::SdkCameraMan(camera);

    // TODO: Fix this to follow car...
    camera->setPosition(Ogre::Vector3(0, 0, 80));
    camera->lookAt(Ogre::Vector3(0, 0, -300));

	std::cout << ((Ogre::RenderWindow*)root->getRenderTarget("Rally Sport Racing Game"))->getName();

	if(setup(renderWindow)){
		std::cout << " about to start rendering ";
		root->startRendering();
		std::cout << " after startrendering ";

	}


    // TODO: Implement separate scene loading (how do we do with lights?)
    Ogre::Entity* ogreHead = sceneManager->createEntity("Head", "ogrehead.mesh");
    Ogre::SceneNode* headNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    headNode->attachObject(ogreHead);
    sceneManager->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));
    Ogre::Light* light = sceneManager->createLight("MainLight");
    light->setPosition(20, 80, 50);

	Ogre::Light* pointLight = sceneManager->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(250, 150, 250));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);

}

Ogre::Viewport* SceneView::addViewport(Ogre::Camera* followedCamera) {
    Ogre::Viewport* viewport = renderWindow->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    return viewport;
}

Ogre::Camera* SceneView::addCamera(Ogre::String cameraName) {
    // Setup camera to match viewport
    Ogre::Camera* camera = sceneManager->createCamera(cameraName);
    camera->setNearClipDistance(5);

    return camera;
}

void SceneView::loadResourceConfig(Ogre::String resourceConfigPath) {
    Ogre::ResourceGroupManager& resourceGroupManager = Ogre::ResourceGroupManager::getSingleton();

    Ogre::ConfigFile resourceConfig;
    resourceConfig.load(resourceConfigPath);

    for(Ogre::ConfigFile::SectionIterator sectionIterator = resourceConfig.getSectionIterator();
            sectionIterator.hasMoreElements();
            sectionIterator.moveNext()) {
        Ogre::ConfigFile::SettingsMultiMap* settings = sectionIterator.peekNextValue();

        // Now load all the resources for this resource group
        for(Ogre::ConfigFile::SettingsMultiMap::iterator resource = settings->begin();
                resource != settings->end();
                ++resource) {
            resourceGroupManager.addResourceLocation(
                resource->second, // filename of directory
                resource->first, // resource type
                sectionIterator.peekNextKey()); // resource group
        }
    }
}

bool SceneView::renderFrame() {
    Ogre::WindowEventUtilities::messagePump();

    if(renderWindow->isClosed())  {
        return false;
    } else {
        Ogre::Root& root = Ogre::Root::getSingleton();
        if(!root.renderOneFrame()) {
            return false;
        }
    }
    return true;
}

bool SceneView::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseInputManager::frameRenderingQueued(evt);

    if(!processUnbufferedInput(evt)) return false;

    return ret;
}

bool SceneView::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    static bool mMouseDown = false;     // If a mouse button is depressed
    static Ogre::Real mToggle = 0.0;    // The time left until next toggle
    static Ogre::Real mRotate = 0.13;   // The rotate constant
    static Ogre::Real mMove = 250;      // The movement constant

    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);

    if (currMouse && ! mMouseDown)
    {
        // denna failar /Joel
        //Ogre::Light* light = sceneManager->getLight("pointLight");
        //light->setVisible(! light->isVisible());
    }

    mMouseDown = currMouse;

    mToggle -= evt.timeSinceLastFrame;

    if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_1))
    {
        mToggle  = 0.5;
        Ogre::Light* light = sceneManager->getLight("pointLight");
        light->setVisible(! light->isVisible());
    }

    return true;
}



