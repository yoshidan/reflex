#ifndef REFLEX_STUNHEADER_H
#define REFLEX_STUNHEADER_H

#include <boost/array.hpp>
#include <iostream>
#include "udpserver.h"

namespace domain {

	const uint32_t STUN_COOKIE = 0x2112A442;
	const uint16_t STUN_MESSAGE_TYPE_BINDING= 0x0001;

	using transactionId = uint8_t[12];
	using ipv6Address = uint8_t[16];


	struct ErrorCodeAttributeValue {
		const uint16_t padding = 0x0000;
		const uint8_t clazz;
		const uint8_t errnum;

		ErrorCodeAttributeValue(uint8_t clazz, uint8_t errnum) : clazz(clazz), errnum(errnum){

		}
	};

	struct ErrorCodeAttribute {
		const uint16_t type = 0x0009;
		const uint16_t valueLengthByte;
		const ErrorCodeAttributeValue value;

		ErrorCodeAttribute(uint16_t valueLengthByte, ErrorCodeAttributeValue& value) : valueLengthByte(valueLengthByte), value(value){
		}
	};

	struct MappedAddressAttributeValue {
		const uint8_t offset = 0x00;
		const uint16_t port;

		explicit MappedAddressAttributeValue(uint16_t port) : port(port) {
		}
	};

	struct IpV4MappedAddressAttributeValue : MappedAddressAttributeValue {
		const uint8_t family = 0x01;
		const uint32_t address;

		IpV4MappedAddressAttributeValue(uint32_t address, uint16_t port) :address(address), MappedAddressAttributeValue(port){
		}

	};

	struct IpV6MappedAddressAttributeValue : MappedAddressAttributeValue{
		const uint8_t family = 0x02;
		const ipv6Address& address;

		IpV6MappedAddressAttributeValue(const ipv6Address& address, const uint16_t& port) : address(address), MappedAddressAttributeValue(port){
		}
	};

	template <class T> struct XorMappedAddressAttribute {
		const uint16_t type = 0x0020;
		const uint16_t valueLengthByte;
		const T& value;

		XorMappedAddressAttribute(uint16_t&& valueLengthByte, const T& value) : valueLengthByte(valueLengthByte), value(value){
		}
	};

	struct StunHeader {
		/**
		 * contains first 2bit 00,
		 * - BindingRequest is 0x0001
		 * - BindingResponse is 0x0101
		 */
		uint16_t messageType;
		uint16_t bodyLengthByte;
		uint32_t magicCookie;
		transactionId transactionId;

		StunHeader createForSuccessResponse() const;

		StunHeader createForErrorResponse() const;

		bool validateRequest() const;

		void fill(udpserver::WebRTCStunBufferReader& reader);

		template<size_t Size> void writeHeader(udpserver::Writer<Size>& writeBuffer) const;

		udpserver::Writer<32> createWritingBuffer(const domain::XorMappedAddressAttribute<domain::IpV4MappedAddressAttributeValue>& attribute) const;

		udpserver::Writer<44> createWritingBuffer(const domain::XorMappedAddressAttribute<domain::IpV6MappedAddressAttributeValue>& attribute) const;

		udpserver::Writer<32> createErrorBuffer(const domain::ErrorCodeAttribute& attribute) const;

	};

}

#endif
