#include "enetserver.h"

#include <vector>
#include <iostream>

namespace mw {

	EnetServer::EnetServer(int port, ServerInterface& serverInterface) : serverInterface_(serverInterface) {
		status_ = NOT_ACTIVE;

		address_.host = ENET_HOST_ANY;
		address_.port = port;

		// Garanties that the server always has the correct id.
		id_ = Network::SERVER_ID + 1;

		// The id to be assigned to the next connected client.
		currentId_ = id_ + 1;
	}

	EnetServer::~EnetServer() {
		stop();
		if (thread_.joinable()) {
			thread_.join();
		}

		for (auto& pair : peers_) {
			enet_peer_reset(pair.first);
		}
		if (server_ != 0) {
			enet_host_destroy(server_);
		}
	}

	void EnetServer::serverPushToSendBuffer(const Packet& packet, PacketType type, int toId) {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			// Copy buffert to send buffert. Assign the correct sender id.
			if (packet.size() > 0) {
				if (toId == 0) {
					pushToSendBuffer(packet, type);
				} else {
					// Send to others!
					sendPackets_.push(InternalPacket(packet, SERVER_ID, type, toId));
				}
			}
		}
		condition_.notify_one();
	}

	void EnetServer::serverPushToSendBuffer(const Packet& packet, PacketType type) {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (packet.size() > 0) {
				// Send to all, id = 0.
				sendPackets_.push(InternalPacket(packet, SERVER_ID, type, 0));
				// Send to local client.
				receivePackets_.push(InternalPacket(packet, SERVER_ID, type, id_));
			}
		}
		condition_.notify_one();
	}

	void EnetServer::start() {
		std::lock_guard<std::mutex> lock(mutex_);
		if (status_ == NOT_ACTIVE) {
			status_ = ACTIVE;
			// Remove old packet.
			while (!sendPackets_.empty()) {
				sendPackets_.pop();
			}
			while (!receivePackets_.empty()) {
				receivePackets_.pop();
			}

			// Create a host.
			server_ = enet_host_create(&address_, 32, 2, 0, 0);
			if (server_ == NULL) {
				fprintf(stderr, "An error occured while trying to create an ENet server host\n");
				exit(EXIT_FAILURE);
			}

			thread_ = std::thread(&EnetServer::update, this);
		}
	}

	void EnetServer::stop() {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (status_ == ACTIVE) {
				status_ = DISCONNECTING;
				for (auto it = peers_.begin(); it != peers_.end(); ++it) {
					ENetPeer* peer = it->first;
					enet_peer_disconnect(peer, 0);
				}
			}
		}
		condition_.notify_one();
	}

	void EnetServer::update() {
		mutex_.lock();
		while (status_ != NOT_ACTIVE) {
			ENetEvent eNetEvent;
			int eventStatus = 0;
			while (status_ != NOT_ACTIVE &&
				(eventStatus = enet_host_service(server_, &eNetEvent, 0)) > 0) {
				switch (eNetEvent.type) {
					case ENET_EVENT_TYPE_CONNECT:
						//printf("(Server) We got a new connection from %x\n",eNetEvent.peer->address.host);
						if (status_ != DISCONNECTING) {
							// Signal the client that a new client is connected!
							// Is the connection accepted?
							int tmpId_ = currentId_ + 1;
							++currentId_;
							// Already own the mutex.
							std::unique_lock<std::mutex> lock(mutex_, std::adopt_lock);
							condition_.wait(lock);
							if (serverInterface_.connectToServer(tmpId_)) {
								// Assign id to client and set the next id to an uniqe value.
								Pair pair(eNetEvent.peer, tmpId_);
								peers_.push_back(pair);

								// Send info about the new client to everybody.
								sendConnectInfoToPeers(peers_);
							} else {
								enet_peer_disconnect(eNetEvent.peer, 0);
							}
							// To keep the mutex locked!
							lock.release();
						} else {
							// Stops new connections to be made.
							enet_peer_disconnect(eNetEvent.peer, 0);
						}
						break;
					case ENET_EVENT_TYPE_RECEIVE:
						if (status_ != NOT_ACTIVE) {
							InternalPacket iPacket = receive(eNetEvent);
							
							// No data to receive?
							if (iPacket.data_.size()) {
								// Already own the mutex.
								std::unique_lock<std::mutex> lock(mutex_, std::adopt_lock);
								condition_.wait(lock);
								// Sent to who?
								if (iPacket.toId_ == SERVER_ID) { // To server?
									serverInterface_.receiveToServer(iPacket.data_, iPacket.fromId_);
								} else if (iPacket.toId_ != 0) { // Sent to a specific client?
									if (iPacket.toId_ == id_) { // Sent to local client.
										receivePackets_.push(iPacket);
									} else { // Sent to Remote client
										sendPackets_.push(iPacket);
									}									
								} else { // Sent to all!
									receivePackets_.push(iPacket);
									sendPackets_.push(iPacket);
								}
								// To keep the mutex locked!
								lock.release();
							}
						}
						enet_packet_destroy(eNetEvent.packet);
						break;
					case ENET_EVENT_TYPE_DISCONNECT:
					{
						printf("%s disconnected.\n", (char*) eNetEvent.peer->data);
						// Reset client's information
						auto it = peers_.begin();
						for (; it != peers_.end(); ++it) {
							if (it->first == eNetEvent.peer) {
								break;
							}
						}

						// Remove the connection if its old (i.e. it is in the vector
						// and not a turned down connection).
						if (it != peers_.end()) {
							InternalPacket iPacket(Packet(), it->second, PacketType::RELIABLE);
							// Already own the mutex.
							std::unique_lock<std::mutex> lock(mutex_, std::adopt_lock);
							condition_.wait(lock);
							// Signal the server that a client is disconnecting.
							serverInterface_.disconnectToServer(iPacket.fromId_);
							lock.release();
							// Remove peer from vector.
							std::swap(*it, peers_.back());
							peers_.pop_back();

							// Send the updated client list to all clients.
							sendConnectInfoToPeers(peers_);
						}

						// When all peers is disconnected, then clean up.
						if (status_ == DISCONNECTING && peers_.size() == 0) {
							eNetEvent.peer->data = NULL;
							status_ = NOT_ACTIVE;
						}

						eNetEvent.peer->data = NULL;
					}
						break;
					case ENET_EVENT_TYPE_NONE:
						break;
				}
			}

			// Send all packets in send buffer to all clients.
			while (status_ != NOT_ACTIVE && !sendPackets_.empty()) {
				InternalPacket& iPacket = sendPackets_.front();

				// Data to send? And data through the filter is allowed to be sent?
				if (iPacket.data_.size() > 0) {
					// Send the packet to the peer over channel id 0.
					// enet handles the cleen up of eNetPacket;
					for (auto it = peers_.begin(); it != peers_.end(); ++it) {
						int id = it->second;
						// Send to all?
						if (iPacket.toId_ == 0) {
							// The sender?
							if (iPacket.fromId_ == id) {
								// Skip to return data to the sender.
								continue;
							}
							ENetPacket* eNetPacket = createEnetPacket(iPacket.data_, iPacket.fromId_, iPacket.type_);
							ENetPeer* peer = it->first;
							enet_peer_send(peer, 0, eNetPacket);
						} else if (id == iPacket.toId_) { // Send to the specific client?
							ENetPacket* eNetPacket = createEnetPacket(iPacket.data_, iPacket.fromId_, iPacket.type_);
							ENetPeer* peer = it->first;
							enet_peer_send(peer, 0, eNetPacket);
							// Only send to one client.
							break;
						}
					}
				}

				sendPackets_.pop();
			}

			enet_host_flush(server_);

			// The server is not active? Or the disconnection is finish?
			if (status_ == NOT_ACTIVE || (status_ == DISCONNECTING && peers_.size() == 0)) {
				enet_host_destroy(server_);
				server_ = 0;
				status_ = NOT_ACTIVE;
			}
			
			mutex_.unlock();
			std::chrono::milliseconds duration(50);
			std::this_thread::sleep_for(duration);
			mutex_.lock();
		}
		mutex_.unlock();
	}

	EnetServer::InternalPacket EnetServer::receive(ENetEvent eNetEvent) {
		ENetPacket* packet = eNetEvent.packet;
		//char id = packet->data[1];
		//Find the id for the client which sent the package.
		auto it = peers_.begin();
		for (; it != peers_.end(); ++it) {
			if (it->first == eNetEvent.peer) {
				break;
			}
		}

		char id = it->second;
		// TODO!! Stop connection which violates the protocol.
		char type = packet->data[0];
		char toId = packet->data[1];
		switch (type) {
			case CONNECT_INFO:
				// ERROR. SERVER SHOULD ONLY SEND NOT RECEIVE CONNECT_INFO.
				// TODO!! Stop connection which violates the protocol.
				break;
			case PACKET:
				// [0]=type,[1]=id,[2...] = data
				return InternalPacket(mw::Packet((char*) packet->data + 2, packet->dataLength - 2), id, PacketType::RELIABLE, toId);
		}
		// TODO!! Stop connection which violates the protocol. ERROR.
		return InternalPacket(Packet(), 0, PacketType::RELIABLE);
	}

	// Sends connectInfo to new connected client. Client is assigned
	// the number id.
	// 0   char type = |CONNECT_INFO
	// 1   char id   = |id
	// 2   char id1  = |?
	//		...
	// N-1 char idN  = |?
	void EnetServer::sendConnectInfoToPeers(const std::vector<Pair>& peers) const {
		char data[256];
		data[0] = CONNECT_INFO;
		for (const auto& pair : peers) {
			data[1] = pair.second;
			int size = peers.size();
			for (int i = 0; i < size; ++i) {
				data[i + 2] = peers[i].second;
			}

			ENetPacket* eNetPacket = enet_packet_create(data, size + 2, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(pair.first, 0, eNetPacket);
		}
	}

} // Namespace mw.
