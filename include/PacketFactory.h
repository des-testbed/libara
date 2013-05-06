/*
 * $FU-Copyright$
 */

#ifndef PACKET_FACTORY_H_
#define PACKET_FACTORY_H_

#include "Address.h"
#include "Packet.h"

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

/**
 * The PacketFactory is responsible for creating the packet instances for an ~AbstractARAClient.
 * This class is necessary so we can decide at runtime what the concrete Packet class shall be.
 * If you do not want to use another Packet implementation class than the standard implementation
 * you need to override PacketFactory::makePacket(...) to return something that inherits from packet.
 */
class PacketFactory {
public:
    PacketFactory(int maxHopCount);
    virtual ~PacketFactory() {};

    /**
     * Creates a copy of the original packet.
     *
     * Note: The result of this method is a newly created object which must be
     * deleted later by the calling class.
     */
    Packet* makeClone(const Packet* originalPacket);

    /**
     * Creates a new FANT based on the given addresses and sequence number.
     *
     * Note: The result of this method is a newly created object which must be
     * deleted later by the calling class.
     */
    Packet* makeFANT(AddressPtr source, AddressPtr destination, unsigned int newSequenceNumber);

    /**
     * Creates a new BANT based on the given packet. This BANT has the destination of
     * this packet as its source and the destination of this as its source.
     * The sequence number of the BANT is given as argument of this method.
     *
     * Note: The result of this method is a newly created object which must be
     * deleted later by the calling class.
     */
     Packet* makeBANT(const Packet* originalPacket, unsigned int sequenceNumber);

     /**
      * Creates a new DUPLICATE_WARNING packet based on the information of the
      * given packet. The DUPLICATE_WARNING inherits all the addresses of this
      * packet. As this packet is only destined for the immediate neighbor, the
      * TTL is set to 1. The sequence number is given as argument of this method.
      *
      * Note: The result of this method is a newly created object which must be
      * deleted later by the calling class.
      */
     Packet* makeDulicateWarningPacket(const Packet* originalPacket, AddressPtr senderOfDuplicateWarning, unsigned int newSequenceNumber);

     /**
      * Creates a new acknowledgment packet based on this packet. The source,
      * destination and sequence number will equal to this packet. As this
      * packet is only destined for the immediate neighbor, the TTL is set to 1.
      *
      * Note: The result of this method is a newly created object which must be
      * deleted later by the calling class.
      */
     Packet* makeAcknowledgmentPacket(const Packet* originalPacket);

     /**
      * Creates a new route failure packet based on the given addresses and sequence number.
      * As this packet is only destined for the immediate neighbor, the TTL is set
      * to 1.
      *
      * Note: The result of this method is a newly created object which must be
      * deleted later by the calling class.
      */
     Packet* makeRouteFailurePacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber);

     /**
       * Creates a new energy dissemination packet with the given parameters.
       * As this packet is only destined for the immediate neighbor, the TTL
       * is set to 1.
       *
       * Note: The result of this method is a newly created object which must be
       * deleted later by the calling class.
       */
     Packet* makeEnergyDisseminationPacket(AddressPtr source, unsigned int seqNr, unsigned char energyLevel);


     void setMaxHopCount(int n);
     int getMaximumNrOfHops();
protected:
     /**
      * This method is responsible for creating the actual packet instance.
      * It can be overridden if another other Packet class shall be used.
      */
     virtual Packet* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr);

     int maxHopCount;
};

} /* namespace ARA */
#endif // PACKET_FACTORY_H_
