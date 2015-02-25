#include "controller/MainController.h"
#include "model/Car.h"
#include "util/Timer.h"
#include "view/InputInit.h"
#include "view/SceneView.h"

namespace Rally { namespace Controller {
    MainController::MainController() :
            sceneView(SceneView(world)),
            remoteCarListener(MainController_RemoteCarListener(world, sceneView)),
            netView(View::RallyNetView(remoteCarListener)) {
    }

    MainController::~MainController() {
    }

    void MainController::initialize(std::string resourceConfigPath, std::string pluginConfigPath) {
        world.initialize("../../media/world.bullet");

        sceneView.initialize(resourceConfigPath, pluginConfigPath);

        netView.initialize(std::string("127.0.0.1"), 1337, &world.getPlayerCar());

        inputInit.setup();
    }

    void MainController::start() {
        Rally::Util::Timer frameTimer;

        frameTimer.reset();
        while(true) {
            updateInput();

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

            netView.pullRemoteChanges();

            world.update(deltaTime);

            netView.pushLocalChanges();

            // TODO: Investigate in which order we'll do things (buffer up graphics commands, do some CPU, flip render buffers)
            if(!sceneView.renderFrame()) {
                return;
            }
        }
    }

    void MainController::updateInput() {
        Rally::Model::Car& car = world.getPlayerCar();

        // Accelerate and break
        car.setAccelerationRequested(inputInit.isKeyPressed("up"));
        car.setBreakingRequested(inputInit.isKeyPressed("down"));

        // Steering
        bool left = inputInit.isKeyPressed("left");
        bool right = inputInit.isKeyPressed("right");
        if(left && !right) {
            car.setSteeringRequested(1);
        } else if(!left && right) {
            car.setSteeringRequested(-1);
        } else {
            // neither left nor right, or left and right
            car.setSteeringRequested(0);
        }

        if(inputInit.isKeyPressed("d")){
		    sceneView.setDebugDrawEnabled(true);
        }
        if (inputInit.isKeyPressed("f")){
		    sceneView.setDebugDrawEnabled(false);
        }
    }

    void MainController_RemoteCarListener::carUpdated(unsigned short carId,
            Rally::Vector3 position,
            Rally::Quaternion orientation,
            Rally::Vector3 velocity) {
        Rally::Model::RemoteCar& remoteCar = world.getRemoteCar(carId); // carId is upcast to int

        remoteCar.setTargetTransform(position, velocity, orientation);

        sceneView.remoteCarUpdated(carId, remoteCar);
    }

    void MainController_RemoteCarListener::carRemoved(unsigned short carId) {
        sceneView.remoteCarRemoved(carId, world.getRemoteCar(carId)); // carId is upcast to int

        world.removeRemoteCar(carId); // carId is upcast to int
    }
} }
