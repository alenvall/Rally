#include "view/RemoteSkidmarkView.h"

#include <OgreRoot.h>
#include <OgreBillboardSet.h>
#include <OgreBillboard.h>

#include <sstream>
#include <string>

namespace Rally { namespace View {


	RemoteSkidmarkView::RemoteSkidmarkView() {}

	RemoteSkidmarkView::~RemoteSkidmarkView() {}

	void RemoteSkidmarkView::attachTo(Ogre::SceneManager* sceneManager){
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
		skidmarkBillboards->setDefaultDimensions(Ogre::Real(0.15), Ogre::Real(0.4));
		skidmarkBillboards->setAutoextend(true);

		skidmarkNode->attachObject(skidmarkBillboards);
	}

	void RemoteSkidmarkView::update(const Rally::Model::RemoteCar& car){
        float tractionReq = 0.1f;

        float speed = car.getVelocity().length();

        float heightOffset = -1.1f;
        float xOffset = 0.60f;
        float zOffsetFront = 1.50f;
        float zOffsetBack = -1.25f;

        Rally::Vector3 direction = car.getVelocity().normalisedCopy();

        Rally::Vector3 normal(0, 1, 0);
        
        Rally::Vector3 position;

        // Right front
        if(car.getTractionVector().x < tractionReq){
            position = car.getPosition() + Ogre::Vector3(xOffset, heightOffset, zOffsetFront);
            if(isWheelOnGround(position, car))
                createSkidmark(position, normal, direction, car.getTractionVector().x, speed);
        }
        // Left front
        if(car.getTractionVector().y < tractionReq){
            position = car.getPosition() + Ogre::Vector3(-xOffset, heightOffset, zOffsetFront);
            if(isWheelOnGround(position, car))
			    createSkidmark(position, normal, direction, car.getTractionVector().y, speed);
        }
        // Right back
        if(car.getTractionVector().z < tractionReq){
            position = car.getPosition() + Ogre::Vector3(xOffset, heightOffset, zOffsetBack);
            if(isWheelOnGround(position, car))
			    createSkidmark(position, normal, direction, car.getTractionVector().z, speed);
        }
        // Left back
        if(car.getTractionVector().w < tractionReq){
            position = car.getPosition() + Ogre::Vector3(-xOffset, heightOffset, zOffsetBack);
            if(isWheelOnGround(position, car))
			    createSkidmark(position, normal, direction, car.getTractionVector().w, speed);
        }
        
	}

    bool RemoteSkidmarkView::isWheelOnGround(Rally::Vector3 position, const Rally::Model::RemoteCar& car){
        btVector3 start(position.x, position.y + 0.5f, position.z);
	    btVector3 end(position.x, position.y - 1.5f, position.z);

	    btCollisionWorld::ClosestRayResultCallback ClosestRayResultCallBack(start, end);

	    // Perform raycast
        car.getPhysicsWorld().getDynamicsWorld()->getCollisionWorld()->rayTest(start, end, ClosestRayResultCallBack);

	    if(ClosestRayResultCallBack.hasHit() && ClosestRayResultCallBack.m_collisionObject->getActivationState() != 4) {
            std::cout << "raytest came out true " << std::endl;
            return true;
        } else {
            return false;
        }
    }

	void RemoteSkidmarkView::createSkidmark(Rally::Vector3 position, Rally::Vector3 normal, Rally::Vector3 direction, float traction, float speed){
		Ogre::Billboard* b = skidmarkBillboards->createBillboard(Rally::Vector3(position.x, position.y+0.05f, position.z), 
			Ogre::ColourValue::Black);
        
        double lengthAdjust = 0.045;
        float alphaAdjust = 15.0f;

        b->setDimensions(Ogre::Real(0.14), Ogre::Real(Ogre::Math::Clamp(lengthAdjust*speed, 0.2, 1.0)));
        b->setColour(Ogre::ColourValue(1.0, 1.0, 1.0, Ogre::Math::Pow(1 - traction, alphaAdjust)));

		b->mDirection = normal;

		direction.y = 0;

		Ogre::Radian r = direction.angleBetween(Rally::Vector3(0, 0, 1));

		if(direction.z > 0 && direction.x > 0){
			r =  Ogre::Radian(Ogre::Math::PI*2) - r;
		}

		if(direction.z < 0 && direction.x > 0){
			r *= -1;
		}

		b->setRotation(r);

		if(skidmarkBillboards->getNumBillboards() > 1000)
			skidmarkBillboards->removeBillboard(skidmarkBillboards->getBillboard(skidmarkBillboards->getNumBillboards()-1000));
	}

} }
