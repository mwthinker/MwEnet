#ifndef MW_SERVER_H
#define MW_SERVER_H

#include "enetnetwork.h"

namespace mw {

	class Server : public EnetNetwork {
	public:
		Server(int port, ServerFilter* serverFilter);
		~Server();

		// Override from EnetConnection.
		void start() override;
		// Override from EnetConnection.
		void stop() override;
		// Override from EnetConnection.
		void update() override;

		// Override from EnetConnection.
		int getId() const override;

		// Override from EnetConnection.
		std::vector<int> getConnectionIds() const override;

	protected:
		// Override from EnetConnection.
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
