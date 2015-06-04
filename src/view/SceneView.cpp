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

#include <OgreOverlaySystem.h>

int Rally::View::OgreText::init=0;

SceneView::SceneView(Rally::Model::World& world) :
        world(world),
        camera(NULL),
        viewport(NULL),
        sceneManager(NULL),
        renderWindow(NULL),
        postProcessingEnabled(false),
		showKeyMenu(true),
		timeText(NULL),
		speedText(NULL),
		lastTimeText(NULL),
        highScoreText(NULL),
		kmhText(NULL),
		keyText(NULL),
		titleText(NULL),
		trackText(NULL){
    debugDrawEnabled = false;
}

SceneView::~SceneView() {
	delete timeText;
	delete speedText;
	delete lastTimeText;
	delete highScoreText;
	delete kmhText;
	delete keyText;
	delete titleText;
	delete trackText;

    //delete bulletDebugDrawer;
    //tunnelPortalView.detach();

    playerCarView.detach();

    motionBlurView.detach();
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
	sceneManager->getEntity("direction")->setCastShadows(false);
	sceneManager->getEntity("direction2")->setCastShadows(false);
	sceneManager->getEntity("direction3")->setCastShadows(false);
	sceneManager->getEntity("direction3")->setVisible(false);
	sceneManager->getEntity("direction")->setVisible(false);
	sceneManager->getEntity("direction2")->setVisible(false);
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
    sceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 0.95f));
    sceneManager->setShadowFarDistance(250);
    sceneManager->setShadowColour(Ogre::ColourValue(0.80f, 0.80f, 0.80f));
    sceneManager->setShadowTextureSize( 2048 );
    sceneManager->setShadowTextureCount( 1 );

	// lights
    Ogre::Light* sunLight = sceneManager->createLight("sunLight");
    sunLight->setType(Ogre::Light::LT_DIRECTIONAL);
    sunLight->setCastShadows(true);
    sunLight->setDirection(Ogre::Vector3( 1, -1, -1 ));
    sunLight->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 0.7f));
    sunLight->setSpecularColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
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
	goalView.attachTo(sceneManager, "Start", "carkombi.mesh", world.getStart());
	sceneManager->getEntity("Start")->setCastShadows(false);

    // Debug draw Bullet
    bulletDebugDrawer = new Rally::Util::BulletDebugDrawer(sceneManager);
    world.getPhysicsWorld().getDynamicsWorld()->setDebugDrawer(bulletDebugDrawer);

    sceneManager->setSkyDome(true, "Rally/CloudySky", 5, 8, 500);

	// Place the magic surface at the end of the tunnel.
	// tunnelPortalView.attachTo(sceneManager, "TunnelPortal");
	// tunnelPortalView.setScale(15.0f, 5.0f, true);
    // tunnelPortalView.setPosition(Rally::Vector3(86.0f, 5.0f, -134.0f));
    // tunnelPortalView.setOrientation(Rally::Quaternion(Ogre::Math::Sqrt(0.5f), 0, -Ogre::Math::Sqrt(0.5f), 0));

    // Snap a picture for the magic surface at Kopparbunken.
	//tunnelPortalView.moveCamera(
      //  Rally::Vector3(255.0f, 0.0f, 240.0f), // position
       // Rally::Vector3(255.0f, 0.0f, 239.0f)); // look at
    //tunnelPortalView.takeSnapshot();

	lensflare = new Rally::View::LensFlareView();
	lensflare->init(sceneManager, camera, viewport->getWidth(), viewport->getHeight(), 800, 30, "LensFlareHalo", "LensFlareCircle", "LensFlareBurst");
	lensflare->setPosition(Ogre::Vector3(-800, 300, 800));

	// Overlay
	Ogre::OverlaySystem* pOverlaySystem = new Ogre::OverlaySystem();
	sceneManager->addRenderQueueListener(pOverlaySystem);
	timeText=new Rally::View::OgreText;
	timeText->setText("Hello World!");    // Text to be displayed
			                               // Now it is possible to use the Ogre::String as parameter too
	timeText->setPos(0.05f,0.05f);        // Text position, using relative co-ordinates
	timeText->setCol(1.0f,1.0f,1.0f,0.5f);    // Text colour (Red, Green, Blue, Alpha)
	timeText->setTextSize(0.06f);

	speedText=new Rally::View::OgreText;
	speedText->setPos(0.75f,0.80f);
	speedText->setCol(1.0f,0.0f,0.0f,0.7f);
	speedText->setTextSize(0.17f);

	kmhText=new Rally::View::OgreText;
	kmhText->setText("km/h");
	kmhText->setPos(0.85f,0.83f);
	kmhText->setCol(1.0f,1.0f,1.0f,0.5f);
	kmhText->setTextSize(0.06f);

	lastTimeText=new Rally::View::OgreText;
	lastTimeText->setPos(0.05f,0.85f);
	lastTimeText->setCol(1.0f,1.0f,1.0f,0.5f);
	lastTimeText->setTextSize(0.06f);

	highScoreText=new Rally::View::OgreText;
	highScoreText->setPos(0.05f,0.80f);
	highScoreText->setCol(1.0f,1.0f,1.0f,0.5f);
	highScoreText->setTextSize(0.06f);

	keyText=new Rally::View::OgreText;
	keyText->setPos(0.05f,0.70f);
	keyText->setCol(1.0f,1.0f,1.0f,0.9f);
	keyText->setTextSize(0.06f);

	titleText=new Rally::View::OgreText;
	titleText->setPos(0.05f,0.16f);
	titleText->setCol(1.0f,0.0f,0.0f,0.9f);
	titleText->setTextSize(0.25f);

	trackText=new Rally::View::OgreText;
	trackText->setPos(0.75f,0.05f);
	trackText->setCol(1.0f,1.0f,1.0f,0.5f);
	trackText->setTextSize(0.06f);

	SceneView::toggleKeyMenu();
    toggleReflections();
    togglePostProcessing();
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
		updateOverlay();
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

