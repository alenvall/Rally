#ifndef RALLY_MODEL_PHYSICSWORLD_H_
#define RALLY_MODEL_PHYSICSWORLD_H_

#include <string>

class btBroadphaseInterface;
class btBulletWorldImporter;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btBulletWorldImporter;

namespace Rally { namespace Model {

    class PhysicsWorld {
        public:
            PhysicsWorld();
            virtual ~PhysicsWorld();
            void initialize(const std::string & bulletFile);
            void update(float deltaTime);

        private:
            btBroadphaseInterface* broadphase;
            btDefaultCollisionConfiguration* collisionConfiguration;
            btCollisionDispatcher* dispatcher;
            btSequentialImpulseConstraintSolver* solver;
            btDiscreteDynamicsWorld* dynamicsWorld;
            btBulletWorldImporter* fileLoader;
    };

} }

#endif // RALLY_MODEL_PHYSICSWORLD_H_
