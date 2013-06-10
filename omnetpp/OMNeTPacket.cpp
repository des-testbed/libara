/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/OMNeTAddress.h"

#include <iostream>
#include <sstream>

namespace ARA {
namespace omnetpp {

typedef std::shared_ptr<Address> AddressPtr;

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

Register_Class(OMNeTPacket);

OMNeTPacket::OMNeTPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) : cPacket(PacketType::getAsString(type).c_str(), type), ARA::Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize) {

}

OMNeTPacket::OMNeTPacket(const OMNeTPacket& other) : cPacket(other), ARA::Packet(other.source, other.destination, other.sender, other.type, other.seqNr, other.ttl, other.payload, other.payloadSize) {
    this->previousHop = other.previousHop;
}

OMNeTPacket& OMNeTPacket::operator=(const OMNeTPacket& other) {
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void OMNeTPacket::copy(const OMNeTPacket& other) {
    this->source = other.source;
    this->destination = other.destination;
    this->sender = other.sender;
    this->previousHop = other.previousHop;
    this->type = other.type;
    this->seqNr = other.seqNr;
    this->payload = other.payload;
    this->payloadSize = other.payloadSize;
    this->ttl = other.ttl;
}

void OMNeTPacket::parsimPack(cCommBuffer *b) {
    throw cRuntimeError("Parsim error: OMNeTPacket::parsimPack is not yet implemented");
}

void OMNeTPacket::parsimUnpack(cCommBuffer *b) {
    throw cRuntimeError("Parsim error: OMNeTPacket::parsimUnpack is not yet implemented");
}

std::shared_ptr<OMNeTAddress> OMNeTPacket::getSource() const {
    return std::dynamic_pointer_cast<OMNeTAddress>(this->source);
}

std::shared_ptr<OMNeTAddress> OMNeTPacket::getDestination() const {
    return std::dynamic_pointer_cast<OMNeTAddress>(this->destination);
}

std::shared_ptr<OMNeTAddress> OMNeTPacket::getSender() const {
    return std::dynamic_pointer_cast<OMNeTAddress>(this->sender);
}

std::shared_ptr<OMNeTAddress> OMNeTPacket::getPreviousHop() const {
    return std::dynamic_pointer_cast<OMNeTAddress>(this->previousHop);
}

class OMNeTPacketDescriptor : public cClassDescriptor {
public:
    OMNeTPacketDescriptor();
    virtual ~OMNeTPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;

private:
    const int nrOfFields = 9;
};

Register_ClassDescriptor(OMNeTPacketDescriptor);

OMNeTPacketDescriptor::OMNeTPacketDescriptor() : cClassDescriptor("OMNeTPacket", "cPacket")
{
}

OMNeTPacketDescriptor::~OMNeTPacketDescriptor() {
}

bool OMNeTPacketDescriptor::doesSupport(cObject *obj) const {
    return dynamic_cast<OMNeTPacket *>(obj)!=NULL;
}

const char *OMNeTPacketDescriptor::getProperty(const char *propertyname) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int OMNeTPacketDescriptor::getFieldCount(void *object) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? nrOfFields+basedesc->getFieldCount(object) : nrOfFields;
}

unsigned int OMNeTPacketDescriptor::getFieldTypeFlags(void *object, int field) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object)) {
            return basedesc->getFieldTypeFlags(object, field);
        }
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND | FD_ISPOINTER,   // Address* source
        FD_ISCOMPOUND | FD_ISPOINTER,   // Address* destination
        FD_ISCOMPOUND | FD_ISPOINTER,   // Address* sender
        FD_ISCOMPOUND | FD_ISPOINTER,   // Address* prevHop
        FD_NONE,                        // char type;
        FD_NONE,                        // unsigned int seqNr;
        FD_NONE,                        // const char* payload;
        FD_NONE,                        // unsigned int payloadSize;
        FD_NONE                         // int TTL;
    };
    return (field>=0 && field<nrOfFields) ? fieldTypeFlags[field] : 0;
}

const char *OMNeTPacketDescriptor::getFieldName(void *object, int field) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object)) {
            return basedesc->getFieldName(object, field);
        }
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "source",
        "destination",
        "sender",
        "prevHop",
        "type",
        "seqNr",
        "payload",
        "payloadSize",
        "TTL"
    };
    return (field>=0 && field<nrOfFields) ? fieldNames[field] : NULL;
}

int OMNeTPacketDescriptor::findField(void *object, const char *fieldName) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sender")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "prevHop")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "seqNr")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "payload")==0) return base+6;
    if (fieldName[0]=='p' && strcmp(fieldName, "payloadSize")==0) return base+7;
    if (fieldName[0]=='T' && strcmp(fieldName, "TTL")==0) return base+8;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *OMNeTPacketDescriptor::getFieldTypeString(void *object, int field) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "Address*",
        "Address*",
        "Address*",
        "Address*",
        "PacketType",
        "unsigned int",
        "string",
        "unsigned int",
        "int"
    };
    return (field>=0 && field<nrOfFields) ? fieldTypeStrings[field] : NULL;
}

const char *OMNeTPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int OMNeTPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    OMNeTPacket *pp = (OMNeTPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string OMNeTPacketDescriptor::getFieldAsString(void *object, int field, int i) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    OMNeTPacket *pp = (OMNeTPacket *)object; (void)pp;

    if(field >= 0 && field < 4) {
        std::shared_ptr<OMNeTAddress> address;
        switch (field) {
            case 0: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getSource())); break;
            case 1: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getDestination())); break;
            case 2: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getSender())); break;
            case 3: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getPreviousHop())); break;
        }

        if(address == nullptr) {
            return "NULL (nullptr)";
        }
        else {
            return address->str();
        }
    }

    switch (field) {
        case 4: return PacketType::getAsString(pp->getType());
        case 5: return long2string(pp->getSequenceNumber());
        case 6: return pp->getPayload() == NULL ? "NULL" : pp->getPayload();
        case 7: return long2string(pp->getPayloadLength());
        case 8: return long2string(pp->getTTL());
        default: return "NOT IMPLEMENTED";
    }
}

bool OMNeTPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    OMNeTPacket *pp = (OMNeTPacket *)object; (void)pp;
    switch (field) {
        // nothing is editable yet
        default: return false;
    }
}

const char *OMNeTPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        "Address",
        "Address",
        "Address",
        "Address",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<nrOfFields) ? fieldStructNames[field] : NULL;
}

void *OMNeTPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    OMNeTPacket* pp = (OMNeTPacket *)object; (void)pp;
    switch (field) {
        case 0: return pp->getSource().get();
        case 1: return pp->getDestination().get();
        case 2: return pp->getSender().get();
        case 3: return pp->getPreviousHop().get();
        default: return NULL;
    }
}

} /* namespace omnetpp */
} /* namespace ARA */
