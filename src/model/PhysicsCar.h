#ifndef RALLY_MODEL_PHYSICSCAR_H_
#define RALLY_MODEL_PHYSICSCAR_H_

#include "Rally.h"
#include "model/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

#include <string>

namespace Rally { namespace Model {

    class PhysicsCar : public PhysicsWorld_StepCallback {
        public:
            PhysicsCar();
            virtual ~PhysicsCar();
            void attachTo(PhysicsWorld& physicsWorld);

            Rally::Vector3 getPosition() const;
            Rally::Quaternion getOrientation() const;
            Rally::Vector3 getVelocity() const;

            Rally::Quaternion getRightFrontWheelOrientation() const;
            Rally::Quaternion getLeftFrontWheelOrientation() const;
            Rally::Quaternion getRightBackWheelOrientation() const;
            Rally::Quaternion getLeftBackWheelOrientation() const;

            float getRightFrontWheelTraction() const;
            float getLeftFrontWheelTraction() const;
            float getRightBackWheelTraction() const;
            float getLeftBackWheelTraction() const;

            // @Override
            virtual void stepped(float deltaTime);

            void setAccelerationRequested(bool accelerationRequested) {
                this->accelerationRequested = accelerationRequested;
            }

            void setBreakingRequested(bool breakingRequested) {
                this->breakingRequested = breakingRequested;
            }

            void setSteeringRequested(int steeringRequested) {
                this->steeringRequested = steeringRequested;
            }
			
			std::list<Rally::Vector3> getParticlePositions(){
				return particlePositions;
			}

			void clearParticlePositions(){
				return particlePositions.clear();
			}

        private:
            void initializeConstructionInfo();
            bool isAllWheelsOnGround();

            btDynamicsWorld* dynamicsWorld;

            btBoxShape* bodyShape;
            btCompoundShape* lowerMassCenterShape;
            btDefaultMotionState bodyMotionState;
            btRigidBody::btRigidBodyConstructionInfo* bodyConstructionInfo;
            btRigidBody* bodyRigidBody;

            btVehicleRaycaster* vehicleRaycaster;
            btRaycastVehicle* raycastVehicle;

            float steering;
            float lastCompensatedSteering;
            bool accelerationRequested;
            bool breakingRequested;
            int steeringRequested; // steeringRequested is -1, 0 or 1

            float engineForce;
            float breakingForce;

			std::list<Rally::Vector3> particlePositions;
    };

} }

#endif // RALLY_MODEL_PHYSICSCAR_H_
