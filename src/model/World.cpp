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
			start(physicsWorld),
            tunnelTeleport(physicsWorld) {
    }

    World::~World() {
    }

    void World::initialize(const std::string & bulletFile) {
        physicsWorld.initialize(bulletFile);
        playerCar.attachToWorld();
		finish.attachToWorld(btVector3(-1.3f, 0.f, -1.1f), btVector3(0.1f, 6.f, 3.2f));
		start.attachToWorld(btVector3(113.0f, 0.f, 183.0f), btVector3(10.f, 10.f, 10.f));
        tunnelTeleport.attachToWorld(btVector3(90.0f, 1.5f, -134.0f), btVector3(3.f, 3.f, 3.f));
        finishTimer.reset();
		start.collide();
		highScore = 200;
        lastTime = 0;
    }

    void World::update(float deltaTime) {
        physicsWorld.update(deltaTime);
        /*
        Rally::Vector3 playerCarPosition = playerCar.getPosition();
        if(playerCarPosition.y < -20.0f) {
            playerCar.teleport(Rally::Vector3(playerCarPosition.x, 30.0f, playerCarPosition.z),
                playerCar.getOrientation(),
                false);
        }

        // TODO: Removed until the tunnelTeleport.isEnabled/.hasCollided methods are fixed.
        /*if(tunnelTeleport.hasCollided()) {
            playerCar.teleport(Rally::Vector3(-126.0f, -4.0f, -52.0f),
                Rally::Quaternion(1.0f, 0.0f, 0.0f, 0.0f),
                true);
            tunnelTeleport.setEnabled(false);
        }*/

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
