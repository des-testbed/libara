/*
 * $FU-Copyright$
 */

#include "EARAPacketFactory.h"

ARA_NAMESPACE_BEGIN

EARAPacket* EARAPacketFactory::makeClone(const Packet* originalPacket) {
    EARAPacket* clone = makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), originalPacket->getType(), originalPacket->getSequenceNumber(), originalPacket->getTTL(), originalPacket->getPayload(), originalPacket->getPayloadLength(), originalPacket->getPreviousHop());

    const EARAPacket* originalEARAPacket = dynamic_cast<const EARAPacket*>(originalPacket);
    if (originalEARAPacket != NULL) {
        clone->setTotalEnergyValue(originalEARAPacket->getTotalEnergyValue());
        clone->setMinimumEnergyValue(originalEARAPacket->getMinimumEnergyValue());
    }

    return clone;
}

EARAPacket* EARAPacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    EARAPacket* packet = new EARAPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    if(previousHop != nullptr) {
        packet->setPreviousHop(previousHop);
    }
    return packet;
}

ARA_NAMESPACE_END
