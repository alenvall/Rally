#ifndef RALLY_CONTROLLER_MAINCONTROLLER_H_
#define RALLY_CONTROLLER_MAINCONTROLLER_H_

#include <string>
#include "view/SceneView.h"
#include "view/RallyNetView.h"
#include "model/World.h"
#include "view/InputInit.h"

namespace Rally { namespace Controller {
    class MainController_RemoteCarListener : public Rally::View::RallyNetView_NetworkCarListener {
        public:
            MainController_RemoteCarListener(Rally::Model::World& world, SceneView& sceneView) :
                world(world),
                sceneView(sceneView) {
            }

            // @Override
            virtual void carUpdated(unsigned short carId,
                Rally::Vector3 position,
                Rally::Quaternion orientation,
                Rally::Vector3 velocity,
                char carType,
                Rally::Vector4 tractionVector);

            // @Override
            virtual void carRemoved(unsigned short carId);

        private:
            Rally::Model::World& world;
            SceneView& sceneView;
    };

	class MainController : public Rally::View::SceneView_LogicListener {
        public:
            MainController();
            virtual ~MainController();
            void initialize(std::string resourceConfigPath, std::string pluginConfigPath);
            virtual void updateLogic(float deltaTime);
            void start();

        private:
            void updateInput();

            Rally::Model::World world;
            SceneView sceneView;
            InputInit inputInit;
            Rally::View::RallyNetView netView;
            MainController_RemoteCarListener remoteCarListener;
    };
} }

#endif // RALLY_CONTROLLER_MAINCONTROLLER_H_
