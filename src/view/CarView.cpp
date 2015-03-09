#include "view/CarView.h"

#include <OgreRoot.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>

#include <sstream>
#include <string>

namespace Rally { namespace View {

	CarView::CarView() :
		sceneManager(NULL),
		carEntity(NULL),
		carNode(NULL),
		rightFrontWheelEntity(NULL),
		leftFrontWheelEntity(NULL),
		rightBackWheelEntity(NULL),
		leftBackWheelEntity(NULL),
		rightFrontWheelNode(NULL),
		leftFrontWheelNode(NULL),
		rightBackWheelNode(NULL),
		leftBackWheelNode(NULL) {
	}

	CarView::~CarView() {
	}

	void CarView::detach() {
		if(sceneManager != NULL) {
			sceneManager->destroySceneNode(rightFrontWheelNode);
			sceneManager->destroySceneNode(leftFrontWheelNode);
			sceneManager->destroySceneNode(rightBackWheelNode);
			sceneManager->destroySceneNode(leftBackWheelNode);

			sceneManager->destroyEntity(rightFrontWheelEntity);
			sceneManager->destroyEntity(leftFrontWheelEntity);
			sceneManager->destroyEntity(rightBackWheelEntity);
			sceneManager->destroyEntity(leftBackWheelEntity);

			sceneManager->destroySceneNode(carNode);
			sceneManager->destroyEntity(carEntity);
		}
	}

	void CarView::attachTo(Ogre::SceneManager* sceneManager, const std::string& carName) {
		this->sceneManager = sceneManager;

		carEntity = sceneManager->createEntity(carName, "car.mesh");
		carEntity->getSubEntity(4)->setMaterialName("carcolourwhite");
		carNode = sceneManager->getRootSceneNode()->createChildSceneNode();
		carNode->attachObject(carEntity);

		rightFrontWheelNode = carNode->createChildSceneNode(Ogre::Vector3(0.7f, -0.5f, 1.50f), Ogre::Quaternion::IDENTITY);
		leftFrontWheelNode = carNode->createChildSceneNode(Ogre::Vector3(-0.7f, -0.5f, 1.50f), Ogre::Quaternion::IDENTITY);
		rightBackWheelNode = carNode->createChildSceneNode(Ogre::Vector3(0.7f, -0.5f, -1.25f), Ogre::Quaternion::IDENTITY);
		leftBackWheelNode = carNode->createChildSceneNode(Ogre::Vector3(-0.7f, -0.5f, -1.25f), Ogre::Quaternion::IDENTITY);

		rightFrontWheelEntity = sceneManager->createEntity(carName + "_RightFrontWheel", "hjul.mesh");
		leftFrontWheelEntity = sceneManager->createEntity(carName + "_LeftFrontWheel", "hjul.mesh");
		rightBackWheelEntity = sceneManager->createEntity(carName + "_RightBackWheel", "hjul.mesh");
		leftBackWheelEntity = sceneManager->createEntity(carName + "_LeftBackWheel", "hjul.mesh");

		rightFrontWheelNode->attachObject(rightFrontWheelEntity);
		leftFrontWheelNode->attachObject(leftFrontWheelEntity);
		rightBackWheelNode->attachObject(rightBackWheelEntity);
		leftBackWheelNode->attachObject(leftBackWheelEntity);

		initParticleSystem();
		initSkidmarks();
	}

	void CarView::updateBody(const Rally::Vector3& position, const Rally::Quaternion& orientation) {
		carNode->setPosition(position);
		carNode->setOrientation(orientation);
	}

	void CarView::updateWheels(
		const Rally::Quaternion& rightFrontWheelOrientation,
		const Rally::Quaternion& leftFrontWheelOrientation,
		const Rally::Quaternion& rightBackWheelOrientation,
		const Rally::Quaternion& leftBackWheelOrientation) {
			Rally::Quaternion localCompensation = carNode->getOrientation().Inverse();
			rightFrontWheelNode->setOrientation(localCompensation*rightFrontWheelOrientation);
			leftFrontWheelNode->setOrientation(localCompensation*leftFrontWheelOrientation);
			rightBackWheelNode->setOrientation(localCompensation*rightBackWheelOrientation);
			leftBackWheelNode->setOrientation(localCompensation*leftBackWheelOrientation);
	}

