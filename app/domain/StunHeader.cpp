
#include "StunHeader.h"

namespace domain {

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

};

