#include "MainController.h"

MainController::MainController() {
    this->sceneView = new SceneView();
}

MainController::~MainController(void) {
    delete sceneView;
}

bool MainController::initialize(std::string resourceConfigPath, std::string pluginConfigPath) {
    return sceneView->initialize(resourceConfigPath, pluginConfigPath);
}

void MainController::start() {
    while(true) {
        // TODO: Drive the models here...

        if(!sceneView->renderFrame()) {
            return;
        }
    }
}
