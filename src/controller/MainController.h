#ifndef RALLY_CONTROLLER_MAINCONTROLLER_H_
#define RALLY_CONTROLLER_MAINCONTROLLER_H_

#include <string>
#include "view/SceneView.h"

class MainController {
    public:
        MainController();
        virtual ~MainController();
        bool initialize(std::string resourceConfigPath, std::string pluginConfigPath);
        void start();

    private:
        SceneView* sceneView;
};

#endif // RALLY_CONTROLLER_MAINCONTROLLER_H_
