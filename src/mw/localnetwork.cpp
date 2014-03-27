#include "localnetwork.h"

#include <queue>

namespace mw {

	LocalNetwork::LocalNetwork(ServerFilter* serverFilter) {
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
		if (getStatus() == Network::NOT_ACTIVE) {
			setStatus(Network::ACTIVE);
		}
	}

	void LocalNetwork::stop() {
		if (getStatus() == Network::ACTIVE) {
			setStatus(Network::NOT_ACTIVE);
		}
	}

	void LocalNetwork::update() {
	}

	std::vector<int> LocalNetwork::getConnectionIds() const {
		std::vector<int> ids;
		ids.push_back(getId());
		return ids;
	}

	// Return the same id as server id.
	int LocalNetwork::getId() const {
		return getServerId();
	}

} // Namespace mw.
