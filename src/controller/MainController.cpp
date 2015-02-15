#include "controller/MainController.h"
#include "model/Car.h"
#include "util/Timer.h"

namespace Rally { namespace Controller {
    MainController::MainController() :
            sceneView(SceneView(world)),
            remoteCarListener(MainController_RemoteCarListener()),
            netView(View::RallyNetView(remoteCarListener)) {
    }

    MainController::~MainController() {
    }

    void MainController::initialize(std::string resourceConfigPath, std::string pluginConfigPath) {
        world.initialize("world.bullet");

        sceneView.initialize(resourceConfigPath, pluginConfigPath);

        netView.initialize(std::string("127.0.0.1"), 1337, playerCar);
    }

    void MainController::start() {
        Rally::Util::Timer frameTimer;

        frameTimer.reset();
        while(true) {
            float deltaTime = frameTimer.getElapsedSeconds();

            // Allow max 1000 FPS for precision/stability reasons.
            // This is hopefully capped by vsync or atleast computation below.
            if(deltaTime < 0.001f) {
                continue;
            }

            // Don't forget to do this AFTER the epsilon check above
            // (avoiding an infinite loop), but before anything else.
            frameTimer.reset();

            // ADD ANYTHING THAT'S NOT FRAME-TIMING CODE BELOW THIS LINE!

            // netView.update();

            world.update(deltaTime);

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
