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

		initParticleSystem(carName);
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

	void CarView::initParticleSystem(const std::string& carName){
		bodyParticleNode = sceneManager->getRootSceneNode()->createChildSceneNode();

		rightBackSystem = sceneManager->createParticleSystem(carName + "_RightBackParticleSystem", "Car/Dirt");
		leftBackSystem = sceneManager->createParticleSystem(carName + "_LeftBackParticleSystem", "Car/Dirt");		
		rightFrontSystem = sceneManager->createParticleSystem(carName + "_RightFrontParticleSystem", "Car/Dirt");
		leftFrontSystem = sceneManager->createParticleSystem(carName + "_LeftFrontParticleSystem", "Car/Dirt");
		
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
		Rally::Vector3 offset(0.f, -0.25f, 0.f);

		rightBackSystem->getEmitter(0)->setEnabled(enabled[0]);
		if(enabled[0]){
			rightBackSystem->getEmitter(0)->setPosition(position[0] + offset);
		}

		rightFrontSystem->getEmitter(0)->setEnabled(enabled[1]);
		if(enabled[1]){
			rightFrontSystem->getEmitter(0)->setPosition(position[1] + offset);
		}

		leftBackSystem->getEmitter(0)->setEnabled(enabled[2]);
		if(enabled[2]){
			leftBackSystem->getEmitter(0)->setPosition(position[2] + offset);
		}

		leftFrontSystem->getEmitter(0)->setEnabled(enabled[3]);
		if(enabled[3]){
			leftFrontSystem->getEmitter(0)->setPosition(position[3] + offset);
		}
	}
} }