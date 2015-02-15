#include "model/PhysicsCar.h"

#include "model/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

#include <sstream>
#include <iostream>

namespace Rally { namespace Model {

    namespace {
        const btVector3 CAR_DIMENSIONS(2.0f, 1.0f, 4.0f);
        const float CAR_MASS = 1.0f;

        // The rolling torque from each wheel is scaled with this (to prevent rollover).
        // 0.0f = no rolling, 1.0f = rolling like in reality.
        const float ROLL_INFLUENCE = 0.1f;

        const float SUSPENSION_REST_LENGTH = 0.6f; // (see also maxSuspensionTravelCm)
        const float WHEEL_RADIUS = 0.5f;
        // (There is no wheel width.)

        // The wheel distance is calculated from the origin = center of the car.
        // The wheel (including radius) should be located inside the car body:
        //  - Below the car and it might sink into the ground (rays goes into infinity)
        //  - Above the car and it confuses the body with the ground => weird behavior.
        //    (The official demo does this though... Maybe the suspension is included?)
        // Values below represent the right wheel (mirrored in the yz-plane for left).
        const btVector3 FRONT_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 - 0.1f, 0.f, CAR_DIMENSIONS.z()/2 - 0.3f - WHEEL_RADIUS);
        const btVector3 BACK_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 - 0.1f, 0.f, CAR_DIMENSIONS.z()/2 - 0.1f - WHEEL_RADIUS);
    }

    PhysicsCar::PhysicsCar() :
            dynamicsWorld(NULL),
            bodyShape(NULL),
            bodyMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 5.0f, 0.0f))),
            bodyConstructionInfo(NULL),
            bodyRigidBody(NULL),
            vehicleRaycaster(NULL),
            raycastVehicle(NULL),
            leftFrontWheel(NULL),
            rightFrontWheel(NULL),
            leftBackWheel(NULL),
            rightBackWheel(NULL) {
    }

    PhysicsCar::~PhysicsCar() {
    }

    void PhysicsCar::initializeConstructionInfo() {
        if(bodyConstructionInfo != NULL) {
            return;
        }

        bodyShape = new btBoxShape(CAR_DIMENSIONS / 2); // Takes half-extents

        // Initialize what I think is the diagonal in the inertia tensor
        btVector3 inertia(0, 0, 0);
        bodyShape->calculateLocalInertia(CAR_MASS, inertia); // TODO: We might want to lower/move center of gravity...

        bodyConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(CAR_MASS, &bodyMotionState, bodyShape, inertia);
    }

    void PhysicsCar::attachTo(PhysicsWorld& physicsWorld) {
        dynamicsWorld = physicsWorld.getDynamicsWorld();

        // Create car body
        this->initializeConstructionInfo();
        bodyRigidBody = new btRigidBody(*bodyConstructionInfo);
        bodyRigidBody->setActivationState(DISABLE_DEACTIVATION); // Needed for btRaycastVehicles
        dynamicsWorld->addRigidBody(bodyRigidBody);

        // Tuning for car (applied to all the wheels internally).
        btRaycastVehicle::btVehicleTuning tuning;
        tuning.m_suspensionStiffness = 20.0f;
        tuning.m_suspensionDamping = 2.3f;
        tuning.m_suspensionCompression = 4.4f;
        tuning.m_frictionSlip = 0.8f;// no drift = 1000.0f;
        // rollInfluence cannot be set here, so we have to apply it to every wheel individually.

        // Create the raycasting part for the "wheels"
        vehicleRaycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
        raycastVehicle = new btRaycastVehicle(tuning, bodyRigidBody, vehicleRaycaster);
        dynamicsWorld->addVehicle(raycastVehicle);

        // Create the actual wheels:
        const btVector3 wheelDirection(0, -1.0f, 0); // This is the direction of the raycast.
        const btVector3 wheelAxle(-1.0f, 0, 0); // This is spinning direction (using right hand rule).

        // Right front wheel.
        rightFrontWheel = &raycastVehicle->addWheel(
            FRONT_WHEEL_DISTANCE, // connection point
            wheelDirection, // wheel direction
            wheelAxle, // axle
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            true // isFrontWheel
        );
        rightFrontWheel->m_rollInfluence = ROLL_INFLUENCE;

        // Left front wheel.
        leftFrontWheel = &raycastVehicle->addWheel(
            btVector3(-FRONT_WHEEL_DISTANCE.x(), FRONT_WHEEL_DISTANCE.y(), -FRONT_WHEEL_DISTANCE.z()), // connection point
            wheelDirection, // wheel direction
            wheelAxle, // axle
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            true // isFrontWheel
        );
        leftFrontWheel->m_rollInfluence = ROLL_INFLUENCE;

        // Right back wheel.
        rightBackWheel = &raycastVehicle->addWheel(
            BACK_WHEEL_DISTANCE, // connection point
            wheelDirection, // wheel direction
            wheelAxle, // axle
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            false // isFrontWheel
        );
        rightBackWheel->m_rollInfluence = ROLL_INFLUENCE;

        // Left back wheel.
        leftBackWheel = &raycastVehicle->addWheel(
            btVector3(-BACK_WHEEL_DISTANCE.x(), BACK_WHEEL_DISTANCE.y(), -BACK_WHEEL_DISTANCE.z()), // connection point
            wheelDirection, // wheel direction
            wheelAxle, // axle
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            false // isFrontWheel
        );
        leftBackWheel->m_rollInfluence = ROLL_INFLUENCE;
    }

    Rally::Vector3 PhysicsCar::getPosition() const {
        // Note that we cannot ask the rigidbody or use raycastVehicle->getForwardVector(),
        // as we won't get interpolated values then. That's motion-state exclusive info.
        const btTransform& graphicsTransform = bodyMotionState.m_graphicsWorldTrans;
        const btVector3& position = graphicsTransform.getOrigin();
        return Rally::Vector3(position.x(), position.y(), position.z());
    }

    Rally::Vector3 PhysicsCar::getOrientation() const {
        const btTransform& graphicsTransform = bodyMotionState.m_graphicsWorldTrans;
        return Rally::Vector3(
            graphicsTransform.getBasis()[0][2], // The z-column basis vector
            graphicsTransform.getBasis()[1][2], // The z-column basis vector
            graphicsTransform.getBasis()[2][2] // The z-column basis vector
        );
        //const btQuaternion orientation = graphicsTransform.getRotation();
        //return Rally::Quaternion(orientation.x(), orientation.y(), orientation.z(), orientation.w());
    }

    Rally::Vector3 PhysicsCar::getVelocity() const {
        if(bodyRigidBody == NULL) {
            return Rally::Vector3(0, 0, 0);
        }

        const btVector3 velocity = bodyRigidBody->getLinearVelocity();
        return Rally::Vector3(velocity.x(), velocity.y(), velocity.z());
    }

    float PhysicsCar::getRightFrontWheelTraction() const {
        return rightFrontWheel->m_skidInfo;
    }

    float PhysicsCar::getLeftFrontWheelTraction() const {
        return leftFrontWheel->m_skidInfo;
    }

    float PhysicsCar::getRightBackWheelTraction() const {
        return rightBackWheel->m_skidInfo;
    }

    float PhysicsCar::getLeftBackWheelTraction() const {
        return leftBackWheel->m_skidInfo;
    }

} }
