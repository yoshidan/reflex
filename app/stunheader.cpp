
#include "stunheader.h"

namespace domain {

	void StunHeader::fill(udpserver::WebRTCStunBufferReader& reader) {
		size_t size = sizeof(*this);
		reader.read(this, size);

		messageType = ntohs(messageType);
		bodyLengthByte = ntohs(bodyLengthByte);
		magicCookie = ntohl(magicCookie);
	}

	StunHeader StunHeader::createForSuccessResponse() const {
		StunHeader responseHeader{};
		responseHeader.magicCookie = magicCookie;
		std::memcpy(responseHeader.transactionId, transactionId, 12);
		responseHeader.messageType = 0x0101; // success response
		return responseHeader;
	}

	StunHeader StunHeader::createForErrorResponse() const {
		StunHeader responseHeader{};
		responseHeader.magicCookie = magicCookie;
		std::memcpy(responseHeader.transactionId, transactionId, 12);
		responseHeader.messageType = 0x0111; // error response
		return responseHeader;
	}

	bool StunHeader::validateRequest() const {

		// allow only binding request
		if (messageType != STUN_MESSAGE_TYPE_BINDING) {
			std::stringstream hexString;
			hexString << std::hex << messageType;
			logger::error("invalid message type : " + hexString.str());
			return false;
		}
		if (magicCookie != STUN_COOKIE) {
			std::stringstream hexString;
			hexString << std::hex << magicCookie;
			logger::error("invalid magic cookie : " + hexString.str());
			return false;
		}

		if (bodyLengthByte % 4 != 0) {
			logger::error("length must be multiple of 32bit : " + std::to_string(bodyLengthByte));
			return false;
		}

		// WebRTCではボティはなし
		if (bodyLengthByte != 0) {
			logger::error("too long message length : " + std::to_string(bodyLengthByte));
			return false;
		}
		return true;
	}

	template<size_t Size> void StunHeader::writeHeader(udpserver::Writer<Size>& writeBuffer) const {
		writeBuffer.writeUint16(htons(messageType));
		writeBuffer.writeUint16(htons(bodyLengthByte));
		writeBuffer.writeUint32(htonl(magicCookie));
		auto size = sizeof(transactionId);
		writeBuffer.write(transactionId, size);
	}

	udpserver::Writer<32> StunHeader::createWritingBuffer(const domain::XorMappedAddressAttribute<domain::IpV4MappedAddressAttributeValue> &attribute) const {

		udpserver::Writer<32> writeBuffer;
		writeHeader(writeBuffer);

		writeBuffer.writeUint16(htons(attribute.type));
		writeBuffer.writeUint16(htons(attribute.valueLengthByte));
		writeBuffer.writeUint8(attribute.value.offset);
		writeBuffer.writeUint8(attribute.value.family);
		writeBuffer.writeUint16(htons(attribute.value.port));
		writeBuffer.writeUint32(htonl(attribute.value.address));

		return writeBuffer;
	}

	udpserver::Writer<44> StunHeader::createWritingBuffer(const domain::XorMappedAddressAttribute<domain::IpV6MappedAddressAttributeValue> &attribute) const {

		udpserver::Writer<44> writeBuffer;
		writeHeader(writeBuffer);

		writeBuffer.writeUint16(htons(attribute.type));
		writeBuffer.writeUint16(htons(attribute.valueLengthByte));
		writeBuffer.writeUint8(attribute.value.offset);
		writeBuffer.writeUint8(attribute.value.family);
		writeBuffer.writeUint16(htons(attribute.value.port));

		//NBO
		// ipv6 addresses are the same in either byte ordering - just an array of 16 bytes
		auto length = sizeof(attribute.value.address);
		for (int i = 0; i < length; i++) {
			writeBuffer.writeUint8(attribute.value.address[i]);
		}

		return writeBuffer;
	}

	udpserver::Writer<32> StunHeader::createErrorBuffer(const domain::ErrorCodeAttribute &attribute) const {

		udpserver::Writer<32> writeBuffer;
		auto size = sizeof(transactionId);
		writeBuffer.write(transactionId, size);

		writeBuffer.writeUint16(htons(attribute.type));
		writeBuffer.writeUint16(htons(attribute.valueLengthByte));
		writeBuffer.writeUint16(attribute.value.padding);
		writeBuffer.writeUint8(attribute.value.clazz);
		writeBuffer.writeUint8(attribute.value.errnum);

		return writeBuffer;
	}

};

