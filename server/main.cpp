#if _WIN32
#define PLATFORM_WINDOWS
#endif

#if PLATFORM_WINDOWS
#include <WinSock2.h>
//#include "fcntl.h"
#else
#include <sys/socket.h>
#include <netinet/in.h>
// Not needed here
//#include <sys/types.h>
#include <arpa/inet.h>
//#include <netdb.h>
// close() is in here for newer gcc
#include <unistd.h>
#endif

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <map>
#include <ctime>
#include <climits>
#include <cstring>

const unsigned int MAX_PACKET_SIZE = 255;
const unsigned int CLIENT_TIMEOUT_DELAY = 40; // seconds
const unsigned int RECEIVE_TIMEOUT = 10; // seconds, used to not hog the cpu in the main loop

// Only one packet is supported, of type 1 (player position).
// Layout:
// Byte 0: packet type (always 1)
// Byte 1..2: sequence ID
// Byte 3: car type (always 0)
// Byte 4..5, 6..7, 8..9: IEEEfloat16 x, y, z = position of car (world space)
// Byte 10..11, 12..13, 14..15: IEEEfloat16 x, y, z = orientation of car
// Byte 16..17, 18..19, 20..21: 3x IEEEfloat16 x, y, z = velocity of car
// Note that the orientation is NOT the same as the normalized velocity,
// as the velocity and heading differs when drifting, jumping etc.

struct ClientIdentifier {
    ClientIdentifier(unsigned int address, unsigned short port) :
        address(address),
        port(port) {
    }

    unsigned int address;
    unsigned short port;

    // TODO: Could make this friend operator<<
    std::string toString() const {
        std::stringstream out;

        in_addr ip;
        ip.s_addr = address;
        out << inet_ntoa(ip);// "1.2.3.4"

        out << ":";

        out << port;

        return out.str();
    }

    friend bool operator<(const ClientIdentifier & left, const ClientIdentifier & right);
};

bool operator<(const ClientIdentifier & left, const ClientIdentifier & right) {
    return left.address < right.address ||
        (left.address == right.address && left.port < right.port);
}

class ClientData {
    public:
        ClientData() :
            totalPackagesReceived(0),
            lastPositionPacketId(0),
            lastPacketArrival(0),
            playerId(NEXT_AVAILABLE_PLAYER_ID++) {
        }

        bool processPositionPacket(short packetId) {
            totalPackagesReceived++;

            unsigned short lastPacketIdShort = static_cast<unsigned short>(lastPositionPacketId);

            if(lastPacketIdShort > packetId) {
                // Possible wrap-around?
                if(lastPacketIdShort > USHRT_MAX/2 && packetId < USHRT_MAX/2) {
                    lastPositionPacketId += USHRT_MAX + 1;
                } else {
                    return false;
                }
            }

            // Keep everything except the last short, overwrite that.
            lastPositionPacketId = (lastPositionPacketId & (UINT_MAX xor USHRT_MAX)) + packetId;
            lastPacketArrival = ::time(0);
            return true;
        }

        bool isClientTimedOut(time_t now) {
            return (lastPacketArrival + CLIENT_TIMEOUT_DELAY < now);
        }

        bool getTotalPackagesReceived() {
            return totalPackagesReceived;
        }

        unsigned short getPlayerId() {
            return playerId;
        }

    private:
        bool newClient;
        unsigned int totalPackagesReceived;
        unsigned int lastPositionPacketId;
        time_t lastPacketArrival;
        unsigned short playerId;
        static unsigned short NEXT_AVAILABLE_PLAYER_ID;
};

unsigned short ClientData::NEXT_AVAILABLE_PLAYER_ID = 0;

int startServer(unsigned short serverPort) {
#ifdef PLATFORM_WINDOWS
    WSADATA WsaData;
    if(::WSAStartup(MAKEWORD(2, 2), &WsaData) != NO_ERROR) {
        throw std::runtime_error("WSAStartup failed!");
    }
#endif

    int socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket <= 0) {
        throw std::runtime_error("Could not connect!");
    }

    // Bind to any address
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(serverPort);
    if(::bind(socket, (const sockaddr*) & address, sizeof(sockaddr_in)) < 0) {
        throw std::runtime_error("Could not bind to socket!");
    }

    return socket;
}

ClientIdentifier receivePacket(int socket, char* packet, int* packetSize) {
    unsigned int addressSize = sizeof(sockaddr_in); // recvfrom requires unsigned int*

    sockaddr_in address;
    int receivedBytes = ::recvfrom(socket, packet, MAX_PACKET_SIZE, 0x00000000, (sockaddr*) &address, &addressSize);

    if(receivedBytes < 0) {
        throw std::runtime_error("recvfrom() failed.");
    }

    *packetSize = receivedBytes;

    ClientIdentifier clientIdentifier();

    return ClientIdentifier(ntohl(address.sin_addr.s_addr), htons(address.sin_port));
}

