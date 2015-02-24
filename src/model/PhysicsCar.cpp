#include "model/PhysicsCar.h"

#include "model/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

#include <sstream>
#include <iostream>

namespace Rally { namespace Model {

    namespace {
        const btVector3 CAR_DIMENSIONS(2.0f, 1.0f, 4.0f);
        const float CAR_MASS = 800.0f;

        // The rolling torque from each wheel is scaled with this (to prevent rollover).
        // 0.0f = no rolling, 1.0f = rolling like in reality.
        const float ROLL_INFLUENCE = 0.1f;

        const float SUSPENSION_REST_LENGTH = 0.6f; // (see also maxSuspensionTravelCm)
        const float WHEEL_RADIUS = 0.5f;
        const float WHEEL_GROUND_FRICTION = 0.8f;
        // (There is no wheel width.)

        const float ENGINE_FORCE = -2000.0f;
        const float ENGINE_REVERSE_FORCE = 1500.0f;
        const float BREAKING_FORCE = 100.0f;

        const float MAX_STEERING = 0.9f;
        const float STEERING_INCREASE = 5.0f; // [same unit as steering] per second

        // The wheel distance is calculated from the origin = center of the car.
        // The wheel (including radius) should be located inside the car body:
        //  - Below the car and it might sink into the ground (rays goes into infinity)
        //  - Above the car and it confuses the body with the ground => weird behavior.
        //    (The official demo does this though... Maybe the suspension is included?)
        // Values below represent the right wheel (mirrored in the yz-plane for left).
        const btVector3 FRONT_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 + 0.1f, 0.f, -(CAR_DIMENSIONS.z()/2 - 0.3f - WHEEL_RADIUS));
        const btVector3 BACK_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 + 0.1f, 0.f, CAR_DIMENSIONS.z()/2 - 0.1f - WHEEL_RADIUS);
    }

    PhysicsCar::PhysicsCar() :
            dynamicsWorld(NULL),
            bodyShape(NULL),
            bodyMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(-50.0f, 1.2f, 40.0f))),
            bodyConstructionInfo(NULL),
            bodyRigidBody(NULL),
            vehicleRaycaster(NULL),
            raycastVehicle(NULL),
            leftFrontWheel(NULL),
            rightFrontWheel(NULL),
            leftBackWheel(NULL),
            rightBackWheel(NULL),
            steering(0),
            accelerationRequested(false),
            breakingRequested(false),
            steeringRequested(0) {
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
        tuning.m_frictionSlip = WHEEL_GROUND_FRICTION;// no drift = 1000.0f;
        // rollInfluence cannot be set here, so we have to apply it to every wheel individually.

        // Create the raycasting part for the "wheels"
        vehicleRaycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
        raycastVehicle = new btRaycastVehicle(tuning, bodyRigidBody, vehicleRaycaster);
        dynamicsWorld->addVehicle(raycastVehicle);

        // Create the actual wheels:
        const btVector3 wheelDirection(0, -1.0f, 0); // This is the direction of the raycast.
        const btVector3 wheelAxle(-1.0f, 0, 0); // This is spinning direction (using right hand rule).

        // Right front wheel.
        raycastVehicle->addWheel(
            FRONT_WHEEL_DISTANCE, // connection point
            wheelDirection, // wheel direction
            wheelAxle, // axle
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            true // isFrontWheel
        );
        rightFrontWheel = &raycastVehicle->getWheelInfo(0);
        rightFrontWheel->m_rollInfluence = ROLL_INFLUENCE;

        // Left front wheel.
        raycastVehicle->addWheel(
            btVector3(-FRONT_WHEEL_DISTANCE.x(), FRONT_WHEEL_DISTANCE.y(), FRONT_WHEEL_DISTANCE.z()), // connection point
            wheelDirection, // wheel direction
            wheelAxle, // axle
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            true // isFrontWheel
        );
        leftFrontWheel = &raycastVehicle->getWheelInfo(1);
        leftFrontWheel->m_rollInfluence = ROLL_INFLUENCE;

        // Right back wheel.
        raycastVehicle->addWheel(
            BACK_WHEEL_DISTANCE, // connection point
            wheelDirection, // wheel direction
            wheelAxle, // axle
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            false // isFrontWheel
        );
        rightBackWheel = &raycastVehicle->getWheelInfo(2);
        rightBackWheel->m_rollInfluence = ROLL_INFLUENCE;

        // Left back wheel.
        raycastVehicle->addWheel(
            btVector3(-BACK_WHEEL_DISTANCE.x(), BACK_WHEEL_DISTANCE.y(), BACK_WHEEL_DISTANCE.z()), // connection point
            wheelDirection, // wheel direction
            wheelAxle, // axle
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            false // isFrontWheel
        );
        leftBackWheel = &raycastVehicle->getWheelInfo(3);
        leftBackWheel->m_rollInfluence = ROLL_INFLUENCE;

        physicsWorld.registerStepCallback(this);
    }

    Rally::Vector3 PhysicsCar::getPosition() const {
        // Note that we cannot ask the rigidbody or use raycastVehicle->getForwardVector(),
        // as we won't get interpolated values then. That's motion-state exclusive info.
        const btTransform& graphicsTransform = bodyMotionState.m_graphicsWorldTrans;
        const btVector3& position = graphicsTransform.getOrigin();
        return Rally::Vector3(position.x(), position.y(), position.z());
    }

    Rally::Quaternion PhysicsCar::getOrientation() const {
        const btTransform& graphicsTransform = bodyMotionState.m_graphicsWorldTrans;
        /*return Rally::Vector3(
            graphicsTransform.getBasis()[0][2], // The z-column basis vector
            graphicsTransform.getBasis()[1][2], // The z-column basis vector
            graphicsTransform.getBasis()[2][2] // The z-column basis vector
        );*/
        const btQuaternion orientation = graphicsTransform.getRotation();
        return Rally::Quaternion(orientation.x(), orientation.y(), orientation.z(), orientation.w());
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

    void PhysicsCar::stepped(float deltaTime) {
        float engineForce = 0;
        float breakingForce = 0;

        if(breakingRequested) {
            engineForce = 0;
            breakingForce = BREAKING_FORCE;

            // Some special logic for putting in the reverse
            if(true) {
                engineForce = ENGINE_REVERSE_FORCE;
                breakingForce = 0;
            }
        } else if(accelerationRequested) {
            engineForce = ENGINE_FORCE;
            breakingForce = 0;
        } else {
            // (Free wheeling)
        }

        // Apply equally to both back wheels.
        raycastVehicle->applyEngineForce(engineForce, 2);
        raycastVehicle->applyEngineForce(engineForce, 3);
        raycastVehicle->setBrake(breakingForce, 2);
        raycastVehicle->setBrake(breakingForce, 3);

        // Fast-move steering to 0 if it isn't requested anymore.
        if((steering < 0 && steeringRequested >= 0) || (steering > 0 && steeringRequested <= 0)) {
            steering = 0;
        }

        // Steering, integrated from last step to deltaTime seconds later.
        steering += deltaTime * STEERING_INCREASE * steeringRequested; // steeringRequested is -1, 0 or 1
        if(steering > MAX_STEERING) {
            steering = MAX_STEERING;
        } else if(steering < -MAX_STEERING) {
            steering = -MAX_STEERING;
        }

        // Apply equally on both front wheels.
        raycastVehicle->setSteeringValue(steering, 0);
        raycastVehicle->setSteeringValue(steering, 1);
    }

} }
