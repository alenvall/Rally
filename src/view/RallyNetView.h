#ifndef RALLY_VIEW_RALLYNETVIEW_H_
#define RALLY_VIEW_RALLYNETVIEW_H_

#include "model/Car.h"
#include "util/Timer.h"

#include <map>

namespace Rally { namespace View {

    class RallyNetView_NetworkCarListener {
        public:
            virtual ~RallyNetView_NetworkCarListener() {
            }

            /** Called when a car is added or updated. */
            virtual void carUpdated(unsigned short carId,
                Rally::Vector3 position,
                Rally::Quaternion orientation,
                Rally::Vector3 velocity) = 0;

            /** Called when a car is removed due to disconenct/timeout. */
            virtual void carRemoved(unsigned short carId) = 0;
    };

    struct RallyNetView_InternalClient {
        RallyNetView_InternalClient() :
            lastPacketArrival(0),
            lastPositionSequenceId(0) {
        }

        time_t lastPacketArrival;
        unsigned short lastPositionSequenceId;
    };

    class RallyNetView {
        public:
            RallyNetView(RallyNetView_NetworkCarListener & listener);
            virtual ~RallyNetView();
            void initialize(const std::string & server, unsigned short port, const Model::Car* playerCar);

            void pullRemoteChanges();
            void pushLocalChanges();

        private:
            int socket;
            unsigned short lastSentPacketId;
            RallyNetView_NetworkCarListener& listener;
            std::map<unsigned short, RallyNetView_InternalClient> clients;
            const Model::Car* playerCar;
            Rally::Util::Timer rateLimitTimer;

            void pushCar();
            void pullCars();
            void cleanupClients();
    };

} }

#endif // RALLY_VIEW_RALLYNETVIEW_H_
