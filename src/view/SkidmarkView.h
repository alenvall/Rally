#ifndef RALLY_VIEW_SKIDMARKVIEW_H_
#define RALLY_VIEW_SKIDMARKVIEW_H_

#include "model/Car.h"

#include "Rally.h"

namespace Ogre {
    class SceneManager;
    class Entity;
    class SceneNode;
}

namespace Rally { namespace View {

class SkidmarkView {
    public:
        SkidmarkView();
        virtual ~SkidmarkView();

        void attachTo(Ogre::SceneManager* sceneManager, Rally::Model::Car& car);

		void update();

    private:

        Ogre::SceneNode* skidmarkNode;
		Ogre::BillboardSet* skidmarkBillboards;

		void createSkidmark(Rally::Vector3 position, Rally::Vector3 normal, Rally::Vector3 direction, float traction);
		
        Rally::Model::Car* car;

};

} }
#endif // RALLY_VIEW_SKIDMARKVIEW_H_
