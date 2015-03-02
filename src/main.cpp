#include "controller/MainController.h"

#include "Rally.h"
#include <OgreException.h>
#include <stdexcept>

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char* argv[]){
#ifdef PLATFORM_WINDOWS
#ifdef _DEBUG
    std::string resourceConfigPath = "resources_d.cfg";
    std::string pluginConfigPath = "plugins_d.cfg";
#else
    std::string resourceConfigPath = "resources.cfg";
    std::string pluginConfigPath = "plugins.cfg";
#endif
#else
    std::string resourceConfigPath = "resources.cfg";
    std::string pluginConfigPath = "plugins-linux.cfg";
#endif

    Rally::Controller::MainController mainController;

    try {
        mainController.initialize(resourceConfigPath, pluginConfigPath);
        mainController.start();
    } catch(Ogre::Exception& e) {
        std::cerr << "Ogre exception: " << e.getFullDescription().c_str() << std::endl;
    } catch(std::exception& error) {
        std::cerr << "Exception: " << error.what() << std::endl;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
