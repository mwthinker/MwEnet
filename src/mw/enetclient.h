#ifndef MW_ENETCLIENT_H
#define MW_ENETCLIENT_H

#include "enetnetwork.h"

#include <string>
#include <vector>
#include <thread>
#include <mutex>

namespace mw {

	class EnetClient : public EnetNetwork {
	public:
		EnetClient(int port, std::string ip);
		~EnetClient();

		void start() override;
		
		void stop() override;

	protected:
		void update();

		InternalPacket receive(ENetEvent eNetEvent) override;

	private:
		std::vector<int> ids_;

		ENetPeer* peer_;
		ENetAddress address_;
		ENetHost* client_;
		std::thread thread_;
	};

} // Namespace mw.

#endif // MW_ENETCLIENT_H
