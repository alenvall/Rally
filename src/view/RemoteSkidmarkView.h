#ifndef RALLY_VIEW_REMOTESKIDMARKVIEW_H_
#define RALLY_VIEW_REMOTESKIDMARKVIEW_H_

#include "model/RemoteCar.h"
#include "view/SkidmarkView.h"

#include "Rally.h"

namespace Ogre {
    class SceneManager;
    class Entity;
    class SceneNode;
}

namespace Rally { namespace View {

class RemoteSkidmarkView : public SkidmarkView {
    public:
        RemoteSkidmarkView();
        virtual ~RemoteSkidmarkView();

        void attachTo(Ogre::SceneManager* sceneManager);

		void update(const Rally::Model::RemoteCar& remoteCar);

    private:

        Ogre::SceneNode* skidmarkNode;
		Ogre::BillboardSet* skidmarkBillboards;

		void createSkidmark(Rally::Vector3 position, Rally::Vector3 normal, Rally::Vector3 direction, float traction, float speed);
};

} }
#endif // RALLY_VIEW_REMOTESKIDMARKVIEW_H_
