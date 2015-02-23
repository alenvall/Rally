#include "model/Car.h"

#include "model/World.h"

namespace Rally { namespace Model {

    Car::Car(Rally::Model::PhysicsWorld& physicsWorld) :
            physicsWorld(physicsWorld) {
    }

    Car::~Car() {
    }

    void Car::attachToWorld() {
        physicsCar.attachTo(physicsWorld);
    }

    Rally::Vector3 Car::getPosition() const {
        return physicsCar.getPosition();
    }

    Rally::Quaternion Car::getOrientation() const {
        return physicsCar.getOrientation();
    }

    Rally::Vector3 Car::getVelocity() const {
        return physicsCar.getVelocity();
    }

} }
