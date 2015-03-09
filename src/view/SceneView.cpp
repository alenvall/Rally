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
        renderWindow(NULL){
        debugDrawEnabled = false;
}

SceneView::~SceneView() {
    //delete bulletDebugDrawer;
    tunnelPortalView.detach();

    playerCarView.detach();

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

    sceneManager->setSkyDome(true, "Rally/CloudySky", 5, 8);

    camera = this->addCamera("MainCamera");
    Ogre::Viewport* viewport = this->addViewport(camera);
    camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    Ogre::SceneNode* sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    sceneNode->setPosition(Ogre::Vector3(0, 0, 0));

    // Load the scene.
    DotSceneLoader loader;
    loader.parseDotScene("world.scene", "General", sceneManager, sceneNode);
	sceneManager->getEntity("leaves1")->setCastShadows(false);
	sceneManager->getEntity("leaves2")->setCastShadows(false);
	sceneManager->getEntity("leaves3")->setCastShadows(false);
	sceneManager->getEntity("leaves4")->setCastShadows(false);
	sceneManager->getEntity("leaves5")->setCastShadows(false);
	sceneManager->getEntity("leaves6")->setCastShadows(false);
	sceneManager->getEntity("leaves7")->setCastShadows(false);
	sceneManager->getEntity("leaves8")->setCastShadows(false);
	sceneManager->getEntity("leaves9")->setCastShadows(false);
	sceneManager->getEntity("leaves10")->setCastShadows(false);
	sceneManager->getEntity("leaves11")->setCastShadows(false);
	sceneManager->getEntity("leaves12")->setCastShadows(false);
	sceneManager->getEntity("leaves13")->setCastShadows(false);
	sceneManager->getEntity("buske1")->setCastShadows(false);
	sceneManager->getEntity("buske2")->setCastShadows(false);
	sceneManager->getEntity("buske3")->setCastShadows(false);
	sceneManager->getEntity("buske4")->setCastShadows(false);
	sceneManager->getEntity("buske5")->setCastShadows(false);
	sceneManager->getEntity("buske6")->setCastShadows(false);
	sceneManager->getEntity("buske7")->setCastShadows(false);
	sceneManager->getEntity("tree1")->setCastShadows(false);
	sceneManager->getEntity("tree2")->setCastShadows(false);
	sceneManager->getEntity("tree3")->setCastShadows(false);
	sceneManager->getEntity("tree4")->setCastShadows(false);
	sceneManager->getEntity("tree5")->setCastShadows(false);
	sceneManager->getEntity("tree6")->setCastShadows(false);
	sceneManager->getEntity("tree7")->setCastShadows(false);
	sceneManager->getEntity("tree8")->setCastShadows(false);
	sceneManager->getEntity("tree9")->setCastShadows(false);
	sceneManager->getEntity("tree10")->setCastShadows(false);

    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    sceneManager->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5) );
    sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));

    Ogre::Light* sunLight = sceneManager->createLight("sunLight");
    sunLight->setType(Ogre::Light::LT_DIRECTIONAL);
    sunLight->setCastShadows(true);
    sunLight->setDirection(Ogre::Vector3( 1, -2, 1 ));
    sunLight->setDiffuseColour(Ogre::ColourValue(1, 1, 1));
    sunLight->setSpecularColour(Ogre::ColourValue(1, 1, 1));
    sceneNode->attachObject(sunLight);

	playerCarView.attachTo(sceneManager);

    // Debug draw Bullet
    bulletDebugDrawer = new Rally::Util::BulletDebugDrawer(sceneManager);
    world.getPhysicsWorld().getDynamicsWorld()->setDebugDrawer(bulletDebugDrawer);

	// Sky dome
	//sceneManager->setSkyDome(true, "Rally/CloudySky", 5, 8, 1000, true);

	sceneManager->setSkyDome(true, "Rally/CloudySky", 5, 8);

	// Place the magic surface at the end of the tunnel.
	tunnelPortalView.attachTo(sceneManager, "TunnelPortal");
	tunnelPortalView.setScale(15.0f, 5.0f, true);
    tunnelPortalView.setPosition(Rally::Vector3(86.0f, 5.0f, -134.0f));
    tunnelPortalView.setOrientation(Rally::Quaternion(Ogre::Math::Sqrt(0.5f), 0, -Ogre::Math::Sqrt(0.5f), 0));

    // Snap a picture for the magic surface at Kopparbunken.
	tunnelPortalView.moveCamera(
        Rally::Vector3(255.0f, 12.0f, 240.0f), // position
        Rally::Vector3(255.0f, 12.0f, 239.0f)); // look at
    tunnelPortalView.takeSnapshot();
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
        updateRemoteCars();

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


