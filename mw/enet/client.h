#ifndef MW_ENET_CLIENT_H
#define MW_ENET_CLIENT_H

#include "enetnetwork.h"

#include <string>

namespace mw {
	namespace enet {

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

			// Override from EnetConnection.
			std::vector<int> getConnectionIds() const override;

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

	} // Namespace enet.
} // Namespace mw.

#endif // MW_ENET_CLIENT_H
