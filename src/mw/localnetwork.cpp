#include "localnetwork.h"

#include <queue>
#include <cassert>

namespace mw {

	LocalNetwork::LocalNetwork(ServerInterface& serverFilter) : serverFilter_(serverFilter) {
		status_ = Network::NOT_ACTIVE;
	}

	void LocalNetwork::pushToSendBuffer(const Packet& packet, Network::PacketType type, int toId, bool server) {
		if (packet.size() > 0) {
			if (toId == 0) {
				buffer_.push(Data(packet, server));
			} else if (toId == 1) {
				int id = server ? SERVER_ID : getId();
				serverFilter_.receiveToServer(packet, id);
			} else if (toId == getId()) {
				buffer_.push(Data(packet, server));
			} else {
				// Id not assign to local client or server!
				assert(0);
			}
		}
	}

	void LocalNetwork::pushToSendBuffer(const Packet& packet, PacketType type, int toId) {	
		pushToSendBuffer(packet, type, toId, false);
	}

	// Send data to receiveBuffer.
	// Send data through serverfilter (if there is one).
	void LocalNetwork::pushToSendBuffer(const Packet& packet, PacketType type) {
		pushToSendBuffer(packet, type, 0, false);
	}

	void LocalNetwork::serverPushToSendBuffer(const Packet& packet, Network::PacketType type, int toId) {
		pushToSendBuffer(packet, type, toId, true);
		// Must be sent to the local client, or to everybody which is the same as the local client.
		assert(toId == 0 || toId == getId());
	}

	void LocalNetwork::serverPushToSendBuffer(const Packet& packet, Network::PacketType type) {
		pushToSendBuffer(packet, type, 0, true);
	}

	// Receives data from server.	
	int LocalNetwork::pullFromReceiveBuffer(Packet& packet) {
		if (buffer_.empty()) {
			return 0;
		}
		Data& data = buffer_.front();
		packet = data.packet_;
		int id = data.server_ ? SERVER_ID : getId();

		buffer_.pop();
		// There is no remote package to be received.
		return id;
	}

	void LocalNetwork::start() {
		if (status_ == Network::NOT_ACTIVE) {
			status_ = Network::ACTIVE;
		}
	}

	void LocalNetwork::stop() {
		if (status_ == Network::ACTIVE) {
			status_ = Network::NOT_ACTIVE;
		}
	}
	
	int LocalNetwork::getId() const {
		return Network::SERVER_ID + 1;
	}

	Network::Status LocalNetwork::getStatus() const {
		return status_;
	}

} // Namespace mw.