	void CarView::initParticleSystem(){
		bodyParticleNode = sceneManager->getRootSceneNode()->createChildSceneNode();
		particleSystem = sceneManager->createParticleSystem("Sun", "Space/Sun");
		bodyParticleNode->attachObject(particleSystem);

		rightBackSystem = sceneManager->createParticleSystem("Wheel_rightback", "Space/Sun");
		leftBackSystem = sceneManager->createParticleSystem("Wheel_leftback", "Space/Sun");
		rightFrontSystem = sceneManager->createParticleSystem("Wheel_rightfront", "Space/Sun");
		leftFrontSystem = sceneManager->createParticleSystem("Wheel_leftfront", "Space/Sun");

		rightBackWheelNode->attachObject(rightBackSystem);
		leftBackWheelNode->attachObject(leftBackSystem);
		rightFrontWheelNode->attachObject(rightFrontSystem);
		leftFrontWheelNode->attachObject(leftFrontSystem);

		rightBackSystem->getEmitter(0)->setEnabled(false);
		leftBackSystem->getEmitter(0)->setEnabled(false);
		rightFrontSystem->getEmitter(0)->setEnabled(false);
		leftFrontSystem->getEmitter(0)->setEnabled(false);
	}

	void CarView::updateParticles(Rally::Vector3 position, int intensity){
		int hello = particleSystem->getNumEmitters();
		//ugly removal of emitters to prevent lag
		if(particleSystem->getNumEmitters() > 50)
			for(int i = 0; i < particleSystem->getNumEmitters(); i++)
				particleSystem->removeEmitter(i);

		particleSystem->addEmitter("Point")->setDuration(2);
		particleSystem->getEmitter(particleSystem->getNumEmitters()-1)->setPosition(position);

	}

	void CarView::enableWheelParticles(bool rightBack, bool rightFront, bool leftBack, bool leftFront){
		rightBackSystem->getEmitter(0)->setEnabled(rightBack);
		rightFrontSystem->getEmitter(0)->setEnabled(rightFront);
		leftBackSystem->getEmitter(0)->setEnabled(leftBack);
		leftFrontSystem->getEmitter(0)->setEnabled(leftFront);
	}

	void CarView::initSkidmarks(){

		skidmarkNode = sceneManager->getRootSceneNode()->createChildSceneNode();

		Ogre::BillboardType type = Ogre::BillboardType::BBT_PERPENDICULAR_SELF;
		Ogre::BillboardRotationType rotationType = Ogre::BillboardRotationType::BBR_VERTEX;
		Rally::Vector3 up(0, 0, 1);
		Rally::Vector3 common(0, 1, 0);

		skidmarkBillboards = sceneManager->createBillboardSet();
		skidmarkBillboards->setMaterialName("tiremark");
		skidmarkBillboards->setVisible(true);
		skidmarkBillboards->setBillboardType(type);
		skidmarkBillboards->setBillboardRotationType(rotationType);
		skidmarkBillboards->setCommonUpVector(up);
		skidmarkBillboards->setCommonDirection(common);
		skidmarkBillboards->setBillboardsInWorldSpace(true);
		skidmarkBillboards->setDefaultDimensions(Ogre::Real(0.25), Ogre::Real(0.25));
		skidmarkBillboards->setAutoextend(true);

		skidmarkNode->attachObject(skidmarkBillboards);
	}

	void CarView::updateSkidmarks(Rally::Vector3 position, Rally::Vector3 normal, float speed){
		Ogre::Billboard* b = skidmarkBillboards->createBillboard(Rally::Vector3(position.x, position.y+0.05, position.z), 
			Ogre::ColourValue::Black);
		
		b->mDirection = normal;

		float speedEffect = 0.5 - 0.5/(speed+0.01);
		b->setColour(Ogre::ColourValue(0, 0, 0, speed));
		
		if(skidmarkBillboards->getNumBillboards() > 2500)
			skidmarkBillboards->removeBillboard(skidmarkBillboards->getBillboard(skidmarkBillboards->getNumBillboards()-2500));
	}
} }
