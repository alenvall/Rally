#include "view/CarView.h"

#include <OgreRoot.h>
#include <OgreEntity.h>

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

	void CarView::activateParticles(Rally::Vector3 position, int intensity){
		//ugly removal of emitters to prevetn lag
		/*
		if(particleSystem->getNumEmitters() > 100)
			for(int i = 0; i < particleSystem->getNumEmitters(); i++)
				particleSystem->removeEmitter(0);
		*/

		particleSystem->addEmitter("Point")->setDuration(2);
		particleSystem->getEmitter(particleSystem->getNumEmitters()-1)->setPosition(position);
		std::cout << particleSystem->getEmitter(particleSystem->getNumEmitters()-1)->getPosition() << " : " << position << std::endl;
	
	}

	void CarView::enableWheelParticles(bool rightBack, bool rightFront, bool leftBack, bool leftFront){
		rightBackSystem->getEmitter(0)->setEnabled(rightBack);
		rightFrontSystem->getEmitter(0)->setEnabled(rightFront);
		leftBackSystem->getEmitter(0)->setEnabled(leftBack);
		leftFrontSystem->getEmitter(0)->setEnabled(leftFront);
	}
} }
