#ifndef RALLY_CONTROLLER_MAINCONTROLLER_H_
#define RALLY_CONTROLLER_MAINCONTROLLER_H_

#include <string>
#include "SceneView.h"
#include "RallyNetView.h"

namespace Rally { namespace Controller {
    class MainController_RemoteCarListener : public Rally::View::RallyNetView_NetworkCarListener {
        public:
            // @Override
            virtual void carUpdated(unsigned short carId,
                Rally::Vector3 position,
                Rally::Vector3 orientation,
                Rally::Vector3 velocity);

            // @Override
            virtual void carRemoved(unsigned short carId);
    };

    class MainController {
        public:
            MainController();
            virtual ~MainController();
            void initialize(std::string resourceConfigPath, std::string pluginConfigPath);
            void start();

        private:
            SceneView sceneView;
            Rally::View::RallyNetView netView;
            MainController_RemoteCarListener remoteCarListener;
            Rally::Model::Car playerCar;
    };
} }

#endif // RALLY_CONTROLLER_MAINCONTROLLER_H_
