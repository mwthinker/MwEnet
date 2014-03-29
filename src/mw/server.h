#ifndef MW_SERVER_H
#define MW_SERVER_H

#include "enetnetwork.h"

namespace mw {

	class Server : public EnetNetwork {
	public:
		Server(int port, ServerFilter* serverFilter);
		~Server();
		
		void start() override;
		
		void stop() override;
		
		void update() override;
		
		int getId() const override;
		
		std::vector<int> getConnectionIds() const override;

	protected:
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

		int id_;
		int currentId_;
		std::vector<Pair> peers_;
		int maxNbrOfRemoteClients_;
		ENetAddress address_;
		ENetHost* server_;

		ServerFilter* serverFilter_;
	};

} // Namespace mw.

#endif // MW_SERVER_H
