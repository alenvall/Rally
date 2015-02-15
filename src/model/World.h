#ifndef RALLY_MODEL_WORLD_H_
#define RALLY_MODEL_WORLD_H_

#include "model/PhysicsWorld.h"

namespace Rally { namespace Model {

    class World {
        public:
            World();
            virtual ~World();
            void initialize(const std::string & bulletFile);
            void update(float deltaTime);

            Rally::Model::PhysicsWorld& getPhysicsWorld() {
                return physicsWorld;
            }

        private:
            Rally::Model::PhysicsWorld physicsWorld;
    };

} }

#endif // RALLY_MODEL_WORLD_H_
