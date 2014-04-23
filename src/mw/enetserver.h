#ifndef MW_ENETSERVER_H
#define MW_ENETSERVER_H

#include "enetnetwork.h"
#include "server.h"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace mw {

	class EnetServer : public EnetNetwork, public Server {
	public:
		EnetServer(int port, ServerInterface& serverInterface);
		~EnetServer();

		EnetServer(const EnetServer&) = delete;
		EnetServer& operator=(const EnetServer&) = delete;

		void serverPushToSendBuffer(const Packet& packet, PacketType type, int toId) override;

		void serverPushToSendBuffer(const Packet& packet, PacketType type) override;
		
		void start() override;
		
		void stop() override;

	protected:
		void update();

		InternalPacket receive(ENetEvent eNetEvent) override;

	private:
		typedef std::pair<ENetPeer*, int> Pair; // first: Peer second: id.

		// Sends connectInfo to new connected client. Client is assigned
		// the number id.
		// char type = |CONNECT_INFO
		// char id   = |id
		// char id1  = |?
		//		...
		// char idN  = |?
		void sendConnectInfoToPeers(const std::vector<Pair>& peers) const;
		
		int currentId_;
		std::vector<Pair> peers_;
		ENetAddress address_;
		ENetHost* server_;

		std::thread thread_;
		ServerInterface& serverInterface_;
	};

} // Namespace mw.

#endif // MW_ENETSERVER_H
