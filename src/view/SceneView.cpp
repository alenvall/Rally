#include "view/SceneView.h"

#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>

SceneView::SceneView()
    : camera(NULL),
    sceneManager(NULL),
    renderWindow(NULL) {
}

SceneView::~SceneView(void) {
    Ogre::Root* root = Ogre::Root::getSingletonPtr();
    delete root;
}

bool SceneView::initialize(std::string resourceConfigPath, std::string pluginConfigPath) {
    Ogre::Root* root = new Ogre::Root(pluginConfigPath);

    this->loadResourceConfig(resourceConfigPath);
    // (The actual precaching is done below, once there is a render context)

    if(!root->restoreConfig() && !root->showConfigDialog()) {
        return false;
    }

    renderWindow = root->initialise(
        true, // auto-create the render window now
        "Rally Sport Racing Game");

    sceneManager = root->createSceneManager(Ogre::ST_GENERIC); // Todo: Research a good scene manager

    // This should be done after creating a scene manager, so that there is a render context (GL/D3D)
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    camera = this->addCamera("MainCamera");
    Ogre::Viewport* viewport = this->addViewport(camera);
    camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    // TODO: Fix this to follow car...
    camera->setPosition(Ogre::Vector3(0, 0, 80));
    camera->lookAt(Ogre::Vector3(0, 0, -300));

    // TODO: Implement separate scene loading (how do we do with lights?)
    Ogre::Entity* ogreHead = sceneManager->createEntity("Head", "ogrehead.mesh");
    Ogre::SceneNode* headNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    headNode->attachObject(ogreHead);
    sceneManager->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    Ogre::Light* light = sceneManager->createLight("MainLight");
    light->setPosition(20, 80, 50);
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
