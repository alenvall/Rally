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
            Rally::Vector3 getOrientation() const;
            Rally::Vector3 getVelocity() const;

        private:
            Rally::Model::PhysicsWorld& physicsWorld;
            Rally::Model::PhysicsCar physicsCar;
    };

} }

#endif // RALLY_MODEL_CAR_H_
