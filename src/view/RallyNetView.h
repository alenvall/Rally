#ifndef RALLY_VIEW_RALLYNETVIEW_H_
#define RALLY_VIEW_RALLYNETVIEW_H_

#include "model/Car.h"
#include <map>

namespace Rally { namespace View {

    class RallyNetView_NetworkCarListener {
        public:
            virtual ~RallyNetView_NetworkCarListener();

            /** Called when a car is added or updated. */
            virtual void carUpdated(unsigned short carId,
                Rally::Vector3 position,
                Rally::Vector3 orientation,
                Rally::Vector3 velocity) = 0;

            /** Called when a car is removed due to disconenct/timeout. */
            virtual void carRemoved(unsigned short carId) = 0;
    };

    struct RallyNetView_InternalClient {
        InternalClient() :
            lastPacketArrival(0),
            lastPositionSequenceId(0) {
        }

        time_t lastPacketArrival;
        unsigned short lastPositionSequenceId;
    };

    class RallyNetView {
        public:
            RallyNetView(const RallyNetView_NetworkCarListener & listener);
            virtual ~RallyNetView();
            bool initialize(const std::string & server, unsigned short port);

            void update();

        private:
            int socket;
            unsigned short lastSentPacketId;
            RallyNetView_NetworkCarListener& listener;
            std::map<unsigned short, RallyNetView_InternalClient> clients;
    };

} }

#endif // RALLY_VIEW_RALLYNETVIEW_H_
