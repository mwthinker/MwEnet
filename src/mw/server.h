#ifndef MW_SERVER_H
#define MW_SERVER_H

#include "packet.h"
#include "network.h"

#include <vector>

namespace mw {

	class ServerInterface {
	public:
		virtual ~ServerInterface() {
		}

		virtual void receiveToServer(const Packet& packet, int clientId) = 0;

		virtual bool connectToServer(int clientId) = 0;

		virtual void disconnectToServer(int clientId) = 0;
	};

	// This class works as a server.
	// The server is responsible to give all client a unique value and
	// serves as a relay station which relays all data to and from clients.
	class Server {
	public:
		virtual ~Server() {
		}

		virtual void serverPushToSendBuffer(const Packet& packet, Network::PacketType type, int toId) = 0;

		virtual void serverPushToSendBuffer(const Packet& packet, Network::PacketType type) = 0;
	};

} // Namespace mw.

#endif // MW_SERVER_H
