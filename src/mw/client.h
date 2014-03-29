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

		// Override from EnetConnection.
		void start() override;
		// Override from EnetConnection.
		void stop() override;

		// Override from EnetConnection.
		void update() override;

		// Override from EnetConnection.
		int getId() const override;

	protected:
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
