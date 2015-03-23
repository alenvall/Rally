#include "Rally.h"
#include "model/Checkpoint.h"

namespace Ogre {
    class SceneManager;
    class Entity;
    class SceneNode;
}

namespace Rally { namespace View {

class CheckpointView {
    public:
		CheckpointView();
        virtual ~CheckpointView();
        void attachTo(Ogre::SceneManager* sceneManager, const std::string& checkPointName, 
			const std::string& meshName, const Rally::Model::Checkpoint& checkPoint);

        void update();

    private:
        Ogre::SceneManager* sceneManager;
        Ogre::Entity* checkPointEntity;
        Ogre::SceneNode* checkPointNode;

		const Rally::Model::Checkpoint* checkPoint;
};

} }
