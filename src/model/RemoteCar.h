#ifndef RALLY_MODEL_REMOTECAR_H_
#define RALLY_MODEL_REMOTECAR_H_

#include "Rally.h"

#include "model/PhysicsRemoteCar.h"

namespace Rally { namespace Model {

    class World;

    class RemoteCar {
        public:
            RemoteCar(Rally::Model::PhysicsWorld& physicsWorld);
            virtual ~RemoteCar();

            void attachToWorld();

            Rally::Vector3 getPosition() const;
            Rally::Quaternion getOrientation() const;
            Rally::Vector3 getVelocity() const;

        private:
            Rally::Model::PhysicsWorld& physicsWorld;
            Rally::Model::PhysicsRemoteCar physicsRemoteCar;
    };

} }

#endif // RALLY_MODEL_REMOTECAR_H_
