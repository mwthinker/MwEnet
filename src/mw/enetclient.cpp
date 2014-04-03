#include "enetclient.h"

#include <algorithm>
#include <string>
#include <iostream>

namespace mw {

	EnetClient::EnetClient(int port, std::string ip) {
		status_ = NOT_ACTIVE;
		enet_address_set_host(&address_, ip.c_str());
		address_.port = port;
		id_ = -1;
		peer_ = 0;
	}

	EnetClient::~EnetClient() {
		stop();
		if (thread_.joinable()) {
			thread_.join();
		}
		
		if (peer_ != 0) {
			enet_peer_reset(peer_);
		}
		if (client_ != 0) {
			enet_host_destroy(client_);
		}
	}

	void EnetClient::start() {
		std::lock_guard<std::mutex> lock(mutex_);
		if (status_ == NOT_ACTIVE) {
			status_ = ACTIVE;

			id_ = -1;
			peer_ = 0;

			// Remove old package.
			while (!sendPackets_.empty()) {
				sendPackets_.pop();
			}
			while (!receivePackets_.empty()) {
				receivePackets_.pop();
			}

			// Create a host using enet_host_create
			client_ = enet_host_create(NULL, 1, 2, 0, 0);

			if (client_ == 0) {
				std::cerr << "An error occured while trying to create an ENet server host" << std::endl;
				exit(EXIT_FAILURE);
			}

			peer_ = enet_host_connect(client_, &address_, 2, 0);

			if (peer_ == 0) {
				std::cerr << "No available peers for initializing an ENet connection" << std::endl;
				exit(EXIT_FAILURE);
			}
			thread_ = std::thread(&EnetClient::update, this);
		}
	}

	void EnetClient::stop() {
		std::lock_guard<std::mutex> lock(mutex_);
		if (status_ == ACTIVE) {
			status_ = DISCONNECTING;
			if (peer_ != 0) {
				enet_peer_disconnect(peer_, 0);
			}
		}
	}

	void EnetClient::update() {
		mutex_.lock();
		Status tmp = status_;
		mutex_.unlock();
		while (tmp != NOT_ACTIVE) {
			mutex_.lock();
			ENetEvent eNetEvent;
			int eventStatus = 0;
			while (status_ != NOT_ACTIVE &&
				(eventStatus = enet_host_service(client_, &eNetEvent, 0)) > 0) {
				switch (eNetEvent.type) {
					case ENET_EVENT_TYPE_CONNECT:
						printf("(Client) We got a new connection from %x\n", eNetEvent.peer->address.host);
						peer_ = eNetEvent.peer;
						break;
					case ENET_EVENT_TYPE_RECEIVE:
						if (status_ != NOT_ACTIVE) {
							// Add to receive buffer.
							InternalPacket iPacket = receive(eNetEvent);
							if (iPacket.data_.size() > 0) {
								receivePackets_.push(iPacket);
							}
						} else {
							std::cerr << "Client receives data, STATUS == NOT_ACTIVE" << std::endl;
						}
						enet_packet_destroy(eNetEvent.packet);
						break;
					case ENET_EVENT_TYPE_DISCONNECT:
						printf("%s disconnected.\n", (char*) eNetEvent.peer->data);
						// Reset client's information
						eNetEvent.peer->data = NULL;
						status_ = NOT_ACTIVE;
						id_ = -1; // Is assigned by server.
						peer_ = 0;
						break;
					case ENET_EVENT_TYPE_NONE:
						break;
				}
			}

			// Send all packets in send buffert to all clients.
			// Must been assinged id and got a connection and not active
			while (id_ != -1 && id_ != 0 && peer_ != 0 && status_ != NOT_ACTIVE && !sendPackets_.empty()) {
				InternalPacket iPacket = sendPackets_.front();

				ENetPacket* eNetPacket = createEnetPacket(iPacket.data_, iPacket.toId_, iPacket.type_); // id is set to be the client which will receive it. id = 0 means every client.

				// Send the packet to the peer over channel id 0.
				// One could also broadcast the packet by
				// enet handles the cleen up of eNetPacket;
				enet_peer_send(peer_, 0, eNetPacket);

				// Cleen up.
				sendPackets_.pop();
			}

			// The client is not active? Or disconnecting is finished?
			if (status_ == NOT_ACTIVE || status_ == DISCONNECTING) {
				if (peer_ != 0) {
					enet_peer_reset(peer_);
					peer_ = 0;
				}
				enet_host_flush(client_);
				enet_host_destroy(client_);
				client_ = 0;
				status_ = NOT_ACTIVE;
			}

			tmp = status_;
			mutex_.unlock();
			std::chrono::milliseconds duration(50);
			std::this_thread::sleep_for(duration);
		}
	}

	EnetClient::InternalPacket EnetClient::receive(ENetEvent eNetEvent) {
		ENetPacket* packet = eNetEvent.packet;
		char type = packet->data[0];
		char id = packet->data[1];
		switch (type) {
			case CONNECT_INFO:
				id_ = id;
				ids_.clear();
				for (unsigned int i = 2; i < packet->dataLength; ++i) {
					ids_.push_back(packet->data[i]);
				}
				break;
			case PACKET:
				//[0]=type,[1]=id,[2...] = data
				return InternalPacket(Packet((char*) packet->data + 2, packet->dataLength - 2), id, PacketType::RELIABLE); // Doesn't matter which packet type!
		}
		return InternalPacket(Packet(), 0, PacketType::RELIABLE);
	}

} // Namespace mw.
