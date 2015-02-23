#ifndef RALLY_MODEL_PHYSICSCAR_H_
#define RALLY_MODEL_PHYSICSCAR_H_

#include "Rally.h"

#include <btBulletDynamicsCommon.h>

#include <string>

namespace Rally { namespace Model {

    class PhysicsWorld;

    class PhysicsCar {
        public:
            PhysicsCar();
            virtual ~PhysicsCar();
            void attachTo(PhysicsWorld& physicsWorld);

            Rally::Vector3 getPosition() const;
            Rally::Quaternion getOrientation() const;
            Rally::Vector3 getVelocity() const;

            float getRightFrontWheelTraction() const;
            float getLeftFrontWheelTraction() const;
            float getRightBackWheelTraction() const;
            float getLeftBackWheelTraction() const;

        private:
            void initializeConstructionInfo();
            btDynamicsWorld* dynamicsWorld;

            btCollisionShape* bodyShape;
            btDefaultMotionState bodyMotionState;
            btRigidBody::btRigidBodyConstructionInfo* bodyConstructionInfo;
            btRigidBody* bodyRigidBody;

            btVehicleRaycaster* vehicleRaycaster;
            btRaycastVehicle* raycastVehicle;
            btWheelInfo* rightFrontWheel;
            btWheelInfo* leftFrontWheel;
            btWheelInfo* rightBackWheel;
            btWheelInfo* leftBackWheel;
    };

} }

#endif // RALLY_MODEL_PHYSICSCAR_H_
