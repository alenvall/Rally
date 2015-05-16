#include "model/Car.h"

#include "model/World.h"

namespace Rally { namespace Model {

    Car::Car(Rally::Model::PhysicsWorld& physicsWorld) :
            physicsWorld(physicsWorld),
            carType('a') {
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

    float Car::getEffectFactor() const {
        // Clamped and normalized velocity is enought for now
        float result = physicsCar.getVelocity().length() / 40.0f;
        return (result > 1.0f) ? 1.0f : result;
    }

    void Car::cycleCarType() {
        // Todo: clean up this mess when the models need to perform logic on carType.
        carType++;
        if(carType > 'r') {
            carType = 'a';
        }
		physicsCar.teleport(Rally::Vector3::UNIT_Y*20.0f, Rally::Quaternion::IDENTITY);
    }

} }
