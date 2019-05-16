#ifndef REFLEX_BINDING_H
#define REFLEX_BINDING_H

#include <boost/asio/spawn.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/array.hpp>
#include "../udpserver.h"
#include "../domain/StunHeader.h"
#include "../repository/StunRepository.h"

namespace controller {

	inline void error(boost::system::error_code &ec) {
		if (ec) {
			std::cerr << ec << std::endl;
		} else {
			logger::info("send stun message success");
		}
	}

	inline void handleStunRequest(const boost::asio::io_context &ioc, udpserver::Request &&request,
						   const boost::asio::yield_context &yield) {

		auto const stunRepository = repository::StunRepository();

		auto const requestHeader = stunRepository.read(request.buffer);
		if (!requestHeader.validateRequest()) {
			logger::info("bad request:");
			auto responseHeader = requestHeader.createForErrorResponse();
			responseHeader.bodyLengthByte = 8;
			domain::ErrorCodeAttributeValue attributeValue(0x04, 0x00);
			domain::ErrorCodeAttribute attribute(4, attributeValue);

			auto const buffer = stunRepository.error(responseHeader, attribute);

			boost::system::error_code ec;
			request.server.async_send_to(boost::asio::buffer(buffer.buffer), request.client, yield[ec]);
			error(ec);
			return;

		} else {
			// webrtct doesn't use stun body so ignore body
			auto responseHeader = requestHeader.createForSuccessResponse();

			if (request.client.address().is_v4()) {
				responseHeader.bodyLengthByte = 12;
				// RFC 5389
				// X-ポートは、マップされたポートをホストバイトオーダーで取得して、マジッククッキーの最上位16ビットでそれをXORして、そしてその後、その結果をネットワークバイトオーダーへ変換することで計算される
				auto topBit = (uint16_t) (requestHeader.magicCookie >> 16);
				auto port = request.client.port() ^ topBit;
				auto address = request.client.address().to_v4().to_uint() ^ requestHeader.magicCookie;
				domain::IpV4MappedAddressAttributeValue attributeValue(address, port);
				domain::XorMappedAddressAttribute<domain::IpV4MappedAddressAttributeValue> attribute(8, attributeValue);

				auto const buffer = stunRepository.write(responseHeader, attribute);
				boost::system::error_code ec;
				request.server.async_send_to(boost::asio::buffer(buffer.buffer), request.client, yield[ec]);
				error(ec);
			} else {
				responseHeader.bodyLengthByte = 24;
				auto topBit = (uint16_t) (requestHeader.magicCookie >> 16);
				auto port = request.client.port() ^ topBit;

				// 0xABCDEFの時 bytes[0] = AB, bytes[1] = CDのようにビッグエンディアン(NBO)となる。
				auto bytes = request.client.address().to_v6().to_bytes();
				domain::ipv6Address address{};
				int size = bytes.size();
				for (auto i = 0; i < size; i++){
					address[i] = bytes[i];
				}

				// read時にHBOに復元しているのでNBOに戻す。
				// 全部NBOで合わせて計算する
				auto magicCookieNBO = htonl(requestHeader.magicCookie);
				address[0] = address[0] ^ (uint8_t)(magicCookieNBO & 0x000000ff);
				address[1] = address[1] ^ (uint8_t)((magicCookieNBO & 0x0000ff00) >> 8);
				address[2] = address[2] ^ (uint8_t)((magicCookieNBO & 0x00ff0000) >> 16);
				address[3] = address[3] ^ (uint8_t)(magicCookieNBO >> 24);
				for(auto i = 0; i < 12; i++) {
					address[i + 4] = address[i + 4] ^ (requestHeader.transactionId[i]);
				}
				domain::IpV6MappedAddressAttributeValue attributeValue(address, port);
				domain::XorMappedAddressAttribute<domain::IpV6MappedAddressAttributeValue> attribute(20, attributeValue);

				auto const buffer = stunRepository.write(responseHeader, attribute);
				boost::system::error_code ec;
				request.server.async_send_to(boost::asio::buffer(buffer.buffer), request.client, yield[ec]);
				error(ec);
			}
		}

	}

}

#endif
