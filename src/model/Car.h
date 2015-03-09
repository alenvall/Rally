#ifndef RALLY_MODEL_CAR_H_
#define RALLY_MODEL_CAR_H_

#include "Rally.h"

#include "model/PhysicsCar.h"

namespace Rally { namespace Model {

    class World;

    class Car {
        public:
            Car(Rally::Model::PhysicsWorld& physicsWorld);
            virtual ~Car();

            void attachToWorld();

            Rally::Vector3 getPosition() const;
            Rally::Quaternion getOrientation() const;
            Rally::Vector3 getVelocity() const;

            /**
             *  Normalized speed, might be increased by flight etc. Returns a value in the range[0, 1].
             */
            float getEffectFactor() const;

            Rally::Quaternion getRightFrontWheelOrientation() const {
                return physicsCar.getRightFrontWheelOrientation();
            }

            Rally::Quaternion getLeftFrontWheelOrientation() const {
                return physicsCar.getLeftFrontWheelOrientation();
            }

            Rally::Quaternion getRightBackWheelOrientation() const {
                return physicsCar.getRightBackWheelOrientation();
            }

            Rally::Quaternion getLeftBackWheelOrientation() const {
                return physicsCar.getLeftBackWheelOrientation();
            }

            void setAccelerationRequested(bool accelerationRequested) {
                physicsCar.setAccelerationRequested(accelerationRequested);
            }

            void setBreakingRequested(bool breakingRequested) {
                physicsCar.setBreakingRequested(breakingRequested);
            }

            void setSteeringRequested(int steeringRequested) {
                physicsCar.setSteeringRequested(steeringRequested);
            }
			
			std::list<Rally::Vector3> getParticlePositions(){
				return physicsCar.getParticlePositions();
			}

			void clearParticlePositions(){
				physicsCar.clearParticlePositions();
			}

			void clearSkidmarkPositions(int wheel){
				physicsCar.clearSkidmarkPositions(wheel);
			}
			
			std::list<Rally::Vector3> getSkidmarkPositions(int wheel){
				return physicsCar.getSkidmarkPositions(wheel);
			}

			std::list<Rally::Vector3> getSkidmarkNormals(int wheel){
				return physicsCar.getSkidmarkNormals(wheel);
			}

			void clearSkidmarkNormals(int wheel){
				physicsCar.clearSkidmarkNormals(wheel);
			}

			/*
			std::list<float> getRots(int wheel){
				return physicsCar.getRots(wheel);
			}

			void clearRots(int wheel){
				physicsCar.clearRots(wheel);
			}
			*/

			Rally::Model::PhysicsCar getPhysicsCar(){
				return physicsCar;
			}

        private:
            Rally::Model::PhysicsWorld& physicsWorld;
            Rally::Model::PhysicsCar physicsCar;
    };

} }

#endif // RALLY_MODEL_CAR_H_