void SceneView::updatePlayerCar(float deltaTime) {
    // Todo: Move to separate view
    Rally::Model::Car& playerCar = world.getPlayerCar();
    Rally::Vector3 position = playerCar.getPosition();
    playerCarView.updateBody(playerCar.getPosition(), playerCar.getOrientation());
    playerCarView.updateWheels(
        playerCar.getRightFrontWheelOrientation(),
        playerCar.getLeftFrontWheelOrientation(),
        playerCar.getRightBackWheelOrientation(),
        playerCar.getLeftBackWheelOrientation());

	Rally::Vector3 currentCameraPosition = camera->getPosition();

	Rally::Vector3 displacementBase = playerCar.getOrientation() * Ogre::Vector3::UNIT_Z;
	displacementBase *= -1;

	Rally::Vector3 displacement(6.0f * displacementBase.x, 3.5f, 6.0f * displacementBase.z);
    Rally::Vector3 endPosition = position + displacement;

	float velocityAdjust = playerCar.getVelocity().length()/6;
	float lerpAdjust = Ogre::Math::Clamp(velocityAdjust*deltaTime, 0.01f, 0.25f);

	// Lerp towards the new camera position to get a smoother pan
	float lerpX = Ogre::Math::lerp(currentCameraPosition.x, endPosition.x, lerpAdjust);
	float lerpY = Ogre::Math::lerp(currentCameraPosition.y, endPosition.y, lerpAdjust);
	float lerpZ = Ogre::Math::lerp(currentCameraPosition.z, endPosition.z, lerpAdjust);

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
	camera->lookAt(position);

    // This is a bit of a temporary hack... It is laggy though...
    /*Rally::Vector3 lookVector = (Rally::Vector3(255.0f, 12.0f, 240.0f) - Rally::Vector3(255.0f, 12.0f, 239.0f))*
        (cameraPosition - Rally::Vector3(86.0f, 12.0f, -134.0f)).length();
	tunnelPortalView.moveCamera(
        Rally::Vector3(255.0f, 12.0f, 240.0f) + lookVector, // position
        Rally::Vector3(255.0f, 12.0f, 239.0f) + lookVector); // look at
    tunnelPortalView.takeSnapshot();*/
}

void SceneView::updateRemoteCars() {
    for(std::map<int, Rally::View::RemoteCarView>::iterator carViewIterator = remoteCarViews.begin();
            carViewIterator != remoteCarViews.end();
            ++carViewIterator) {
        carViewIterator->second.updateWithRemoteCar();
    }
}

void SceneView::remoteCarUpdated(int carId, const Rally::Model::RemoteCar& remoteCar) {
    std::map<int, Rally::View::RemoteCarView>::iterator found = remoteCarViews.find(carId);

    // Lazily construct if not found
    if(found == remoteCarViews.end()) {
        found = remoteCarViews.insert(std::map<int, Rally::View::RemoteCarView>::value_type(carId,
            Rally::View::RemoteCarView(remoteCar))).first;
        std::ostringstream carNameStream;
        carNameStream << "RemoteCar_" << carId;
        std::string carName = carNameStream.str();

        found->second.attachTo(sceneManager, carName);
    }

    // We don't really update the car here, as it has to be done every frame for the interpolation.
}

void SceneView::remoteCarRemoved(int carId, const Rally::Model::RemoteCar& remoteCar) {
    remoteCarViews.erase(carId);
}

void SceneView::setDebugDrawEnabled(bool enabled){
    debugDrawEnabled = enabled;
}

void SceneView::toggleReflections() {
    playerCarView.setReflectionsOn(!playerCarView.isReflectionsOn());
}
