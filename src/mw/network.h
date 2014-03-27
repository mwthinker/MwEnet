#ifndef MW_NETWORK_H
#define MW_NETWORK_H

#include "packet.h"

#include <vector>

namespace mw {

	class ServerFilter {
	public:
		virtual ~ServerFilter() {
		}

		// Reprent the type of packet rereceived.
		// NEW_CONNECTION - A new connection from a client.
		// PACKET - Data stored in a packet.
		// DISCONNECTED - A client disconnected.
		enum Type {
			NEW_CONNECTION, PACKET, DISCONNECTED
		};

		// The server passes a received packet (packet) through. It's sent from client
		// with id (fromId) to client with id (toId). The type (type) represents the type of packet received.
		// The derived serverFilter class is responsible of returning the packet 
		// which then will be delegated to all clients' (and the server's) receive buffer.
		// Should return true if the packet should be delegated through to clients else false.
		// A packet sent inside this function will be sent before the passing packet.
		virtual bool sendThrough(const Packet& packet, int fromId, int toId, Type type) = 0;
	};

	// This class works as a multiuser system. Should be used
	// to control a server/client/local system. Helps to create
	// one system instead of three. Is supposed to be inherited by
	// server/client/local classes.
	// The server is responsible to give all client a unique  value.
	class Network {
	public:
		enum Status {
			ACTIVE, DISCONNECTING, NOT_ACTIVE
		};

		Network() {
			status_ = NOT_ACTIVE;
		}

		virtual ~Network() {
		}

		// Push the data (packet) to be sent to a specific client with id (toId). 
		// Id equals zero is the same as calling pushToSendBuffer(const Packet&, PacketType).
		// If id is a real client the data will go through the server before going to the choosen client 
		// (only if the server let it through to the choosen client).
		// The whole buffer will be sent in the next call to update() and in the same 
		// order as data was added.
		// Only data pushed after the call to start() will be sent.
		virtual void pushToSendBuffer(const Packet& packet, PacketType type, int toId) = 0;

		// Push the data (packet) to be sent to all clients (including the server).
		// The data will be copied to the receive buffer and copied to the send buffer.		
		// The whole buffer will be sent in the next call to update() and in the same 
		// order as data was added.
		// When the data is sent it will go to the server passing the server filter and if the 
		// server allowes the data free pass it will end up at all other clients.
		// Only data pushed after the call to start() will be sent.	
		virtual void pushToSendBuffer(const Packet& packet, PacketType type) = 0;

		// Receives data from server.
		// Receives reliable data from all clients and server. 
		// Reliable packet (packet) sent through 
		// pushToSendBuffer is passed directly here (without going through internet).
		virtual int pullFromReceiveBuffer(Packet& packet) = 0;

		// Starts the connection to the server/clients.
		virtual void start() = 0;

		// Ends all active connections.
		virtual void stop() = 0;

		// Must be called frequently in order for the new packets to be sent and received.
		virtual void update() = 0;

		virtual std::vector<int> getConnectionIds() const = 0;

		// Returns the id. The id is assigned by the server. As long as the id
		// is a positive value it is valid. The id is guarantied to not change after
		// it is assigned by the server after a call to start, as long as the connection
		// is active.
		virtual int getId() const = 0;

		// Returns the server id. Is allways a positive value greater than zero.
		static int getServerId() {
			return 1;
		}

		// Returns the current status for the network.
		Status getStatus() const {
			return status_;
		}

		// Returns the number of connections. For a client it returns 1.
		// For the server it returns the number of connected clients.
		virtual int getNbrOfConnections() const {
			return 0;
		}

	protected:
		void setStatus(Status status) {
			status_ = status;
		}

	private:
		Status status_;
	};

} // Namespace mw.

#endif // MW_NETWORK_H
