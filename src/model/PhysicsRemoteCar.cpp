#include "model/PhysicsRemoteCar.h"

#include "model/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

#include <sstream>
#include <iostream>

namespace Rally { namespace Model {

    namespace {
        const btVector3 CAR_DIMENSIONS(2.0f, 1.0f, 4.0f);
        const float MAX_CORRECTION_DISTANCE = 10.0f;
        const float wheelRadius = 0.5f;
    }

    PhysicsRemoteCar::PhysicsRemoteCar() :
            physicsWorld(NULL),
            dynamicsWorld(NULL),
            bodyShape(NULL),
            bodyConstructionInfo(NULL),
            bodyRigidBody(NULL),
            wheelRotation(1.0f, 0, 0, 0) {
    }

    PhysicsRemoteCar::~PhysicsRemoteCar() {
        if(physicsWorld != NULL) {
            physicsWorld->unregisterStepCallback(this);
        }

        if(dynamicsWorld != NULL && bodyRigidBody != NULL) {
            dynamicsWorld->removeRigidBody(bodyRigidBody);
            delete bodyRigidBody;
        }

        // TODO: These could be shared among all remote cars...
        delete bodyConstructionInfo;
        delete bodyShape;
    }

    void PhysicsRemoteCar::initializeConstructionInfo() {
        if(bodyConstructionInfo != NULL) {
            return;
        }

        bodyShape = new btBoxShape(CAR_DIMENSIONS / 2); // Takes half-extents

        bodyConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(0, &bodyMotionState, bodyShape, btVector3(0, 0, 0));
    }

    void PhysicsRemoteCar::attachTo(PhysicsWorld& physicsWorld) {
        this->physicsWorld = &physicsWorld;
        this->dynamicsWorld = physicsWorld.getDynamicsWorld();

        // Create car body
        this->initializeConstructionInfo();
        bodyRigidBody = new btRigidBody(*bodyConstructionInfo);
        bodyRigidBody->setCollisionFlags(bodyRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        bodyRigidBody->setActivationState(DISABLE_DEACTIVATION); // So that our motion stte is queried every simulation stop
        dynamicsWorld->addRigidBody(bodyRigidBody);

        // Register for physics world updates
        physicsWorld.registerStepCallback(this);
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
        return Rally::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z());
    }

    Rally::Vector3 PhysicsRemoteCar::getVelocity() const {

        if(bodyRigidBody == NULL) {
            return Rally::Vector3(0, 0, 0);
        }

        const btVector3 velocity = bodyRigidBody->getLinearVelocity();
        return Rally::Vector3(velocity.x(), velocity.y(), velocity.z());
    }

    void PhysicsRemoteCar_BodyMotionState::setTargetTransform(const Rally::Vector3& targetPosition,
            const Rally::Vector3& incomingVelocity,
            const Rally::Quaternion& targetOrientation) {
        btVector3 currentPosition = currentTransform.getOrigin();
        Rally::Vector3 correctionDistance = targetPosition - Rally::Vector3(currentPosition.x(), currentPosition.y(), currentPosition.z());
        if(correctionDistance.length() <= MAX_CORRECTION_DISTANCE) {
            // Correction velocity is the velocity we need to go to where the physics car will be in one second,
            // in one second. It is then scaled by the length of the incoming velocity to not create jitter.
            // It effectively means that the velocity vector is steered to the correct position, interpolating it.
            Rally::Vector3 correctionVelocity = incomingVelocity + correctionDistance;
            float correctionVelocityLength = correctionVelocity.length();
            if(correctionVelocityLength > 0.001f) {
                correctionVelocity = (correctionVelocity / correctionVelocityLength) * incomingVelocity.length();
            }

            interpolationVelocity = btVector3(correctionVelocity.x, correctionVelocity.y, correctionVelocity.z);
        } else {
            // If the correction distance is to high, we just teleport instead. Probably happens:
            //  * When the simulation has just started and there is no valid position set already.
            //  * When there is a lot of network drop/lag.
            interpolationVelocity = btVector3(incomingVelocity.x, incomingVelocity.y, incomingVelocity.z);
            currentTransform.setOrigin(btVector3(targetPosition.x, targetPosition.y, targetPosition.z));
        }

        // Todo: Interpolate here
        currentTransform.setRotation(btQuaternion(targetOrientation.x, targetOrientation.y, targetOrientation.z, targetOrientation.w));
    }

    void PhysicsRemoteCar::willStep(float deltaTime) {
        // setLinearVelocity() is used by the solver for other colliding objects.
        bodyRigidBody->setLinearVelocity(bodyMotionState.interpolationVelocity);

        // Interpolate the position by integrating the velocity.
        btVector3 currentPosition = bodyMotionState.currentTransform.getOrigin();
        btVector3 deltaPosition = bodyMotionState.interpolationVelocity*deltaTime;
        bodyMotionState.currentTransform.setOrigin(currentPosition + deltaPosition);

        // Turn the wheels. We loose precision relative to the world here, not that it matters...
        float wheelRotationRadians = /* 2*PI* */ deltaPosition.length() / (/* 2*PI* */ wheelRadius);
        wheelRotation = Rally::Quaternion(Rally::Radian(wheelRotationRadians), Rally::Vector3::NEGATIVE_UNIT_X) * wheelRotation;
    }

    void PhysicsRemoteCar_BodyMotionState::getWorldTransform(btTransform& worldTransform) const {
        worldTransform = currentTransform;
    }

    void PhysicsRemoteCar_BodyMotionState::setWorldTransform(const btTransform& worldTransform) {
        std::cout << "SetWorldTransform called for remote car. Notify Joel if you see this!" << std::endl;
    }
} }
