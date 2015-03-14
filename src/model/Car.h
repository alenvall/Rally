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

            Rally::Vector4 getTractionVector() const {
                return Rally::Vector4(physicsCar.getRightFrontWheelTraction(),
                    physicsCar.getLeftFrontWheelTraction(),
                    physicsCar.getRightBackWheelTraction(),
                    physicsCar.getLeftBackWheelTraction());
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

            void setCarType(char carType) {
                this->carType = carType;
            }

            char getCarType() const {
                return carType;
            }

        private:
            Rally::Model::PhysicsWorld& physicsWorld;
            Rally::Model::PhysicsCar physicsCar;
            char carType;
    };

} }

#endif // RALLY_MODEL_CAR_H_
