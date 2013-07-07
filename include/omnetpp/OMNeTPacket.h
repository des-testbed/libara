/*
 * $FU-Copyright$
 */

#ifndef OMNET_PACKET_H_
#define OMNET_PACKET_H_

#include "OMNeTARAMacros.h"
#include "Packet.h"
#include "Address.h"
#include "OMNeTAddress.h"
#include "IPv4Address.h"

OMNETARA_NAMESPACE_BEGIN

class OMNeTPacket : public cPacket, public Packet {
  private:
    void copy(const OMNeTPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const OMNeTPacket&);

  public:
    OMNeTPacket(AddressPtr source=nullptr, AddressPtr destination=nullptr, AddressPtr sender=nullptr, char type=0, unsigned int seqNr=0, int ttl=50, const char* payload=nullptr, unsigned int payloadSize=0);
    OMNeTPacket(const OMNeTPacket& other);
    OMNeTPacket& operator=(const OMNeTPacket& other);
    virtual OMNeTPacket *dup() const {return new OMNeTPacket(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    OMNeTAddressPtr getSource() const;
    OMNeTAddressPtr getDestination() const;
    OMNeTAddressPtr getSender() const;
    OMNeTAddressPtr getPreviousHop() const;

    IPv4Address getSourceIP() const { return *(getSource().get()); }
    IPv4Address getDestinationIP() const { return *(getDestination().get()); }
    IPv4Address getSenderIP() const { return *(getSender().get()); }
};

inline void doPacking(cCommBuffer *b, OMNeTPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, OMNeTPacket& obj) {obj.parsimUnpack(b);}

OMNETARA_NAMESPACE_END

#endif // OMNET_PACKET_H_
