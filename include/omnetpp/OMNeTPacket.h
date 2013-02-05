//
// Generated file, do not edit! Created by opp_msgc 4.2 from OMNeTPacket.msg.
//

#ifndef _OMNETPACKET_H_
#define _OMNETPACKET_H_

#include "Packet.h"
#include "Address.h"

#include <omnetpp.h>

namespace ARA {

class OMNeTPacket : public ::cPacket, public ARA::Packet {
  private:
    void copy(const OMNeTPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const OMNeTPacket&);

  public:
    OMNeTPacket(std::shared_ptr<Address> source=NULL, std::shared_ptr<Address> destination=NULL, std::shared_ptr<Address> sender=NULL, char type=0, unsigned int seqNr=0, const char* payload=NULL, unsigned int payloadSize=0, unsigned int hopCount = 0);
    OMNeTPacket(const OMNeTPacket& other);
    OMNeTPacket& operator=(const OMNeTPacket& other);
    virtual OMNeTPacket *dup() const {return new OMNeTPacket(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    virtual Packet* clone() const;
    virtual Packet* createFANT(unsigned int sequenceNumber) const;
    virtual Packet* createBANT(unsigned int sequenceNumber) const;
    virtual Packet* createDuplicateWarning() const;
};

inline void doPacking(cCommBuffer *b, OMNeTPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, OMNeTPacket& obj) {obj.parsimUnpack(b);}

} /* namespace ARA */
#endif // _OMNETPACKET_H_
