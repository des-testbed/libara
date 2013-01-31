#include <iostream>
#include <sstream>
#include "OMNeTPacket.h"
#include "OMNeTAddress.h"

using namespace ARA;

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

OMNeTPacket::OMNeTPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) : cPacket(PacketType::getAsString(type).c_str(), type), ARA::Packet(source, destination, sender, type, seqNr, payload, payloadSize, hopCount) {

}

OMNeTPacket::OMNeTPacket(const OMNeTPacket& other) : cPacket(other), ARA::Packet(other.source, other.destination, other.sender, other.type, other.seqNr, other.payload, other.payloadSize, other.hopCount) {

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
    this->type = other.type;
    this->seqNr = other.seqNr;
    this->payload = other.payload;
    this->payloadSize = other.payloadSize;
    this->hopCount = other.hopCount;
}

void OMNeTPacket::parsimPack(cCommBuffer *b) {
    throw cRuntimeError("Parsim error: OMNeTPacket::parsimPack is not yet implemented");
}

void OMNeTPacket::parsimUnpack(cCommBuffer *b) {
    throw cRuntimeError("Parsim error: OMNeTPacket::parsimUnpack is not yet implemented");
}

Packet* OMNeTPacket::clone() const {
    return new OMNeTPacket(source, destination, sender, type, seqNr, payload, payloadSize, hopCount);
}

Packet* OMNeTPacket::createFANT(unsigned int sequenceNumber) const {
    return new OMNeTPacket(source, destination, sender, PacketType::FANT, sequenceNumber);
}

Packet* OMNeTPacket::createBANT(unsigned int sequenceNumber) const {
    return new OMNeTPacket(destination, source, sender, PacketType::BANT, sequenceNumber);
}

Packet* OMNeTPacket::createDuplicateWarning() const {
    return new OMNeTPacket(source, destination, sender, PacketType::DUPLICATE_ERROR, seqNr, NULL, 0, hopCount+1);
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
    const int nrOfFields = 8;
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
        FD_NONE,                        // char type;
        FD_NONE,                        // unsigned int seqNr;
        FD_NONE,                        // const char* payload;
        FD_NONE,                        // unsigned int payloadSize;
        FD_NONE                         // unsigned int hopCount;
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
        "type",
        "seqNr",
        "payload",
        "payloadSize",
        "hopCount"
    };
    return (field>=0 && field<nrOfFields) ? fieldNames[field] : NULL;
}

int OMNeTPacketDescriptor::findField(void *object, const char *fieldName) const {
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sender")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+3;
    if (fieldName[0]=='s' && strcmp(fieldName, "seqNr")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "payload")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "payloadSize")==0) return base+6;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCount")==0) return base+7;
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
        "PacketType",
        "unsigned int",
        "string",
        "unsigned int",
        "unsigned int"
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

    if(field >= 0 && field < 3) {
        std::shared_ptr<OMNeTAddress> address;
        switch (field) {
            case 0: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getSource())); break;
            case 1: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getDestination())); break;
            case 2: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getSender())); break;
        }

        if(address == NULL) {
            return "NULL";
        }
        else {
            return address->getAddress().str();
        }
    }

    switch (field) {
        case 3: return PacketType::getAsString(pp->getType());
        case 4: return long2string(pp->getSequenceNumber());
        case 5: return pp->getPayload() == NULL ? "NULL" : pp->getPayload();
        case 6: return long2string(pp->getPayloadLength());
        case 7: return long2string(pp->getHopCount());
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
        default: return NULL;
    }
}


