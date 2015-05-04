#include "model/World.h"
#include "model/PhysicsWorld.h"

#include <fstream>
#include <iostream>
#include <sstream>

	

namespace Rally { namespace Model {

    World::World() :
            physicsWorld(),
            playerCar(physicsWorld),
			finish(physicsWorld),
			start(physicsWorld){
    }

    World::~World() {
    }

    void World::initialize(const std::string & bulletFile) {
        physicsWorld.initialize(bulletFile);
        playerCar.attachToWorld();
		finish.attachToWorld(btVector3(-1.3f, 0.f, -1.1f), btVector3(0.1f, 6.f, 3.2f));
		start.attachToWorld(btVector3(113.0f, 0.f, 183.0f), btVector3(10.f, 10.f, 10.f));
        finishTimer.reset();
		start.collide();
		highScore = 200;

    }

    void World::update(float deltaTime) {
        physicsWorld.update(deltaTime);
		printFinishedTime();

    }

	void World::printFinishedTime(){
		if(!start.hasCollided())
			finish.reset();

		if(start.hasCollided() && finish.hasCollided() && finish.isEnabled()){
			//finish.setEnabled(false);
			if(finishTimer.getElapsedSeconds() > 2)
			  std::cout << "Finished after: " << finishTimer.getElapsedSeconds() << " seconds!" << std::endl;
			lastTime = finishTimer.getElapsedSeconds();
			if(highScore == 200)
				highScore = 199;
			else if(lastTime < highScore)
				highScore = lastTime;
			finish.reset();
			start.reset();
			finishTimer.reset();


		}
	}

	float World::getElapsedSeconds(){
	    return finishTimer.getElapsedSeconds();
	}

	float World::getLastTime(){
		return lastTime;
	}

	float World::getHighScore(){
		return highScore;
	}

	void World::resetHighScore(){
		highScore = 200;
	}
} }
