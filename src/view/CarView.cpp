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

        carEntities.clear();
    }

    void CarView::attachTo(Ogre::SceneManager* sceneManager, const std::string& carName) {
        this->sceneManager = sceneManager;

        carEntities.push_back(sceneManager->createEntity(carName + "_lambo", "car.mesh"));
        carEntities.push_back(sceneManager->createEntity(carName + "_740sedan", "car740.mesh"));
        carEntities.push_back(sceneManager->createEntity(carName + "_740kombi", "car740.mesh"));
        carEntity = carEntities[0];

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

    void CarView::changeCar(char carType) {
        if(carType < 'a' || carType > 'r') {
            // Don't trust the network...
            return;
        }

        carNode->detachObject(carEntity);
        switch(carType) {
            case 'a':
            case 'd':
            case 'g':
            case 'j':
            case 'm':
            case 'p':
                carEntity = carEntities[0];
            break;

            case 'b':
            case 'e':
            case 'h':
            case 'k':
            case 'n':
            case 'q':
                carEntity = carEntities[1];
            break;

            case 'c':
            case 'f':
            case 'i':
            case 'l':
            case 'o':
            case 'r':
                carEntity = carEntities[2];
            break;
        }
        carNode->attachObject(carEntity);

        switch(carType) {
            case 'a':
            case 'b':
            case 'c':
                carEntity->getSubEntity(4)->setMaterialName("carcolourred");
            break;

            case 'd':
            case 'e':
            case 'f':
                carEntity->getSubEntity(4)->setMaterialName("carcolourgustavred");
            break;

            case 'g':
            case 'h':
            case 'i':
                carEntity->getSubEntity(4)->setMaterialName("carcolourhotpink");
            break;

            case 'j':
            case 'k':
            case 'l':
                carEntity->getSubEntity(4)->setMaterialName("carcolourblue");
            break;

            case 'm':
            case 'n':
            case 'o':
                carEntity->getSubEntity(4)->setMaterialName("carcolourwhite");
            break;

            case 'p':
            case 'q':
            case 'r':
                carEntity->getSubEntity(4)->setMaterialName("carcolouryellow");
            break;
        }
    }
} }
