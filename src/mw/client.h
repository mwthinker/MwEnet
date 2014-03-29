#ifndef MW_CLIENT_H
#define MW_CLIENT_H

#include "enetnetwork.h"

#include <string>
#include <vector>
#include <thread>
#include <mutex>

namespace mw {

	class Client : public EnetNetwork {
	public:
		Client(int port, std::string ip);
		~Client();

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

#endif // MW_CLIENT_H
