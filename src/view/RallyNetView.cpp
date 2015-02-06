#if _WIN32
#define PLATFORM_WINDOWS
#endif

#include "view/RallyNetView.h"

#if PLATFORM_WINDOWS
#include <WinSock2.h>
//#include <fcntl.h>
#pragma comment(lib, "wsock32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
// Not needed here
//#include <sys/types.h>
#include <arpa/inet.h>
//#include <netdb.h>
// close() is in here for newer gcc
#include <unistd.h>
#include <errno.h>
#endif

#include <stdexcept>
#include <map>
#include <climits>

namespace Rally { namespace View {

    namespace {
        const unsigned int MAX_PACKET_SIZE = 255;
        const unsigned int CLIENT_TIMEOUT_DELAY = 40; // seconds

        // Some ugly code to fill in a vector3 to a packet. Should endian-convert from
        // host to network byte order if necessary. (Usually it isn't necessary for float.)
        void writeVector3toPacket(char* packet, const Rally::Vector3 & vector) {
            memcpy(packet + 0*4, &vector.x, 4);
            memcpy(packet + 1*4, &vector.y, 4);
            memcpy(packet + 2*4, &vector.z, 4);
        }

        // Takes a packet with offset pre-added and returns a Vector3. This should
        // convert from network to host byte order if necessary (usually not for float).
        Rally::Vector3 packetToVector3(char* packet) {
            float* a = reinterpret_cast<float*>(packet + 0*4);
            float* b = reinterpret_cast<float*>(packet + 1*4);
            float* c = reinterpret_cast<float*>(packet + 2*4);

            return Rally::Vector3(*a, *b, *c);
        }

        // Windows does it another way. Actually a better way too...
        int getErrno() {
#ifdef PLATFORM_WINDOWS
            return ::WSAGetLastError();
#else
            return errno;
#endif
        }
    }

    RallyNetView::RallyNetView(RallyNetView_NetworkCarListener & listener)
        : socket(0),
        lastSentPacketId(0),
        listener(listener) {
    }

    RallyNetView::~RallyNetView() {
    #ifdef PLATFORM_WINDOWS
        if(socket) ::closesocket(socket);
        ::WSACleanup();
    #else
        if(socket) ::close(socket);
    #endif
    }

    void RallyNetView::initialize(const std::string & serverAddress, unsigned short serverPort, Model::Car & playerCar) {
        this->playerCar = playerCar;

    #ifdef PLATFORM_WINDOWS
        WSADATA WsaData;
        if(::WSAStartup(MAKEWORD(2, 2), &WsaData) != NO_ERROR) {
            throw std::runtime_error("WSAStartup failed!");
        }
    #endif

        socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(socket <= 0) {
            throw std::runtime_error("Could not connect!");
        }

        sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        // TODO: use getaddrinfo() if we need more flexibility in the future
        address.sin_addr.s_addr = inet_addr(serverAddress.c_str());
        address.sin_port = htons(serverPort);

        // Note that connect assigns the local server:port choosen by the OS to this socket!
        if(::connect(socket, (const sockaddr*) & address, sizeof(sockaddr_in)) < 0) {
            // Throwing here is ok, since we're using UDP so no connection will actually happen,
            // meaning it won't fail because of network problems.
            throw std::runtime_error("Could not connect to server!");
        }

        bool nonBlockSucceded = false;
#ifdef PLATFORM_WINDOWS
        DWORD nonBlocking = 1;
        nonBlockSucceded = (ioctlsocket(socket, FIONBIO, &nonBlocking) != 0);
#else
        int flags = fcntl(socket, F_GETFL, 0);
        flags &= ~O_NONBLOCK;
        nonBlockSucceded = (fcntl(socket, F_SETFL, flags) == 0);
#endif

        if(!nonBlockSucceded) {
            throw std::runtime_error("Could not make socket non-blocking!");
        }
    }

    void RallyNetView::update() {
        // TODO: Rate limit!
        updatePutCar();
        updateGetCars();
    }

    void RallyNetView::updatePutCar() {
        char packet[40];

        packet[0] = 1; // Type = 1

        // sequence id
        unsigned short packetId = htons(++lastSentPacketId);
        memcpy(packet + 1, &packetId, 2);

        packet[3]  = 0; // Car type = 0

        writeVector3toPacket(packet + 4 + 0*3*4, playerCar.getPosition());
        writeVector3toPacket(packet + 4 + 1*3*4, playerCar.getOrientation());
        writeVector3toPacket(packet + 4 + 2*3*4, playerCar.getVelocity());

        int status = ::send(socket, packet, sizeof(packet), 0x00000000);
        int failedBecauseNonBlocking = (getErrno() == EWOULDBLOCK || getErrno() == EAGAIN);
        if(status < 0 && !failedBecauseNonBlocking) {
            // Note that we don't care if we couldn't send all bytes above...
            throw std::runtime_error("Socket error when sending position update to server.");
        }
    }

    void RallyNetView::updateGetCars() {
        char packet[MAX_PACKET_SIZE];
        while(true) {
            int receivedBytes = ::recv(socket, packet, MAX_PACKET_SIZE, 0x00000000);
            if(receivedBytes == 42 && packet[0] == 1) { // complete packetType == 1
                unsigned short sequenceId;
                memcpy(&sequenceId, packet+1, 2);
                sequenceId = ntohs(sequenceId);

                unsigned short playerId;
                memcpy(&playerId, packet + 4, 2);
                playerId = ntohs(playerId);

                std::map<unsigned short, RallyNetView_InternalClient>::iterator sendingClientIterator = clients.find(playerId);

                // If client was found as previous sender, make sure this packet is relevant
                if(sendingClientIterator != clients.end()) {
                     if(sendingClientIterator->second.lastPositionSequenceId > sequenceId) {
                        // Possible wrap-around?
                        if(sendingClientIterator->second.lastPositionSequenceId > USHRT_MAX/2 && sequenceId < USHRT_MAX/2) {
                            // If we use local 32-bit seq.id like the server in the future: lastPositionPacketId += USHRT_MAX + 1;
                        } else {
                            // Drop packet!
                            continue;
                        }
                    }
                }

                // Lazily create the internal client if not already existant.
                RallyNetView_InternalClient& internalClient = (sendingClientIterator == clients.end()) ? clients[playerId] : sendingClientIterator->second;

                internalClient.lastPositionSequenceId = sequenceId;
                internalClient.lastPacketArrival = time(0);

                char carType = packet[3];

                listener.carUpdated(
                    playerId,
                    packetToVector3(packet + 6 + 0*4*3),
                    packetToVector3(packet + 6 + 1*4*3),
                    packetToVector3(packet + 6 + 2*4*3));
            }
        }
    }
} }
