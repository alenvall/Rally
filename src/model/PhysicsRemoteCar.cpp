#include "model/PhysicsRemoteCar.h"

#include "model/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

#include <sstream>
#include <iostream>

namespace Rally { namespace Model {

    namespace {
        const btVector3 CAR_DIMENSIONS(2.0f, 1.0f, 4.0f);
    }

    PhysicsRemoteCar::PhysicsRemoteCar() :
            dynamicsWorld(NULL),
            bodyShape(NULL),
            bodyConstructionInfo(NULL),
            bodyRigidBody(NULL) {
    }

    PhysicsRemoteCar::~PhysicsRemoteCar() {
    }

    void PhysicsRemoteCar::initializeConstructionInfo() {
        if(bodyConstructionInfo != NULL) {
            return;
        }

        bodyShape = new btBoxShape(CAR_DIMENSIONS / 2); // Takes half-extents

        bodyConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(0, &bodyMotionState, bodyShape, btVector3(0, 0, 0));
    }

    void PhysicsRemoteCar::attachTo(PhysicsWorld& physicsWorld) {
        this->dynamicsWorld = physicsWorld.getDynamicsWorld();

        // Create car body
        this->initializeConstructionInfo();
        bodyRigidBody = new btRigidBody(*bodyConstructionInfo);
        bodyRigidBody->setCollisionFlags(bodyRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        bodyRigidBody->setActivationState(DISABLE_DEACTIVATION); // So that our motion stte is queried every simulation stop
        dynamicsWorld->addRigidBody(bodyRigidBody);
    }

    void PhysicsRemoteCar::setTargetTransform(const Rally::Vector3& targetPosition,
            const Rally::Vector3& incomingVelocity,
            const Rally::Quaternion& targetOrientation) {
        bodyMotionState.setTargetTransform(targetPosition, incomingVelocity, targetOrientation);
    }

    Rally::Vector3 PhysicsRemoteCar::getPosition() const {
        // Note that we cannot ask the rigidbody for its position,
        // as we won't get interpolated values then. That's motion-state exclusive info.
        const btVector3& position = bodyMotionState.currentTransform.getOrigin();
        return Rally::Vector3(position.x(), position.y(), position.z());
    }

    Rally::Quaternion PhysicsRemoteCar::getOrientation() const {
        const btQuaternion orientation = bodyMotionState.currentTransform.getRotation();
        return Rally::Quaternion(orientation.x(), orientation.y(), orientation.z(), orientation.w());
    }

    void PhysicsRemoteCar_BodyMotionState::setTargetTransform(const Rally::Vector3& targetPosition,
            const Rally::Vector3& incomingVelocity,
            const Rally::Quaternion& targetOrientation) {
        //bodyRigidBody->setLinearVelocity(incomingVelocity);
        currentTransform.setOrigin(btVector3(targetPosition.x, targetPosition.y, targetPosition.z));
        currentTransform.setRotation(btQuaternion(targetOrientation.x, targetOrientation.y, targetOrientation.z, targetOrientation.w));
    }

    void PhysicsRemoteCar_BodyMotionState::getWorldTransform(btTransform& worldTransform) const {
        worldTransform = currentTransform;
    }

    void PhysicsRemoteCar_BodyMotionState::setWorldTransform(const btTransform& worldTransform) {
        std::cout << "SetWorldTransform called for remote car. Notify Joel if you see this!" << std::endl;
    }
} }
