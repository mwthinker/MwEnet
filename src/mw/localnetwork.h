#ifndef MW_LOCALNETWORK_H
#define MW_LOCALNETWORK_H

#include "network.h"
#include "server.h"

#include <queue>

namespace mw {

	class LocalNetwork : public Network, public Server {
	public:
		LocalNetwork(ServerInterface& serverFilter);

		void pushToSendBuffer(const Packet& packet, PacketType type, int toId) override;

		// Send data to receiveBuffer.
		// Send data through serverfilter (if there is one).
		void pushToSendBuffer(const Packet& packet, PacketType type) override;

		// Receives data from server.	
		int pullFromReceiveBuffer(Packet& packet) override;

		void serverPushToSendBuffer(const Packet& packet, Network::PacketType type, int toId) override;

		void serverPushToSendBuffer(const Packet& packet, Network::PacketType type) override;

		void start() override;

		void stop() override;

		// Return the same id as server id.
		int getId() const override;

		Status getStatus() const override;

	private:
		void pushToSendBuffer(const Packet& packet, Network::PacketType type, int toId, bool server);

		struct Data {
			Data() {
			}

			Data(const Packet& packet, bool server) : packet_(packet), server_(server) {
			}

			Packet packet_;
			bool server_;
		};

		Status status_;

		std::queue<Data> buffer_;
		ServerInterface& serverFilter_;
	};

} // Namespace mw.

#endif // MW_LOCALNETWORK_H
