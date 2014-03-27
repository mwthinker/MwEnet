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
		acceptNewNetworkConnections_ = true;

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
		// Copy buffert to send buffert. Assign the correct sender id.
		if (packet.size() > 0) {
			if (toId == 0) {
				pushToSendBuffer(packet, type);
			} else {
				// Send to others!
				sendPackets_.push(InternalPacket(packet, getId(), type, toId));
			}
		}
	}

	void EnetNetwork::pushToSendBuffer(const Packet& packet, PacketType type) {
		if (packet.size() > 0) {
			// Send to all, id = 0.
			sendPackets_.push(InternalPacket(packet, getId(), type, 0));
			// Sent from yourself, id = getId().
			receivePackets_.push(InternalPacket(packet, getId(), type, getId()));
		}
	}

	int EnetNetwork::pullFromReceiveBuffer(Packet& data) {
		if (receivePackets_.empty()) {
			return 0;
		}

		InternalPacket& iPacket = receivePackets_.front();
		data = iPacket.data_;
		int id = iPacket.fromId_;

		receivePackets_.pop();
		return id;
	}

	// Sends data packet from/to client.
	// 0 char type    |
	// 1 char id      |
	// 2 char data[N] |
	ENetPacket* EnetNetwork::createEnetPacket(const Packet& packet, char id, PacketType type) const {
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

} // Namespace mw.
