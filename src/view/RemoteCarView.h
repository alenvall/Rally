#ifndef RALLY_VIEW_REMOTECARVIEW_H_
#define RALLY_VIEW_REMOTECARVIEW_H_

#include "view/CarView.h"

#include "Rally.h"

namespace Rally { namespace Model {
    class RemoteCar;
} }

namespace Rally { namespace View {

class RemoteCarView : public CarView {
    public:
        RemoteCarView(const Rally::Model::RemoteCar& remoteCar);
        virtual ~RemoteCarView();
        void updateWithRemoteCar();

    private:
        const Rally::Model::RemoteCar& remoteCar;
};

} }

#endif // RALLY_VIEW_REMOTECARVIEW_H_
