#include "view/RemoteCarView.h"

#include "model/RemoteCar.h"

#include <sstream>
#include <string>

namespace Rally { namespace View {

    RemoteCarView::RemoteCarView(const Rally::Model::RemoteCar& remoteCar) :
            remoteCar(remoteCar) {
    }

    RemoteCarView::~RemoteCarView() {
    }

     void RemoteCarView::attachTo(Ogre::SceneManager* sceneManager, const std::string& carName) {
        CarView::attachTo(sceneManager, carName);
		skidmarkView.attachTo(sceneManager);
    }

    void RemoteCarView::updateWithRemoteCar() {
        Rally::Quaternion carOrientation = remoteCar.getOrientation();
        Rally::Quaternion wheelRotation = carOrientation * remoteCar.getWheelRotation();

        this->updateBody(remoteCar.getPosition(), carOrientation);
        this->updateWheels(wheelRotation, wheelRotation, wheelRotation, wheelRotation);
        
        skidmarkView.update(remoteCar);
    }

} }
