#ifndef MW_PACKET_H
#define MW_PACKET_H

#include <array>
#include <algorithm>

namespace mw {

	class Packet {
	public:
		static const size_t MAX_SIZE = 128;

		Packet() {
			index_ = 0;
			size_ = 0;
		}

		Packet(const char* data, int size) {
			index_ = 0;
			std::copy(data, data + size, data_.data());
			size_ = size;
		}

		// Dangerous if the size of the packet is to big.
		Packet& operator<<(const Packet& packet) {
			std::copy(packet.data_.data(), packet.data_.data() + packet.size_, data_.data() + size_);
			size_ += packet.size_;
			return *this;
		}

		Packet& operator>>(char& byte) {
			byte = data_[index_++];
			return *this;
		}

		Packet& operator<<(char byte) {
			push_back(byte);
			return *this;
		}

		const char* getData() const {
			return data_.data();
		}

		int size() const {
			return size_;
		}

		inline void push_back(char byte) {
			data_[size_++] = byte;
		}

		char operator[](int index) const {
			return data_[index];
		}

		unsigned int dataLeftToRead() const {
			return size_ - index_;
		}

	private:
		std::array<char, MAX_SIZE> data_;
		int index_;
		int size_;
	};

} // Namespace mw.

#endif // MW_PACKET_H
