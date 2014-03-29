#include "localnetwork.h"

#include <queue>

namespace mw {

	LocalNetwork::LocalNetwork(ServerFilter* serverFilter) {
		status_ = Network::NOT_ACTIVE;
		serverFilter_ = serverFilter;
	}

	void LocalNetwork::pushToSendBuffer(const Packet& packet, PacketType type, int toId) {	
		if (packet.size() > 0 ) {
			if (toId == 0) {
				pushToSendBuffer(packet,type);
			} else if (serverFilter_->sendThrough(packet,getId(),toId,ServerFilter::PACKET)) {
				buffer_.push(packet);
			}
		}
	}

	// Send data to receiveBuffer.
	// Send data through serverfilter (if there is one).
	void LocalNetwork::pushToSendBuffer(const Packet& packet, PacketType type) {
		if (packet.size() > 0 ) {
			// Add to the receive buffer and pass it through the server filter.
			buffer_.push(packet);
			serverFilter_->sendThrough(packet,getId(),0,ServerFilter::PACKET);
			// There are no more clients so the result of serverfilter 
			// doesn't matter.
		}
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

	// Return the same id as server id.
	int LocalNetwork::getId() const {
		return Network::SERVER_ID;
	}

	Network::Status LocalNetwork::getStatus() const {
		return status_;
	}

} // Namespace mw.
