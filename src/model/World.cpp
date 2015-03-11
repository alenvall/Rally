#include "model/World.h"
#include "model/PhysicsWorld.h"

#include <sstream>

namespace Rally { namespace Model {

    World::World() :
            physicsWorld(),
            playerCar(physicsWorld),
			finish(physicsWorld),
			start(physicsWorld) {
    }

    World::~World() {
    }

    void World::initialize(const std::string & bulletFile) {
        physicsWorld.initialize(bulletFile);
        playerCar.attachToWorld();
		finish.attachToWorld(btVector3(-1.3f, 0.f, -1.1f), btVector3(0.1f, 6.f, 3.2f));
		start.attachToWorld(btVector3(90.0f, 8.f, 110.0f), btVector3(3.f, 3.f, 3.f));
        finishTimer.reset();
    }

    void World::update(float deltaTime) {
        physicsWorld.update(deltaTime);
		printFinishedTime();
    }

	void World::printFinishedTime(){
		if(!start.hasFinished())
			finishTimer.reset();

		if(start.hasFinished() && finish.hasFinished() && finish.isEnabled()){
			finish.setEnabled(false);
			std::cout << "Finished after: " << finishTimer.getElapsedSeconds() << " seconds!" << std::endl;
		}
	}
} }
