#ifndef MW_CLIENT_H
#define MW_CLIENT_H

#include "enetnetwork.h"

#include <string>
#include <vector>

namespace mw {

	class Client : public EnetNetwork {
	public:
		Client(int port, std::string ip);
		~Client();

		void start() override;
		
		void stop() override;
		
		int getId() const override;

	protected:
		void update();

		// Override from EnetConnection.
		InternalPacket receive(ENetEvent eNetEvent) override;

	private:
		int id_;
		std::vector<int> ids_;

		ENetPeer* peer_;
		ENetAddress address_;
		ENetHost* client_;
	};

} // Namespace mw.

#endif // MW_CLIENT_H