void broadcastPacket(int socket, char* packet, int packetSize, ClientIdentifier sendingClientIdentifier, const std::map<ClientIdentifier, ClientData> & clients) {
    std::map<ClientIdentifier, ClientData>::const_iterator sendingClientIterator = clients.find(sendingClientIdentifier);

    // Broadcast to all clients except the sender.
    for(std::map<ClientIdentifier, ClientData>::const_iterator clientIterator = clients.begin();
            clientIterator != clients.end();
            ++clientIterator) {
        if(clientIterator != sendingClientIterator) {
            sockaddr_in destination;
            memset(&destination, 0, sizeof(destination));
            destination.sin_family = AF_INET;
            destination.sin_addr.s_addr = htonl(clientIterator->first.address);
            destination.sin_port = htons(clientIterator->first.port);

            if(::sendto(socket, packet, packetSize, 0x00000000, (struct sockaddr*) &destination, sizeof(sockaddr_in)) < 0) {
                // Note that we don't care if we couldn't send all bytes above...
                throw std::runtime_error("Socket error when broadcasting message to client " + clientIterator->first.toString());
            }
        }
    }
}

int main(int argc, char** argv) {
    int socket = 0;

    std::map<ClientIdentifier, ClientData> clients;

    try {
        // Read port from stdin
        unsigned short serverPort = 1337;
        if(argc >= 2) {
            std::istringstream parser(argv[1]);
            if(!(parser >> serverPort)) {
                throw std::runtime_error("Oops, the port specified was invalid.");
            }
        }

        std::cout << "Starting Rally server..." << std::endl;

        socket = startServer(serverPort);

        std::cout << "Rally server started!" << std::endl;

        while(true) {
            // Create a set of sockets we can block on (containing one socket), waiting for anything to happen.
            fd_set sockets;
            FD_ZERO(&sockets);
            FD_SET(socket, &sockets);

#ifdef PLATFORM_WINDOWS
            DWORD timeout = RECEIVE_TIMEOUT;
#else
            timeval timeout;
            timeout.tv_sec = RECEIVE_TIMEOUT;
            timeout.tv_usec = 0;
#endif
            ::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

            // Some important notes:
            // select() changes the waiting set, we need to restore it each time.
            // Also, some platforms overwrite timeout! Therefore, we need to restore it too.
            int receivedAnything = ::select(socket + 1, &sockets, NULL, NULL, &timeout);

            if(receivedAnything > 0) {
                // Receive packet
                char packet[MAX_PACKET_SIZE];
                int packetSize;
                ClientIdentifier clientIdentifier = receivePacket(socket, packet, &packetSize);
std::cout << "Client sent." << packetSize << std::endl;
                // Process packet, possibly broadcast it
                if(packetSize == 40 && packet[0] == 1) {
                    ClientData clientData = clients[clientIdentifier]; // Will create if not found

                    unsigned short packetId = ntohs((packet[1]<<8) + packet[2]); // char -> short big endian -> short machine endian
                    if(clientData.processPositionPacket(packetId)) {
                        // Getting here means the packet was relevant/fresh

                        // We need to splice in the sender's playerId (it's server based)
                        unsigned short playerId = htons(clientData.getPlayerId());
                        packetSize += 2;
                        memmove(packet + 6, packet + 4, 36);
                        memcpy(packet + 4, &playerId, 2);

                        broadcastPacket(socket, packet, packetSize, clientIdentifier, clients);
                    }

                    if(clientData.getTotalPackagesReceived() == 1) {
                        std::cout << "Client " << clientIdentifier.toString() << " connected." << std::endl;
                    }

                    clients[clientIdentifier] = clientData;
                }
            } else if(receivedAnything < 0) {
                throw std::runtime_error("Error waiting on/select()ing from socket.");
            } else {
                // receivedAnything == 0 means timeout, just continue to cleanup and then retry.
            }

            // Cleanup internal map from timed out clients.
            time_t now = ::time(0);
            for(std::map<ClientIdentifier, ClientData>::iterator clientIterator = clients.begin();
                    clientIterator != clients.end();
                    ++clientIterator) {
                ClientData clientData = clientIterator->second;

                if(clientData.isClientTimedOut(now)) {
                    std::cout << "Client " << clientIterator->first.toString() << " timed out, after a total of " <<
                    clientData.getTotalPackagesReceived() << " valid packages received." << std::endl;

                    clients.erase(clientIterator);
                }
            }
        }
    } catch(std::exception& error) {
        std::cerr << "Caught exception, aborting. Exception: " << error.what() << std::endl;
#ifdef PLATFORM_WINDOWS
        if(socket) ::closesocket(socket);
        ::WSACleanup();
#else
        if(socket) ::close(socket);
#endif
    }

    return 0;
}
