#include "model/RemoteCar.h"

#include "model/World.h"

namespace Rally { namespace Model {

    RemoteCar::RemoteCar(Rally::Model::PhysicsWorld& physicsWorld) :
            physicsWorld(physicsWorld),
            carType('a') {
    }

    RemoteCar::~RemoteCar() {
    }

    void RemoteCar::attachToWorld() {
        physicsRemoteCar.attachTo(physicsWorld);
    }

    void RemoteCar::setTargetTransform(const Rally::Vector3& targetPosition,
            const Rally::Vector3& incomingVelocity,
            const Rally::Quaternion& targetOrientation) {
        physicsRemoteCar.setTargetTransform(targetPosition, incomingVelocity, targetOrientation);
    }

    Rally::Vector3 RemoteCar::getPosition() const {
        return physicsRemoteCar.getPosition();
    }

    Rally::Quaternion RemoteCar::getOrientation() const {
        return physicsRemoteCar.getOrientation();
    }

} }
