#include "view/SceneView.h"

#include "util/DotSceneLoader.h"

#include "util/BulletDebugDrawer.h"

#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreCompositorManager.h>
#include <OgreWindowEventUtilities.h>

#include <sstream>
#include <string>

#include <btBulletDynamicsCommon.h>

SceneView::SceneView(Rally::Model::World& world) :
        world(world),
        camera(NULL),
        viewport(NULL),
        sceneManager(NULL),
        renderWindow(NULL),
        postProcessingEnabled(false) {
    debugDrawEnabled = false;
}

SceneView::~SceneView() {
    //delete bulletDebugDrawer;
    tunnelPortalView.detach();

    playerCarView.detach();

    bloomView.detach();
    ssaoView.detach();
    gbufferView.detach();

	lensflare->end();
	delete lensflare;
	lensflare = NULL;

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
    viewport = this->addViewport(camera);
    camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    Ogre::SceneNode* sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    sceneNode->setPosition(Ogre::Vector3(0, 0, 0));

    // Load the scene.
    DotSceneLoader loader;
    loader.parseDotScene("world.scene", "General", sceneManager, sceneNode);

    // shadows
   	sceneManager->getEntity("mark")->setCastShadows(false);
    sceneManager->getEntity("horisont")->setCastShadows(false);
    sceneManager->getEntity("fysikgrans")->setCastShadows(false);
    sceneManager->getEntity("maskin")->setCastShadows(false);
    sceneManager->getEntity("plank")->setCastShadows(false);
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
    sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
    sceneManager->setShadowFarDistance(250);
    sceneManager->setShadowColour(Ogre::ColourValue(0.80f, 0.80f, 0.80f));
    sceneManager->setShadowTextureSize( 2048 );
    sceneManager->setShadowTextureCount( 1 );

	// lights
    Ogre::Light* sunLight = sceneManager->createLight("sunLight");
    sunLight->setType(Ogre::Light::LT_DIRECTIONAL);
    sunLight->setCastShadows(true);
    sunLight->setDirection(Ogre::Vector3( 1, -3, 1 ));
    sunLight->setDiffuseColour(Ogre::ColourValue(1, 1, 1));
    sunLight->setSpecularColour(Ogre::ColourValue(1, 1, 1));
    sceneNode->attachObject(sunLight);

    Ogre::Light* skyLight1 = sceneManager->createLight("skyLight1");
    skyLight1->setType(Ogre::Light::LT_POINT);
    skyLight1->setCastShadows(false);
    skyLight1->setPosition(Ogre::Vector3(0, 300, 0));
    skyLight1->setDiffuseColour(Ogre::ColourValue(0.8f, 0.8f, 1.0f));
    skyLight1->setSpecularColour(Ogre::ColourValue(1, 1, 1));
    sceneNode->attachObject(skyLight1);

	playerCarView.attachTo(sceneManager, world.getPlayerCar());
   	sceneManager->getEntity("PlayerCar_lambo")->setCastShadows(false);
   	sceneManager->getEntity("PlayerCar_740sedan")->setCastShadows(false);
   	sceneManager->getEntity("PlayerCar_740kombi")->setCastShadows(false);

	//goalView.attachTo(sceneManager, "Finish", "car.mesh", world.getFinish());
	goalView.attachTo(sceneManager, "Start", "car.mesh", world.getStart());

    // Debug draw Bullet
    bulletDebugDrawer = new Rally::Util::BulletDebugDrawer(sceneManager);
    world.getPhysicsWorld().getDynamicsWorld()->setDebugDrawer(bulletDebugDrawer);

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

	lensflare = new Rally::View::LensFlareView();
	lensflare->init(sceneManager, camera, viewport->getWidth(), viewport->getHeight(), 800, 30, "LensFlareHalo", "LensFlareCircle", "LensFlareBurst");
	lensflare->setPosition(Ogre::Vector3(-800, 500, -800));
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

void SceneView::addLogicListener(Rally::View::SceneView_LogicListener& logicListener) {
    Ogre::Root::getSingleton().addFrameListener(&logicListener);
}

bool SceneView::renderFrame(float deltaTime) {
    Ogre::WindowEventUtilities::messagePump();

    if(renderWindow->isClosed())  {
        return false;
    } else {
        updatePlayerCar(deltaTime);
        updateRemoteCars();
		//updateCheckPoints();
		updateParticles();
		lensflare->update();

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
    playerCarView.setEffectFactor(playerCar.getEffectFactor());
    playerCarView.updateBody(playerCar.getPosition(), playerCar.getOrientation());
    playerCarView.updateWheels(
        playerCar.getRightFrontWheelOrientation(),
        playerCar.getLeftFrontWheelOrientation(),
        playerCar.getRightBackWheelOrientation(),
        playerCar.getLeftBackWheelOrientation());

	Rally::Vector3 currentCameraPosition = camera->getPosition();

	Rally::Vector3 displacementBase = playerCar.getOrientation() * Ogre::Vector3::UNIT_Z;
	displacementBase *= -1;

	float xzdisplacement = 5.0f;
	float ydisplacement = 3.0f;

	Rally::Vector3 displacement(
		xzdisplacement * displacementBase.x,
		ydisplacement,
		xzdisplacement * displacementBase.z);

    Rally::Vector3 endPosition = position + displacement;
    
	float velocityAdjust = playerCar.getVelocity().length()/10;
	float lerpAdjust = Ogre::Math::Clamp(velocityAdjust*deltaTime, 0.001f, 0.8f);

	// Lerp towards the new camera position to get a smoother pan
	float lerpX = Ogre::Math::lerp(currentCameraPosition.x, endPosition.x, lerpAdjust);
	float lerpY = Ogre::Math::lerp(currentCameraPosition.y, endPosition.y, lerpAdjust);
	float lerpZ = Ogre::Math::lerp(currentCameraPosition.z, endPosition.z, lerpAdjust);

	Rally::Vector3 newPos(lerpX, lerpY, lerpZ);
	Rally::Vector3 cameraPosition = newPos;

	/*
	Shoot a ray from the car to the camera.
	If anything is intersected the camera is adjusted to prevent that the camera is blocked.
	*/
	btVector3 start(position.x, position.y + 0.5f, position.z);
	btVector3 end(newPos.x, newPos.y, newPos.z);

	btCollisionWorld::ClosestRayResultCallback ClosestRayResultCallBack(start, end);

	// Perform raycast
	world.getPhysicsWorld().getDynamicsWorld()->getCollisionWorld()->rayTest(start, end, ClosestRayResultCallBack);


	if(ClosestRayResultCallBack.hasHit() &&
		ClosestRayResultCallBack.m_collisionObject->getInternalType() != btCollisionObject::CO_GHOST_OBJECT
        && ClosestRayResultCallBack.m_collisionObject->getActivationState() != 4) {

		btVector3 hitLoc = ClosestRayResultCallBack.m_hitPointWorld;

		//If the camera is blocked, the new camera is set to where the collison happened.
		cameraPosition = Rally::Vector3(hitLoc.getX(), hitLoc.getY(), hitLoc.getZ());
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

void SceneView::updateCheckPoints() {
	//goalView.update();
}

void SceneView::remoteCarUpdated(int carId, const Rally::Model::RemoteCar& remoteCar, bool carTypeChanged) {
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

    found->second.changeCar(remoteCar.getCarType());
}

void SceneView::remoteCarRemoved(int carId, const Rally::Model::RemoteCar& remoteCar) {
    remoteCarViews.erase(carId);
}

void SceneView::toggleDebugDraw() {
    debugDrawEnabled = !debugDrawEnabled;
}

void SceneView::toggleReflections() {
    playerCarView.setReflectionsOn(!playerCarView.isReflectionsOn());
}

void SceneView::togglePostProcessing() {
    postProcessingEnabled = !postProcessingEnabled;

    if(postProcessingEnabled) {
        // TODO: Fix so that setEnabled works, can't be that hard...
        /*gbufferView.setEnabled(true);
        ssaoView.setEnabled(true);
        bloomView.setEnabled(true);*/
        gbufferView.attachTo(viewport);
        gbufferView.setEnabled(true);

        ssaoView.attachTo(viewport, &world.getPlayerCar());
        ssaoView.setEnabled(true);

        bloomView.attachTo(viewport, &world.getPlayerCar());
        bloomView.setEnabled(true);

        motionBlurView.attachTo(viewport, &world.getPlayerCar());
        motionBlurView.setEnabled(true);
    } else {
        motionBlurView.detach();
        bloomView.detach();
        ssaoView.detach();
        gbufferView.detach();
        /*bloomView.setEnabled(false);
        ssaoView.setEnabled(false);
        gbufferView.setEnabled(false);*/
    }
}

void SceneView::updateParticles(){
	bool enabled[4] = {false, false, false, false};
	Rally::Vector3 positions[4];
    float tractions[4] = {1, 1, 1, 1};

	if(world.getPlayerCar().getPhysicsCar().getRightBackWheelTraction() < 0.4){
		positions[0] = world.getPlayerCar().getPhysicsCar().getRightBackWheelOrigin();
		enabled[0] = true;
        tractions[0] = world.getPlayerCar().getPhysicsCar().getRightBackWheelTraction();
	}

	if(world.getPlayerCar().getPhysicsCar().getRightFrontWheelTraction() < 0.4){
		positions[1] = world.getPlayerCar().getPhysicsCar().getRightFrontWheelOrigin();
		enabled[1] = true;
        tractions[1] = world.getPlayerCar().getPhysicsCar().getRightFrontWheelTraction();

	}


	if(world.getPlayerCar().getPhysicsCar().getLeftBackWheelTraction() < 0.4){
		positions[2] = world.getPlayerCar().getPhysicsCar().getLeftBackWheelOrigin();
		enabled[2] = true;
        tractions[2] = world.getPlayerCar().getPhysicsCar().getLeftBackWheelTraction();

	}


	if(world.getPlayerCar().getPhysicsCar().getLeftFrontWheelTraction() < 0.4){
		positions[3] = world.getPlayerCar().getPhysicsCar().getLeftFrontWheelOrigin();
		enabled[3] = true;
        tractions[3] = world.getPlayerCar().getPhysicsCar().getLeftFrontWheelTraction();
	}

	playerCarView.enableWheelParticles(enabled, positions, tractions);

}
