#include "view/CarView.h"

#include <OgreRoot.h>
#include <OgreEntity.h>

#include <sstream>
#include <string>

namespace Rally { namespace View {

    CarView::CarView() :
            sceneManager(NULL),
            carEntity(NULL),
            carNode(NULL) {
    }

    CarView::~CarView() {
        if(sceneManager != NULL) {
            sceneManager->destroySceneNode(carNode);
            sceneManager->destroyEntity(carEntity);
        }
    }

    void CarView::attachTo(Ogre::SceneManager* sceneManager, const std::string& carName) {
        this->sceneManager = sceneManager;

        carEntity = sceneManager->createEntity(carName, "car.mesh");
        carNode = sceneManager->getRootSceneNode()->createChildSceneNode();
        carNode->attachObject(carEntity);
    }

    void CarView::updateBody(Rally::Vector3 position, Rally::Quaternion orientation) {
        carNode->setPosition(position + orientation*Rally::Vector3(0, 0, -2.0f));
        carNode->setOrientation(orientation);
    }

} }
