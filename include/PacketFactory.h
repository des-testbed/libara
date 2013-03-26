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
 * Packets encapsulate a payload that has to be transmitted from
 * a source node to a destination node.
 */
class PacketFactory {
public:
    virtual ~PacketFactory() {};

    /**
     * Creates a copy of the original packet.
     *
     * Note: The result of this method is a newly created object which must be
     * deleted later by the calling class.
     */
    Packet* makeClone(const Packet* originalPacket);

    /**
     * Creates a new FANT based on the given packet. The FANT inherits all the
     * addresses of this packet. The hop count is also replicated.
     *
     * Note: The result of this method is a newly created object which must be
     * deleted later by the calling class.
     */
    Packet* makeFANT(const Packet* originalPacket, unsigned int newSequenceNumber);

    /**
     * Creates a new BANT based on the given packet. This BANT has the destination of
     * this packet as its source and the destination of this as its source.
     * The hop count will be set to 0. The sequence number of the BANT is
     * given as argument of this method.
     *
     * Note: The result of this method is a newly created object which must be
     * deleted later by the calling class.
     */
     Packet* makeBANT(const Packet* originalPacket, unsigned int sequenceNumber);

     /**
      * Creates a new DUPLICATE_WARNING packet based on the information of the
      * given packet. The DUPLICATE_WARNING inherits all the addresses of this
      * packet. The hop count is incremented.
      *
      * Note: The result of this method is a newly created object which must be
      * deleted later by the calling class.
      */
     Packet* makeDulicateWarningPacket(const Packet* originalPacket);

     /**
      * Creates a new acknowledgment packet based on this packet. The source,
      * destination and sequence number will equal to this packet.
      *
      * Note: The result of this method is a newly created object which must be
      * deleted later by the calling class.
      */
     Packet* makeAcknowledgmentPacket(const Packet* originalPacket);

     /**
      * Creates a new route failure packet based on this packet. The source,
      * destination and sequence number will equal to this packet.
      *
      * Note: The result of this method is a newly created object which must be
      * deleted later by the calling class.
      */
     Packet* makeRouteFailurePacket(const Packet* originalPacket);

     /**
       * Creates a new energy dissemination packet with the given parameters.
       *
       * Note: The result of this method is a newly created object which must be
       * deleted later by the calling class.
       */
     Packet* makeEnergyDisseminationPacket(AddressPtr source, unsigned int seqNr, unsigned char energyLevel);

protected:
     /**
      * This method is responsible for creating the actual packet instance.
      * It can be overridden if another other Packet class shall be used.
      */
     virtual Packet* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, const char* payload=nullptr, unsigned int payloadSize=0, unsigned int hopCount = 0);
};

} /* namespace ARA */
#endif // PACKET_FACTORY_H_
