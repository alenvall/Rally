#include "MainController.h"
#include "model/Car.h"

//Antar att input-initialisering goes here somewhere?
//add framelistener to root?

namespace Rally { namespace Controller {
    MainController::MainController() :
            sceneView(SceneView()),
            remoteCarListener(MainController_RemoteCarListener()),
            netView(View::RallyNetView(remoteCarListener)){
				root = Ogre::Root::getSingletonPtr();
    }

    MainController::~MainController() {
    }

    void MainController::initialize(std::string resourceConfigPath, std::string pluginConfigPath) {
        sceneView.initialize(resourceConfigPath, pluginConfigPath);

        playerCar = Model::Car();

        netView.initialize(std::string("127.0.0.1"), 1337, playerCar);
		//inputManager.createFrameListener();
		
    }

    void MainController::start() {
        while(true) {
	//kolla keyevents här med inputmanager?
    //        netView.update();
            // TODO: Drive the models here...

            // TODO: Investigate in which order we'll do things (buffer up graphics commands, do some CPU, flip render buffers)
            if(!sceneView.renderFrame()) {
                return;
            }
        }
    }

    void MainController_RemoteCarListener::carUpdated(unsigned short carId,
            Rally::Vector3 position,
            Rally::Vector3 orientation,
            Rally::Vector3 velocity) {
        std::cout << "Updated or added car " << carId << "." << std::endl;
    }

    void MainController_RemoteCarListener::carRemoved(unsigned short carId) {
        std::cout << "Removed car " << carId << "." << std::endl;
    }

} }
