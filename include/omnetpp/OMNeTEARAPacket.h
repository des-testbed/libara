/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_PACKET_H_
#define OMNET_EARA_PACKET_H_

#include "OMNeTARAMacros.h"
#include "EARAPacket.h"
#include "Address.h"
#include "OMNeTAddress.h"
#include "IPv4Address.h"

OMNETARA_NAMESPACE_BEGIN

class OMNeTEARAPacket : public cPacket, public EARAPacket {
  private:
    void copy(const OMNeTEARAPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const OMNeTEARAPacket&);

  public:
    OMNeTEARAPacket(AddressPtr source=nullptr, AddressPtr destination=nullptr, AddressPtr sender=nullptr, char type=0, unsigned int seqNr=0, int ttl=50, const char* payload=nullptr, unsigned int payloadSize=0);
    OMNeTEARAPacket(const OMNeTEARAPacket& other);
    OMNeTEARAPacket& operator=(const OMNeTEARAPacket& other);
    virtual OMNeTEARAPacket *dup() const {return new OMNeTEARAPacket(*this);}
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

inline void doPacking(cCommBuffer *b, OMNeTEARAPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, OMNeTEARAPacket& obj) {obj.parsimUnpack(b);}

OMNETARA_NAMESPACE_END

#endif // OMNET_EARA_PACKET_H_
