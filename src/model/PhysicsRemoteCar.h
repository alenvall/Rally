#ifndef RALLY_MODEL_PHYSICSREMOTECAR_H_
#define RALLY_MODEL_PHYSICSREMOTECAR_H_

#include "Rally.h"

#include <btBulletDynamicsCommon.h>

namespace Rally { namespace Model {

    class PhysicsWorld;

    class PhysicsRemoteCar {
        public:
            PhysicsRemoteCar();
            virtual ~PhysicsRemoteCar();
            void attachTo(PhysicsWorld& physicsWorld);

            Rally::Vector3 getPosition() const;
            Rally::Quaternion getOrientation() const;
            Rally::Vector3 getVelocity() const;

        private:
            void initializeConstructionInfo();
            btDynamicsWorld* dynamicsWorld;

            btCollisionShape* bodyShape;
            btDefaultMotionState bodyMotionState;
            btRigidBody::btRigidBodyConstructionInfo* bodyConstructionInfo;
            btRigidBody* bodyRigidBody;
    };

} }

#endif // RALLY_MODEL_PHYSICSREMOTECAR_H_
