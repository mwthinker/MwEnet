#include "localnetwork.h"

#include <queue>
#include <cassert>

namespace mw {

	LocalNetwork::LocalNetwork(ServerInterface& serverFilter) : serverFilter_(serverFilter) {
		status_ = Network::NOT_ACTIVE;
	}

	void LocalNetwork::pushToSendBuffer(const Packet& packet, PacketType type, int toId) {	
		if (packet.size() > 0 ) {
			if (toId == 0) {
				pushToSendBuffer(packet,type);
			} else if (toId == 1) {
				serverFilter_.receiveToServer(packet, getId());
			} else if (toId == getId()) {
				buffer_.push(packet);
			} else {
				// Id not assign to local client or server!
				assert(0);
			}
		}
	}

	// Send data to receiveBuffer.
	// Send data through serverfilter (if there is one).
	void LocalNetwork::pushToSendBuffer(const Packet& packet, PacketType type) {
		pushToSendBuffer(packet, type, 0);
	}

	// Receives data from server.	
	int LocalNetwork::pullFromReceiveBuffer(Packet& packet) {
		if (buffer_.empty()) {
			return 0;
		}
		packet = buffer_.front();
		buffer_.pop();
		// There is no remote package to be received.
		return getId();
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
