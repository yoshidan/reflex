
#include "udpserver.h"

namespace udpserver {

	void acceptClient(Handler& handler, asio::io_context &ioc, asio::ip::udp::endpoint local_endpoint, asio::yield_context yield) {

		boost::system::error_code ec;

		asio::ip::udp::socket socket{ioc};
		if (local_endpoint.address().is_v4()) {
			socket.open(asio::ip::udp::v4(), ec);
		}else {
			socket.open(asio::ip::udp::v6(), ec);
		}
		if (ec) return logger::error( ec.message() +  "open");

		socket.bind(local_endpoint, ec);
		if (ec) return logger::error( ec.message() +  "bind");

		while(true) {
			Request request{socket};
			socket.async_receive_from(boost::asio::buffer(request.buffer.buffer), request.client, yield[ec]);
			if (ec) {
				logger::error( ec.message() +  "accept");
			}  else {
				logger::info("on message from " + request.client.address().to_string() + ":" + std::to_string(request.client.port()));
				asio::spawn(ioc,
							std::bind(
								handler,
								std::ref(ioc),
								std::move(request),
								std::placeholders::_1));
			}
		}
	}

	void startServer(const std::string ip , const unsigned short p , asio::io_context& ioc, Handler&& handler) {

		auto const address = asio::ip::make_address(ip);

		asio::spawn(ioc,
					std::bind(
						&acceptClient,
						handler,
						std::ref(ioc),
						asio::ip::udp::endpoint{address, p},
						std::placeholders::_1));


	}
}

