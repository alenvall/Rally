#include "view/CheckpointView.h"

#include <OgreRoot.h>
#include <OgreEntity.h>

#include <sstream>
#include <string>

namespace Rally { namespace View {

    CheckpointView::CheckpointView() :
            sceneManager(NULL),
            checkPointEntity(NULL),
            checkPointNode(NULL),
			checkPoint(NULL) {
    }

    CheckpointView::~CheckpointView() {
        if(sceneManager != NULL) {
            sceneManager->destroySceneNode(checkPointNode);
            sceneManager->destroyEntity(checkPointEntity);
        }
    }

    void CheckpointView::attachTo(Ogre::SceneManager* sceneManager, const std::string& checkPointName, const std::string& meshName, Rally::Model::Checkpoint& checkPoint) {
        this->sceneManager = sceneManager;
		this->checkPoint = &checkPoint;
		
        checkPointEntity = sceneManager->createEntity(checkPointName, meshName);
        checkPointNode = sceneManager->getRootSceneNode()->createChildSceneNode();
        checkPointNode->attachObject(checkPointEntity);
        checkPointNode->setPosition(this->checkPoint->getPosition());
        checkPointNode->setOrientation(this->checkPoint->getOrientation());
    }
	
    void CheckpointView::update() {
        checkPointNode->setPosition(checkPoint->getPosition());
        checkPointNode->setOrientation(checkPoint->getOrientation());
    }

} }
