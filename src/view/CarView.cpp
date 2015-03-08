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
} }