void SceneView::toggleKeyMenu(){
	showKeyMenu = !showKeyMenu;

	if(!showKeyMenu){
		titleText->setText("  Johannebergs-\n     rallyt");
		keyText->setText("  Recommended steering: arrows and x / Alternative steering: WASD\n      Toggle car: space / Float: f / Laggy? Lower graphics: P, O\n               Press K to continue, T to start a race!");
		timeText->setCol(0.0f,0.0f,0.0f,0.0f);
		speedText->setCol(0.0f,0.0f,0.0f,0.0f);
		lastTimeText->setCol(0.0f,0.0f,0.0f,0.0f);
		highScoreText->setCol(0.0f,0.0f,0.0f,0.0f);
		kmhText->setCol(0.0f,0.0f,0.0f,0.0f);
	} else {
		keyText->setText("");
		titleText->setText("");
		speedText->setCol(1.0f,0.0f,0.0f,0.7f);
		kmhText->setCol(1.0f,1.0f,1.0f,0.5f);
	}
}

void SceneView::toggleTrack(){

	if(sceneManager->getEntity("direction")->isVisible()){
		sceneManager->getEntity("direction")->setVisible(false);
		sceneManager->getEntity("direction2")->setVisible(true);
		lastTimeText->setCol(1.0f,1.0f,1.0f,0.5f);
		highScoreText->setCol(1.0f,1.0f,1.0f,0.5f);
		timeText->setCol(1.0f,1.0f,1.0f,0.5f);
		trackText->setText("Track 2 \nquick round");
	} else if(sceneManager->getEntity("direction2")->isVisible()){
	    sceneManager->getEntity("direction2")->setVisible(false);
		sceneManager->getEntity("direction3")->setVisible(true);
		lastTimeText->setCol(1.0f,1.0f,1.0f,0.5f);
		highScoreText->setCol(1.0f,1.0f,1.0f,0.5f);
		timeText->setCol(1.0f,1.0f,1.0f,0.5f);
		trackText->setText("Track 3 \nthe longest road");
	} else if(sceneManager->getEntity("direction3")->isVisible()){
	    sceneManager->getEntity("direction3")->setVisible(false);
		lastTimeText->setCol(0.0f,0.0f,0.0f,0.0f);
		highScoreText->setCol(0.0f,0.0f,0.0f,0.0f);
		timeText->setCol(0.0f,0.0f,0.0f,0.0f);
		trackText->setText("");
	} else {
		sceneManager->getEntity("direction")->setVisible(true);
		lastTimeText->setCol(1.0f,1.0f,1.0f,0.5f);
		highScoreText->setCol(1.0f,1.0f,1.0f,0.5f);
		timeText->setCol(1.0f,1.0f,1.0f,0.5f);
		trackText->setText("track 1 \nnumber one");

	}
}

void SceneView::updateParticles(){
	bool enabled[4] = {false, false, false, false};
	Rally::Vector3 positions[4];
    float tractions[4] = {1, 1, 1, 1};

    float tractionMin = 0.8;

	if(world.getPlayerCar().getPhysicsCar().getRightBackWheelTraction() < tractionMin){
		positions[0] = world.getPlayerCar().getPhysicsCar().getRightBackWheelOrigin();
		enabled[0] = true;
        tractions[0] = world.getPlayerCar().getPhysicsCar().getRightBackWheelTraction();
	}

	if(world.getPlayerCar().getPhysicsCar().getRightFrontWheelTraction() < tractionMin){
		positions[1] = world.getPlayerCar().getPhysicsCar().getRightFrontWheelOrigin();
		enabled[1] = true;
        tractions[1] = world.getPlayerCar().getPhysicsCar().getRightFrontWheelTraction();

	}


	if(world.getPlayerCar().getPhysicsCar().getLeftBackWheelTraction() < tractionMin){
		positions[2] = world.getPlayerCar().getPhysicsCar().getLeftBackWheelOrigin();
		enabled[2] = true;
        tractions[2] = world.getPlayerCar().getPhysicsCar().getLeftBackWheelTraction();

	}


	if(world.getPlayerCar().getPhysicsCar().getLeftFrontWheelTraction() < tractionMin){
		positions[3] = world.getPlayerCar().getPhysicsCar().getLeftFrontWheelOrigin();
		enabled[3] = true;
        tractions[3] = world.getPlayerCar().getPhysicsCar().getLeftFrontWheelTraction();
	}

	playerCarView.enableWheelParticles(enabled, positions, tractions);

}

void SceneView::updateOverlay(){
	std::ostringstream timeOut;
	std::ostringstream speedOut;
	std::ostringstream lastTimeOut;
	std::ostringstream highScoreOut;
	timeOut << "Time " << world.getElapsedSeconds(); //world.getPlayerCar().getEffectFactor();
	timeText->setText(timeOut.str());
	speedOut << (int)(world.getPlayerCar().getVelocity().length() * 3.6);
	speedText->setText(speedOut.str());
	lastTimeOut << "Last time: " << world.getLastTime();
	lastTimeText->setText(lastTimeOut.str());
	highScoreOut << "High Score: " << world.getHighScore();
	if(world.getHighScore() == 200 || world.getHighScore() == 199)
		highScoreText->setText("High Score:");
	else
	    highScoreText->setText(highScoreOut.str());

}
