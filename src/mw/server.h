#ifndef MW_SERVER_H
#define MW_SERVER_H

#include "enetnetwork.h"

#include <thread>
#include <mutex>

namespace mw {

	class Server : public EnetNetwork {
	public:
		Server(int port, ServerInterface& serverInterface);
		~Server();
		
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
		int maxNbrOfRemoteClients_;
		ENetAddress address_;
		ENetHost* server_;

		std::thread thread_;

		ServerInterface& serverInterface_;
	};

} // Namespace mw.

#endif // MW_SERVER_H
