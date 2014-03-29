#ifndef MW_LOCALNETWORK_H
#define MW_LOCALNETWORK_H

#include "network.h"

#include <queue>

namespace mw {

	class LocalNetwork : public Network {
	public:
		LocalNetwork(ServerFilter* serverFilter);

		void pushToSendBuffer(const Packet& packet, PacketType type, int toId) override;

		// Send data to receiveBuffer.
		// Send data through serverfilter (if there is one).
		void pushToSendBuffer(const Packet& packet, PacketType type) override;

		// Receives data from server.	
		int pullFromReceiveBuffer(Packet& packet) override;

		void start() override;

		void stop() override;

		void update() override;

		// Return the same id as server id.
		int getId() const override;

	private:
		std::queue<Packet> buffer_;
		ServerFilter* serverFilter_;
	};

} // Namespace mw.

#endif // MW_LOCALNETWORK_H
