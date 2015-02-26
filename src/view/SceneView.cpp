#include "view/SceneView.h"

#include "DotSceneLoader.h"

#include "util/BulletDebugDrawer.h"

#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include "InputInit.h"

#include <sstream>
#include <string>

SceneView::SceneView(Rally::Model::World& world) :
        world(world),
        camera(NULL),
        sceneManager(NULL),
        renderWindow(NULL){
        debugDrawEnabled = false;
}

SceneView::~SceneView() {
    delete bulletDebugDrawer;

    Ogre::Root* root = Ogre::Root::getSingletonPtr();
    delete root;
}

void SceneView::initialize(std::string resourceConfigPath, std::string pluginConfigPath) {
    Ogre::Root* root = new Ogre::Root(pluginConfigPath);

    this->loadResourceConfig(resourceConfigPath);
    // (The actual precaching is done below, once there is a render context)

    if(!root->restoreConfig() && !root->showConfigDialog()) {
        throw std::runtime_error("Could neither restore Ogre config, nor read it from the user.");
    }

    renderWindow = root->initialise(
        true, // auto-create the render window now
        "Rally Sport Racing Game");

    sceneManager = root->createSceneManager("OctreeSceneManager"); // Todo: Research a good scene manager

    // This should be done after creating a scene manager, so that there is a render context (GL/D3D)
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    camera = this->addCamera("MainCamera");
    Ogre::Viewport* viewport = this->addViewport(camera);
    camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    Ogre::SceneNode* sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    sceneNode->setPosition(Ogre::Vector3(0, 110.0f, 0));

	sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	Ogre::Light* light = sceneManager->createLight("MainLight");

	// Load the scene.
	DotSceneLoader loader;
	loader.parseDotScene("world.scene", "General", sceneManager, sceneNode);

	playerCarView.attachTo(sceneManager, "PlayerCar");

    // Debug draw Bullet
    bulletDebugDrawer = new Rally::Util::BulletDebugDrawer(sceneManager);
    world.getPhysicsWorld().getDynamicsWorld()->setDebugDrawer(bulletDebugDrawer);
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
        updatePlayerCar();

    if(debugDrawEnabled){
        world.getPhysicsWorld().getDynamicsWorld()->debugDrawWorld();
    }

        Ogre::Root& root = Ogre::Root::getSingleton();
        if(!root.renderOneFrame()) {
            return false;
        }
    }
    return true;
}

void SceneView::updatePlayerCar() {
    Rally::Model::Car& playerCar = world.getPlayerCar();
    playerCarView.updateBody(playerCar.getPosition(), playerCar.getOrientation());
    playerCarView.updateWheels(
        playerCar.getRightFrontWheelOrientation(),
        playerCar.getLeftFrontWheelOrientation(),
        playerCar.getRightBackWheelOrientation(),
        playerCar.getLeftBackWheelOrientation());

    // Temporary hack to move camera after car
    Rally::Vector3 displacementBase = playerCar.getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
    Rally::Vector3 displacement(12.0f * displacementBase.x, 3.0f, 12.0f*displacementBase.z);
    Rally::Vector3 cameraPosition = playerCar.getPosition() + displacement;
    camera->setPosition(cameraPosition);
    sceneManager->getLight("MainLight")->setPosition(cameraPosition);
    camera->lookAt(playerCar.getPosition());
}

void SceneView::remoteCarUpdated(int carId, const Rally::Model::RemoteCar& remoteCar) {
    std::map<int, Rally::View::CarView>::iterator found = remoteCarViews.find(carId);

    // Lazily construct if not found
    if(found == remoteCarViews.end()) {
        found = remoteCarViews.insert(std::map<int, Rally::View::CarView>::value_type(carId,
            Rally::View::CarView(/*remoteCar*/))).first;
        std::ostringstream carNameStream;
        carNameStream << "RemoteCar_" << carId;
        std::string carName = carNameStream.str();

        found->second.attachTo(sceneManager, carName);
    }

    found->second.updateBody(remoteCar.getPosition(), remoteCar.getOrientation());
    // found->second.updateWheels();
}

void SceneView::remoteCarRemoved(int carId, const Rally::Model::RemoteCar& remoteCar) {
    remoteCarViews.erase(carId);
}

void SceneView::setDebugDrawEnabled(bool enabled){
    debugDrawEnabled = enabled;
}
