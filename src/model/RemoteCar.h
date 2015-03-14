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

            void setTargetTransform(const Rally::Vector3& targetPosition,
                const Rally::Vector3& incomingVelocity,
                const Rally::Quaternion& targetOrientation);

            void setCarType(char carType) {
                this->carType = carType;
            }

            char getCarType() const {
                return carType;
            }

            void setTractionVector(const Rally::Vector4& tractionVector) {
                this->tractionVector = tractionVector;
            }

            Rally::Vector4 getTractionVector() {
                return tractionVector;
            }

        private:
            Rally::Model::PhysicsWorld& physicsWorld;
            Rally::Model::PhysicsRemoteCar physicsRemoteCar;
            char carType;
            Rally::Vector4 tractionVector;
    };

} }

#endif // RALLY_MODEL_REMOTECAR_H_
