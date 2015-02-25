#ifndef RALLY_VIEW_CARVIEW_H_
#define RALLY_VIEW_CARVIEW_H_

#include "Rally.h"

namespace Ogre {
    class SceneManager;
    class Entity;
    class SceneNode;
}

namespace Rally { namespace View {

class CarView {
    public:
        CarView();
        virtual ~CarView();
        void attachTo(Ogre::SceneManager* sceneManager, const std::string& carName);
        void updateBody(Rally::Vector3 position, Rally::Quaternion orientation);

    private:
        Ogre::SceneManager* sceneManager;
        Ogre::Entity* carEntity;
        Ogre::SceneNode* carNode;
};

} }

#endif // RALLY_VIEW_CARVIEW_H_
