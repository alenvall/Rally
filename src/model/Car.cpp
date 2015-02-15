#include "model/Car.h"

#include "model/World.h"

namespace Rally { namespace Model {

    Car::Car(Rally::Model::World& world) :
            world(world) {
    }

    Car::~Car() {
    }

    void Car::attachToWorld() {
        physicsCar.attachTo(world.getPhysicsWorld());
    }

    Rally::Vector3 Car::getPosition() const {
        return physicsCar.getPosition();
    }

    Rally::Vector3 Car::getOrientation() const {
        return physicsCar.getOrientation();
    }

    Rally::Vector3 Car::getVelocity() const {
        return physicsCar.getVelocity();
    }

} }
