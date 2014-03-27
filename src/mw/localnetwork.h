#ifndef MW_LOCALNETWORK_H
#define MW_LOCALNETWORK_H

#include "network.h"

#include <queue>

namespace mw {

	class LocalNetwork : public Network {
	public:
		LocalNetwork(ServerFilter* serverFilter);

		// Override from Network.
		void pushToSendBuffer(const Packet& packet, PacketType type, int toId) override;

		// Override from Network.
		// Send data to receiveBuffer.
		// Send data through serverfilter (if there is one).
		void pushToSendBuffer(const Packet& packet, PacketType type) override;

		// Override from Network.
		// Receives data from server.	
		int pullFromReceiveBuffer(Packet& packet) override;

		// Override from Network.
		void start() override;

		// Override from Network.
		void stop() override;

		// Override from Network.
		void update() override;

		// Override from Network.
		std::vector<int> getConnectionIds() const override;

		// Override from Network.
		// Return the same id as server id.
		int getId() const override;

	private:
		std::queue<Packet> buffer_;
		ServerFilter* serverFilter_;
	};

} // Namespace mw.

#endif // MW_LOCALNETWORK_H
