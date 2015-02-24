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

            void setAccelerationRequested(bool accelerationRequested) {
                physicsCar.setAccelerationRequested(accelerationRequested);
            }

            void setBreakingRequested(bool breakingRequested) {
                physicsCar.setBreakingRequested(breakingRequested);
            }

            void setSteeringRequested(int steeringRequested) {
                physicsCar.setSteeringRequested(steeringRequested);
            }

        private:
            Rally::Model::PhysicsWorld& physicsWorld;
            Rally::Model::PhysicsCar physicsCar;
    };

} }

#endif // RALLY_MODEL_CAR_H_
