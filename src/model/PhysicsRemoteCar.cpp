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
            bodyMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 10.0f, 0.0f))),
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

    Rally::Vector3 PhysicsRemoteCar::getPosition() const {
        // Note that we cannot ask the rigidbody for its position,
        // as we won't get interpolated values then. That's motion-state exclusive info.
        const btTransform& graphicsTransform = bodyMotionState.m_graphicsWorldTrans;
        const btVector3& position = graphicsTransform.getOrigin();
        return Rally::Vector3(position.x(), position.y(), position.z());
    }

    Rally::Quaternion PhysicsRemoteCar::getOrientation() const {
        const btTransform& graphicsTransform = bodyMotionState.m_graphicsWorldTrans;
        const btQuaternion orientation = graphicsTransform.getRotation();
        return Rally::Quaternion(orientation.x(), orientation.y(), orientation.z(), orientation.w());
    }

    Rally::Vector3 PhysicsRemoteCar::getVelocity() const {
        if(bodyRigidBody == NULL) {
            return Rally::Vector3(0, 0, 0);
        }

        const btVector3 velocity = bodyRigidBody->getLinearVelocity();
        return Rally::Vector3(velocity.x(), velocity.y(), velocity.z());
    }
} }
