#include "model/RemoteCar.h"

#include "model/World.h"

namespace Rally { namespace Model {

    RemoteCar::RemoteCar(Rally::Model::PhysicsWorld& physicsWorld) :
            physicsWorld(physicsWorld) {
    }

    RemoteCar::~RemoteCar() {
    }

    void RemoteCar::attachToWorld() {
        physicsRemoteCar.attachTo(physicsWorld);
    }

    Rally::Vector3 RemoteCar::getPosition() const {
        return physicsRemoteCar.getPosition();
    }

    Rally::Quaternion RemoteCar::getOrientation() const {
        return physicsRemoteCar.getOrientation();
    }

    Rally::Vector3 RemoteCar::getVelocity() const {
        return physicsRemoteCar.getVelocity();
    }

} }
