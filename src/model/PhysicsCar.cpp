#include "model/PhysicsCar.h"

#include "model/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

#include <sstream>
#include <iostream>

namespace Rally { namespace Model {

    namespace {
        const btVector3 CAR_DIMENSIONS(2.0f, 1.0f, 4.0f);
        const float CAR_MASS = 800.0f;
        const float MASS_OFFSET = 1.0f;

        // The rolling torque from each wheel is scaled with this (to prevent rollover).
        // 0.0f = no rolling, 1.0f = rolling like in reality.
        const float ROLL_INFLUENCE = 0.1f;

        const float SUSPENSION_REST_LENGTH = 0.6f; // (see also maxSuspensionTravelCm)
        const float WHEEL_RADIUS = 0.5f;
        const float FRONT_WHEEL_FRICTION = 2.0f;
        const float BACK_WHEEL_FRICTION = 1.999f;

        const float ENGINE_FORCE = 3500.0f;
        const float ENGINE_REVERSE_FORCE = -2800.0f;
        const float BREAKING_FORCE = 2000.0f;

        const float MAX_STEERING = 0.4f;
        const float STEERING_INCREASE = 0.8f; // [same unit as steering] per second

        // The wheel distance is calculated from the origin = center of the car.
        // The wheel (including radius) should be located inside the car body:
        //  - Below the car and it might sink into the ground (rays goes into infinity)
        //  - Above the car and it confuses the body with the ground => weird behavior.
        //    (The official demo does this though... Maybe the suspension is included?)
        // Values below represent the right wheel (mirrored in the yz-plane for left).
        const btVector3 FRONT_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 - 0.1f, MASS_OFFSET, (CAR_DIMENSIONS.z()/2 - 0.3f - WHEEL_RADIUS));
        const btVector3 BACK_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 - 0.1f, MASS_OFFSET, -(CAR_DIMENSIONS.z()/2 - 0.1f - WHEEL_RADIUS));
    }

    PhysicsCar::PhysicsCar() :
            dynamicsWorld(NULL),
            bodyShape(NULL),
            lowerMassCenterShape(NULL),
            bodyMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(-50.0f, 5.2f, 40.0f))),
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
            steeringRequested(0),
            engineForce(0),
            breakingForce(0) {
    }

    PhysicsCar::~PhysicsCar() {
    }

    void PhysicsCar::initializeConstructionInfo() {
        if(bodyConstructionInfo != NULL) {
            return;
        }

        bodyShape = new btBoxShape(CAR_DIMENSIONS / 2); // Takes half-extents

        // Lower the center of mass to improve steering and counteract rolling.
        lowerMassCenterShape = new btCompoundShape();
        lowerMassCenterShape->addChildShape(btTransform(btQuaternion(0, 0, 0, 1.0f), btVector3(0, MASS_OFFSET, 0)), bodyShape);

        // Initialize what I think is the diagonal in the inertia tensor
        btVector3 inertia(0, 0, 0);
        lowerMassCenterShape->calculateLocalInertia(CAR_MASS, inertia); // TODO: We might want to lower/move center of gravity...

        bodyConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(CAR_MASS, &bodyMotionState, lowerMassCenterShape, inertia);
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
        // tuning.m_frictionSlip set below for each wheel
        // rollInfluence cannot be set here, so we have to apply it to every wheel individually.

        // Create the raycasting part for the "wheels"
        vehicleRaycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
        raycastVehicle = new btRaycastVehicle(tuning, bodyRigidBody, vehicleRaycaster);
        raycastVehicle->setCoordinateSystem(0, 1, 2);
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
        rightFrontWheel->m_frictionSlip = FRONT_WHEEL_FRICTION;

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
        leftFrontWheel->m_frictionSlip = FRONT_WHEEL_FRICTION;

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
        rightBackWheel->m_frictionSlip = BACK_WHEEL_FRICTION;

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
        leftBackWheel->m_frictionSlip = BACK_WHEEL_FRICTION;

        physicsWorld.registerStepCallback(this);
    }

    Rally::Vector3 PhysicsCar::getPosition() const {
        // Note that we cannot ask the rigidbody or use raycastVehicle->getForwardVector(),
        // as we won't get interpolated values then. That's motion-state exclusive info.
        // std::cout << getLeftFrontWheelTraction() << " " << getRightFrontWheelTraction() << std::endl;
        const btTransform& graphicsTransform = bodyMotionState.m_graphicsWorldTrans;
        const btVector3& position = graphicsTransform.getOrigin();

        // Move it upwards as to compensate for the center-of-mass offset.
        Rally::Vector3 moveUp = getOrientation() * Rally::Vector3(0, MASS_OFFSET, 0);

        return Rally::Vector3(position.x(), position.y(), position.z()) + moveUp;
    }

    Rally::Quaternion PhysicsCar::getOrientation() const {
        const btTransform& graphicsTransform = bodyMotionState.m_graphicsWorldTrans;
        /*return Rally::Vector3(
            graphicsTransform.getBasis()[0][2], // The z-column basis vector
            graphicsTransform.getBasis()[1][2], // The z-column basis vector
            graphicsTransform.getBasis()[2][2] // The z-column basis vector
        );*/
        const btQuaternion orientation = graphicsTransform.getRotation();
        return Rally::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z());
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
        breakingForce = 0;
        if(breakingRequested) {
            // Breaking actually means reverse
            engineForce = ENGINE_REVERSE_FORCE;
        } else if(accelerationRequested) {
            engineForce = ENGINE_FORCE;
        } else {
            // Free wheeling
            engineForce = 0;
        }

        // In case someone tries to go in the other direction, we break to a halt first.
        btVector3 velocity = bodyRigidBody->getLinearVelocity();
        if(velocity.length() > 0.5f) {
            float directionInfo = raycastVehicle->getForwardVector().dot(velocity); // ||v||*cos(shortest angle)
            bool isActuallyReversing = directionInfo < 0; // If we actually go backwards in the simulation
            if((engineForce > 0 && isActuallyReversing) || (engineForce < 0 && !isActuallyReversing)) {
                engineForce = 0;
                breakingForce = BREAKING_FORCE;
            }
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
