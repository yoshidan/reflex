#ifndef REFLEX_STUN_REPOSITORY_H
#define REFLEX_STUN_REPOSITORY_H
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING

#include "../domain/StunHeader.h"
#include "../udpserver.h"


namespace repository {

	class StunRepository {

	public:

		domain::StunHeader read(udpserver::WebRTCStunBufferReader& reader) const;

		udpserver::Writer<32> write(domain::StunHeader& responseHeader, const domain::XorMappedAddressAttribute<domain::IpV4MappedAddressAttributeValue>& attribute) const;

		udpserver::Writer<44> write(domain::StunHeader& responseHeader, const domain::XorMappedAddressAttribute<domain::IpV6MappedAddressAttributeValue>& attribute) const;

		udpserver::Writer<32> error(domain::StunHeader& responseHeader, const domain::ErrorCodeAttribute& attribute) const;
	};
}
#endif
