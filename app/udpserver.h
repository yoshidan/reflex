#ifndef REFLEX_UDPSERVER_H
#define REFLEX_UDPSERVER_H

#include <iostream>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/array.hpp>
#include <thread>
#include <string>
#include <cstdlib>
#include "logger.h"

namespace udpserver {

	namespace asio = boost::asio;

	template <size_t Size>
	class Reader{

	private:
		size_t pos = 0;

	public:
		boost::array<uint8_t, Size> buffer;

		void read(void* pointer, const size_t size){
			std::memcpy(pointer, buffer.elems + pos,  size);
			pos = pos + size;
		}
	};


	template <size_t Size> class Writer {

	private:

		size_t pos = 0;

	public:
		boost::array<uint8_t, Size> buffer;

		void write(void* data, size_t size){
			std::memcpy(buffer.elems + pos, data, size);
			pos = pos + size;
		}
		void writeUint8(uint8_t val)   {return write(&val, sizeof(val));}
		void writeUint16(uint16_t val) {return write(&val, sizeof(val));}
		void writeUint32(uint32_t val) {return write(&val, sizeof(val));}
	};

	using WebRTCStunBufferReader = Reader<20>;

	struct Request {
		WebRTCStunBufferReader buffer;
		asio::ip::udp::endpoint client;
		asio::ip::udp::socket& server;

		Request(asio::ip::udp::socket& server) : server(server){
		}
	};

	using Handler = std::function<void(boost::asio::io_context &ioc, Request, boost::asio::yield_context yield)>;

	void acceptClient(Handler& handler, asio::io_context &ioc, asio::ip::udp::endpoint local_endpoint, asio::yield_context yield);

	void startServer(const std::string ip , const unsigned short p , asio::io_context& ioc, Handler&& handler);
}

#endif