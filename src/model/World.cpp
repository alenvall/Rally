#include "model/World.h"
#include "model/PhysicsWorld.h"

#include <sstream>

namespace Rally { namespace Model {

    World::World() :
            physicsWorld(),
            playerCar(physicsWorld),
			finish(physicsWorld){
    }

    World::~World() {
    }

    void World::initialize(const std::string & bulletFile) {
        physicsWorld.initialize(bulletFile);
        playerCar.attachToWorld();
		finish.attachToWorld();
    }

    void World::update(float deltaTime) {
        physicsWorld.update(deltaTime);
    }
} }
