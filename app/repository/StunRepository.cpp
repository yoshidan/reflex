
#include "../repository/StunRepository.h"

namespace repository {

	template<size_t Size>
	void writeHeader(udpserver::Writer<Size>& writeBuffer, domain::StunHeader &responseHeader) {
		writeBuffer.writeUint16(htons(responseHeader.messageType));
		writeBuffer.writeUint16(htons(responseHeader.bodyLengthByte));
		writeBuffer.writeUint32(htonl(responseHeader.magicCookie));
		auto size = sizeof(responseHeader.transactionId);
		writeBuffer.write(responseHeader.transactionId, size);
	}

	domain::StunHeader StunRepository::read(udpserver::WebRTCStunBufferReader &reader) const {

		domain::StunHeader request{};
		size_t size = sizeof(request);
		reader.read(&request, size);

		request.messageType = ntohs(request.messageType);
		request.bodyLengthByte = ntohs(request.bodyLengthByte);
		request.magicCookie = ntohl(request.magicCookie);

		return request;
	}

	udpserver::Writer<32> StunRepository::write(domain::StunHeader &responseHeader,
												const domain::XorMappedAddressAttribute<domain::IpV4MappedAddressAttributeValue> &attribute) const {

		udpserver::Writer<32> writeBuffer;
		writeHeader(writeBuffer, responseHeader);

		writeBuffer.writeUint16(htons(attribute.type));
		writeBuffer.writeUint16(htons(attribute.valueLengthByte));
		writeBuffer.writeUint8(attribute.value.offset);
		writeBuffer.writeUint8(attribute.value.family);
		writeBuffer.writeUint16(htons(attribute.value.port));
		writeBuffer.writeUint32(htonl(attribute.value.address));

		return writeBuffer;
	}

	udpserver::Writer<44> StunRepository::write(domain::StunHeader &responseHeader,
												const domain::XorMappedAddressAttribute<domain::IpV6MappedAddressAttributeValue> &attribute) const {

		udpserver::Writer<44> writeBuffer;
		writeHeader(writeBuffer, responseHeader);

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

	udpserver::Writer<32>
	StunRepository::error(domain::StunHeader &responseHeader, const domain::ErrorCodeAttribute &attribute) const {

		udpserver::Writer<32> writeBuffer;
		writeBuffer.write(responseHeader.transactionId, 12);

		writeBuffer.writeUint16(htons(attribute.type));
		writeBuffer.writeUint16(htons(attribute.valueLengthByte));
		writeBuffer.writeUint16(attribute.value.padding);
		writeBuffer.writeUint8(attribute.value.clazz);
		writeBuffer.writeUint8(attribute.value.errnum);

		return writeBuffer;
	}
};
