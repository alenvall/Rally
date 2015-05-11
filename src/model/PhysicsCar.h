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

			void teleport(const Rally::Vector3& position, const Rally::Quaternion& orientation);

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

            Rally::Vector3 getRightFrontWheelOrigin() const;
            Rally::Vector3 getLeftFrontWheelOrigin() const;
            Rally::Vector3 getRightBackWheelOrigin() const;
            Rally::Vector3 getLeftBackWheelOrigin() const;

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
				particlePositions.clear();
			}

			std::list<Rally::Vector3> getSkidmarkPositions(int wheel){
				return skidmarkPositions[wheel];
			}

			void clearSkidmarkPositions(int wheel){
				if(skidmarkPositions[wheel].size() > 3){
					skidmarkPositions[wheel].pop_back();
					skidmarkPositions[wheel].pop_back();

				}else
					skidmarkPositions[wheel].pop_back();

			}

			std::list<Rally::Vector3> getSkidmarkNormals(int wheel){
				return skidmarkNormals[wheel];
			}

			void clearSkidmarkNormals(int wheel){
				if(skidmarkNormals[wheel].size() > 3){
					skidmarkNormals[wheel].pop_back();
					skidmarkNormals[wheel].pop_back();

				}else
					skidmarkNormals[wheel].pop_back();
			}

			std::list<Rally::Vector3> getSkidmarkDirections(int wheel){
				return skidmarkDirections[wheel];
			}

			void clearSkidmarkDirections(int wheel){
				if(skidmarkDirections[wheel].size() > 3){
					skidmarkDirections[wheel].pop_back();
					skidmarkDirections[wheel].pop_back();

				}else
					skidmarkDirections[wheel].pop_back();
			}

            std::list<float> getSkidmarkTractions(int wheel){
				return skidmarkTractions[wheel];
			}

			void clearSkidmarkTractions(int wheel){
				if(skidmarkTractions[wheel].size() > 3){
					skidmarkTractions[wheel].pop_back();
					skidmarkTractions[wheel].pop_back();

				}else
					skidmarkTractions[wheel].pop_back();
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
			
			Rally::Vector3 wheelpos;
			void checkForSkidmarks();
			std::list<Rally::Vector3> skidmarkPositions[4];
			std::list<Rally::Vector3> skidmarkNormals[4];
			std::list<Rally::Vector3> skidmarkDirections[4];
			std::list<float> skidmarkTractions[4];

    };

} }

#endif // RALLY_MODEL_PHYSICSCAR_H_
