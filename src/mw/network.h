#ifndef MW_NETWORK_H
#define MW_NETWORK_H

#include "packet.h"

#include <vector>

namespace mw {

	// This class works as a multiuser system. Should be used
	// to control a server/client/local system.
	// The server is responsible to give all client a unique value and
	// serves as a relay station which relays all data to and from clients.
	class Network {
	public:
		static const int SERVER_ID = 1;

		enum Status {
			ACTIVE, DISCONNECTING, NOT_ACTIVE
		};

		enum PacketType {
			RELIABLE, UN_RELIABLE
		};		

		virtual ~Network() {
		}

		// Push the data (packet) to be sent to a specific client with id (toId).
		// Only data pushed after the call to start() will be sent.
		// toId = 0 sends the data to all clients.
		// toId = 1 sends the data to the server.
		// toId = N, N > 1 sends the data to the client with corresponding id.
		virtual void pushToSendBuffer(const Packet& packet, PacketType type, int toId) = 0;

		// Push the data (packet) to all clients.
		// Same as calling pushToSendBuffer(packet, type, 0).
		virtual void pushToSendBuffer(const Packet& packet, PacketType type) = 0;

		// Pulls received data.
		// Local sent data this client passes without going through the internet.
		virtual int pullFromReceiveBuffer(Packet& packet) = 0;

		// Start the connection to the server.
		virtual void start() = 0;

		// End all active connections.
		virtual void stop() = 0;

		// Return the id. The id is assigned by the server. The id is always positive
		// and higher than the server id. The id is guarantied to not change after it
		// is assigned by the server after a call to start, as long as the connection
		// is active.
		virtual int getId() const = 0;

		// Return the current status for the network.
		virtual Status getStatus() const = 0;
	};

} // Namespace mw.

#endif // MW_NETWORK_H
