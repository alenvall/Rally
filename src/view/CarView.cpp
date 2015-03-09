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
		carEntity->getSubEntity(4)->setMaterialName("carcolourred");
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
		
		rightBackSystem = sceneManager->createParticleSystem("Wheel_rightback", "Car/Dirt");

		leftBackSystem = sceneManager->createParticleSystem("Wheel_leftback", "Car/Dirt");
		
		rightFrontSystem = sceneManager->createParticleSystem("Wheel_rightfront", "Car/Dirt");

		leftFrontSystem = sceneManager->createParticleSystem("Wheel_leftfront", "Car/Dirt");


		bodyParticleNode->attachObject(rightBackSystem);
		bodyParticleNode->attachObject(leftBackSystem);
		bodyParticleNode->attachObject(rightFrontSystem);
		bodyParticleNode->attachObject(leftFrontSystem);

		rightBackSystem->getEmitter(0)->setEnabled(false);
		leftBackSystem->getEmitter(0)->setEnabled(false);
		rightFrontSystem->getEmitter(0)->setEnabled(false);
		leftFrontSystem->getEmitter(0)->setEnabled(false);
	}

	void CarView::enableWheelParticles(bool enabled[], Rally::Vector3 position[]){
		Ogre::Real low(0.3);
		Ogre::Real high(0.7);

		Rally::Vector3 offset(0.f, -0.25f, 0.f);

		rightBackSystem->getEmitter(0)->setEnabled(enabled[0]);
		if(enabled[0]){
			rightBackSystem->getEmitter(0)->setPosition(position[0] + offset);
			rightBackSystem->getEmitter(0)->setDirection(Rally::Vector3(Ogre::Math::RangeRandom(low, high), 0.5, Ogre::Math::RangeRandom(low, high)));
		}

		rightFrontSystem->getEmitter(0)->setEnabled(enabled[1]);
		if(enabled[1]){
			rightFrontSystem->getEmitter(0)->setPosition(position[1] + offset);
			rightFrontSystem->getEmitter(0)->setDirection(Rally::Vector3(Ogre::Math::RangeRandom(low, high), 0.5, Ogre::Math::RangeRandom(low, high)));
		}

		leftBackSystem->getEmitter(0)->setEnabled(enabled[2]);
		if(enabled[2]){
			leftBackSystem->getEmitter(0)->setPosition(position[2] + offset);
			leftBackSystem->getEmitter(0)->setDirection(Rally::Vector3(Ogre::Math::RangeRandom(low, high), 0.5, Ogre::Math::RangeRandom(low, high)));
		}

		leftFrontSystem->getEmitter(0)->setEnabled(enabled[3]);
		if(enabled[3]){
			leftFrontSystem->getEmitter(0)->setPosition(position[3] + offset);
			leftFrontSystem->getEmitter(0)->setDirection(Rally::Vector3(Ogre::Math::RangeRandom(low, high), 1, Ogre::Math::RangeRandom(low, high)));
		}
	}

	void CarView::initSkidmarks(){

		skidmarkNode = sceneManager->getRootSceneNode()->createChildSceneNode();

		Ogre::BillboardType type = Ogre::BillboardType::BBT_PERPENDICULAR_SELF;
		Ogre::BillboardRotationType rotationType = Ogre::BillboardRotationType::BBR_VERTEX;
		Rally::Vector3 up(0, 0, 1);
		Rally::Vector3 common(0, 1, 0);

		skidmarkBillboards = sceneManager->createBillboardSet();
		skidmarkBillboards->setMaterialName("skidmark");
		skidmarkBillboards->setVisible(true);
		skidmarkBillboards->setBillboardType(type);
		skidmarkBillboards->setBillboardRotationType(rotationType);
		skidmarkBillboards->setCommonUpVector(up);
		skidmarkBillboards->setCommonDirection(common);
		skidmarkBillboards->setBillboardsInWorldSpace(true);
		skidmarkBillboards->setDefaultDimensions(Ogre::Real(0.25), Ogre::Real(0.25));
		skidmarkBillboards->setAutoextend(true);
		skidmarkBillboards->setRenderingDistance(100);

		skidmarkNode->attachObject(skidmarkBillboards);

		skidcounter = 0;
	}

	void CarView::updateSkidmarks(Rally::Vector3 position, Rally::Vector3 normal, float speed){
	
		Ogre::Billboard* b = skidmarkBillboards->createBillboard(Rally::Vector3(position.x, position.y+0.05, position.z), 
			Ogre::ColourValue::Black);
		
		
		b->mDirection = normal;
		
		if(skidmarkBillboards->getNumBillboards() > 600)
			skidmarkBillboards->removeBillboard(skidmarkBillboards->getBillboard(skidmarkBillboards->getNumBillboards()-600));
	}
} }
