#include "view/SceneView.h"

#include "DotSceneLoader.h"

#include "util/BulletDebugDrawer.h"

#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>

#include <sstream>
#include <string>

#include <btBulletDynamicsCommon.h>

SceneView::SceneView(Rally::Model::World& world) :
        world(world),
        camera(NULL),
        sceneManager(NULL),
        renderWindow(NULL) {
}

SceneView::~SceneView() {
    //delete bulletDebugDrawer;

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

	// Todo: Move to appropriate view
	Ogre::Entity* playerCarEntity = sceneManager->createEntity("PlayerCar", "car.mesh");
	playerCarNode = sceneManager->getRootSceneNode()->createChildSceneNode();
	playerCarNode->attachObject(playerCarEntity);
    playerCarNode->scale(Ogre::Vector3(2.0f, 1.0f, 4.0f) / playerCarEntity->getBoundingBox().getSize()); // Force scale to 2, 1, 4. Might be buggy...

    // Debug draw Bullet
    //bulletDebugDrawer = new Rally::Util::BulletDebugDrawer(sceneManager);
    //world.getPhysicsWorld().getDynamicsWorld()->setDebugDrawer(bulletDebugDrawer);

	// Sky dome
	//sceneManager->setSkyDome(true, "Rally/CloudySky", 5, 8, 1000, true);
	sceneManager->setSkyDome(true, "Rally/CloudySky", 5, 8);
}

Ogre::Viewport* SceneView::addViewport(Ogre::Camera* followedCamera) {
    Ogre::Viewport* viewport = renderWindow->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    return viewport;
}

Ogre::Camera* SceneView::addCamera(Ogre::String cameraName) {
    // Setup camera to match viewport
    Ogre::Camera* camera = sceneManager->createCamera(cameraName);
	camera->setNearClipDistance(Ogre::Real(0.1));

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

bool SceneView::renderFrame(float deltaTime) {
    Ogre::WindowEventUtilities::messagePump();

    if(renderWindow->isClosed())  {
        return false;
    } else {
        updatePlayerCar(deltaTime);
		world.getPhysicsWorld().getDynamicsWorld()->debugDrawWorld();

        Ogre::Root& root = Ogre::Root::getSingleton();
        if(!root.renderOneFrame()) {
            return false;
        }
    }
    return true;
}

void SceneView::updatePlayerCar(float deltaTime) {
    // Todo: Move to separate view
    Rally::Model::Car& playerCar = world.getPlayerCar();
    Rally::Vector3 position = playerCar.getPosition();
    playerCarNode->setPosition(position);
    playerCarNode->setOrientation(playerCar.getOrientation());

	Rally::Vector3 currentCameraPosition = camera->getPosition();

	Rally::Vector3 displacementBase = playerCar.getOrientation() * Ogre::Vector3::UNIT_Z;
	displacementBase *= -1;
	
	Rally::Vector3 displacement(12.0f * displacementBase.x, 6.0f, 12.0f * displacementBase.z);
    Rally::Vector3 endPosition = position + displacement;

	float lerpAdjust = (playerCar.getVelocity().length())/5;
	//std::cout << lerpAdjust << std::endl;

	// Lerp towards the new camera position to get a smoother pan
	float lerpX = Ogre::Math::lerp(currentCameraPosition.x, endPosition.x, lerpAdjust*deltaTime);
	float lerpY = Ogre::Math::lerp(currentCameraPosition.y, endPosition.y, lerpAdjust*deltaTime);
	float lerpZ = Ogre::Math::lerp(currentCameraPosition.z, endPosition.z, lerpAdjust*deltaTime);

	Rally::Vector3 newPos(lerpX, lerpY, lerpZ);
	Rally::Vector3 cameraPosition = newPos;

	/*
	Shoot a ray from the car (with an offset to prevent collision with itself) to the camera.
	If anyting is intersected the camera is adjusted to prevent that the camera is blocked.
	*/
	btVector3 start(position.x, position.y + 2.0f, position.z);
	btVector3 end(newPos.x, newPos.y, newPos.z);

	btCollisionWorld::ClosestRayResultCallback ClosestRayResultCallBack(start, end);

	// Perform raycast
	world.getPhysicsWorld().getDynamicsWorld()->getCollisionWorld()->rayTest(start, end, ClosestRayResultCallBack);

	if(ClosestRayResultCallBack.hasHit()) {
		btVector3 hitLoc = ClosestRayResultCallBack.m_hitPointWorld;

		//If the camera is blocked, the new camera is set to where the collison
		//happened with a tiny offset.
		cameraPosition = Rally::Vector3(hitLoc.getX(), hitLoc.getY(), hitLoc.getZ());
		
		float camOffset = 0.5f;

		//Adjust for X
		if(hitLoc.getX() > cameraPosition.x)
			cameraPosition += Rally::Vector3(-camOffset, 0.0f, 0.0f);
		else if(hitLoc.getX() < cameraPosition.x)
			cameraPosition += Rally::Vector3(camOffset, 0.0f, 0.0f);
		
		//Adjust for Y
		if(hitLoc.getY() > cameraPosition.y)
			cameraPosition += Rally::Vector3(0.0f, -camOffset, 0.0f);

		//Adjust for Z
		if(hitLoc.getZ() > cameraPosition.z)
			cameraPosition += Rally::Vector3(0.0f, 0.0f, -camOffset);
		else if(hitLoc.getZ() < cameraPosition.z)
			cameraPosition += Rally::Vector3(0.0f, 0.0f, camOffset);

	}

    camera->setPosition(cameraPosition);
    sceneManager->getLight("MainLight")->setPosition(cameraPosition);
	camera->lookAt(position);
}

void SceneView::remoteCarUpdated(int carId, const Rally::Model::RemoteCar& remoteCar) {
    // Todo: Move to separate view
    std::ostringstream baseNameStream;
    baseNameStream << "RemoteCar_" << carId;
    std::string baseString = baseNameStream.str();
    std::string nodeName = baseString + "_Node";

    Ogre::SceneNode* remoteCarNode;
    if(sceneManager->hasSceneNode(nodeName)) {
        remoteCarNode = sceneManager->getSceneNode(nodeName);// Throws if nodeName not found.
    } else {
        // Lazily construct if not found
        Ogre::Entity* remoteCarEntity = sceneManager->createEntity(baseString + "_Entity", "car.mesh");
        remoteCarNode = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
        remoteCarNode->attachObject(remoteCarEntity);
        remoteCarNode->scale(Ogre::Vector3(2.0f, 1.0f, 4.0f) / remoteCarEntity->getBoundingBox().getSize()); // Force scale to 2, 1, 4. Might be buggy...
    }

    remoteCarNode->setPosition(remoteCar.getPosition());
    remoteCarNode->setOrientation(remoteCar.getOrientation());

    /*std::map<const Rally::Model::RemoteCar&, TheViewType::iterator found = remoteCarViews.find(remoteCar);

    // Lazily construct if not found
    if(found == remoteCarViews.end()) {
        found = remoteCarViews.insert(std::map<const Rally::Model::RemoteCar&, TheViewType>::value_type(remoteCar,
            TheViewType(remoteCar))).first;
    }*/
}

void SceneView::remoteCarRemoved(int carId, const Rally::Model::RemoteCar& remoteCar) {
    // Todo: Move to separate view
    std::ostringstream baseNameStream;
    baseNameStream << "RemoteCar_" << carId;
    std::string baseString = baseNameStream.str();

    sceneManager->destroySceneNode(baseString + "_Node");
    sceneManager->destroyEntity(baseString + "_Entity");
}
