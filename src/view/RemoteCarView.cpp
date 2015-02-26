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

    void RemoteCarView::updateWithRemoteCar() {
        this->updateBody(remoteCar.getPosition(), remoteCar.getOrientation());
    }

} }
