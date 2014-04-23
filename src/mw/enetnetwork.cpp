#include "enetnetwork.h"

#include <enet/enet.h>

#include <iostream>
#include <cstdio>
#include <queue>
#include <algorithm>
#include <array>

namespace mw {
	// ----------------------------------------------------------------------------
	// To do! Fix that packages cant be sent before a id is assigned to the client.
	// ----------------------------------------------------------------------------

	int EnetNetwork::nbrOfInstances = 0;

	EnetNetwork::EnetNetwork() {
		// a. Initialize enet
		if (nbrOfInstances < 1) {
			++nbrOfInstances;
			if (enet_initialize() != 0) {
				fprintf(stderr, "An error occured while initializing ENet.\n");
				//return EXIT_FAILURE;
			}
		}
	}

	EnetNetwork::~EnetNetwork() {
		--nbrOfInstances;
		if (nbrOfInstances < 1) {
			enet_deinitialize();
		}
	}

	void EnetNetwork::pushToSendBuffer(const Packet& packet, PacketType type, int toId) {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			// Copy buffert to send buffert. Assign the correct sender id.
			if (packet.size() > 0) {
				if (toId == 0) {
					pushToSendBuffer(packet, type);
				} else {
					// Send to others!
					sendPackets_.push(InternalPacket(packet, id_, type, toId));
				}
			}
		}
		condition_.notify_one();
	}

	void EnetNetwork::pushToSendBuffer(const Packet& packet, PacketType type) {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (packet.size() > 0) {
				// Send to all, id = 0.
				sendPackets_.push(InternalPacket(packet, id_, type, 0));
				// Sent from yourself, id = getId().
				receivePackets_.push(InternalPacket(packet, id_, type, id_));
			}
		}
		condition_.notify_one();
	}

	int EnetNetwork::pullFromReceiveBuffer(Packet& data) {
		int id;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (receivePackets_.empty()) {
				return 0;
			}

			InternalPacket& iPacket = receivePackets_.front();
			data = iPacket.data_;
			id = iPacket.fromId_;

			receivePackets_.pop();
		}
		condition_.notify_one();
		return id;
	}

	// Sends data packet from/to client.
	// 0 char type    |
	// 1 char id      |
	// 2 char data[N] |
	ENetPacket* EnetNetwork::createEnetPacket(const Packet& packet, char id, PacketType type) {
		char data[Packet::MAX_SIZE];
		unsigned int size = 2 + packet.size();
		if (size <= Packet::MAX_SIZE) {
			data[0] = PACKET;
			data[1] = id;
			std::copy(packet.getData(), packet.getData() + packet.size(), data + 2);
		}

		ENetPacket* eNetPacket = 0;
		if (type == PacketType::RELIABLE) {
			eNetPacket = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
		} else {
			eNetPacket = enet_packet_create(data, size, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
		}

		return eNetPacket;
	}

	Network::Status EnetNetwork::getStatus() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return status_;
	}

	int EnetNetwork::getId() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return id_;
	}

} // Namespace mw.
