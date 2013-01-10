#include <iostream>
#include <sstream>
#include "OMNeTPacket.h"
#include "OMNeTAddress.h"

using namespace ARA;

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

OMNeTPacket::OMNeTPacket(Address* source, Address* destination, Address* sender, char type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) : ARA::Packet(source, destination, sender, type, seqNr, payload, payloadSize, hopCount) {

}

OMNeTPacket::OMNeTPacket(const OMNeTPacket& other) : ARA::Packet(other.source, other.destination, other.sender, other.type, other.seqNr, other.payload, other.payloadSize, other.hopCount) {
    copy(other);
}

OMNeTPacket& OMNeTPacket::operator=(const OMNeTPacket& other) {
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void OMNeTPacket::copy(const OMNeTPacket& other) {
    //TODO implement OMNeTPacket::copy
}

void OMNeTPacket::parsimPack(cCommBuffer *b) {
    //TODO implement OMNeTPacket::parsimPack
    cPacket::parsimPack(b);
    /*doPacking(b,this->someField_var);
    doPacking(b,this->anotherField_var);
    b->pack(arrayField1_arraysize);
    doPacking(b,this->arrayField1_var,arrayField1_arraysize);
    doPacking(b,this->arrayField2_var,10);*/
}

void OMNeTPacket::parsimUnpack(cCommBuffer *b) {
    //TODO implement OMNeTPacket::parsimUnpack
    cPacket::parsimUnpack(b);
    /*doUnpacking(b,this->someField_var);
    doUnpacking(b,this->anotherField_var);
    delete [] this->arrayField1_var;
    b->unpack(arrayField1_arraysize);
    if (arrayField1_arraysize==0) {
        this->arrayField1_var = 0;
    } else {
        this->arrayField1_var = new double[arrayField1_arraysize];
        doUnpacking(b,this->arrayField1_var,arrayField1_arraysize);
    }
    doUnpacking(b,this->arrayField2_var,10);*/
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
        FD_ISEDITABLE,                  // char type;
        FD_ISEDITABLE,                  // unsigned int seqNr;
        FD_ISEDITABLE,                  // const char* payload;
        FD_ISEDITABLE,                  // unsigned int payloadSize;
        FD_ISEDITABLE                   // unsigned int hopCount;
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
        OMNeTAddress* address;
        switch (field) {
            case 0: address = (OMNeTAddress*) pp->getSource(); break;
            case 1: address = (OMNeTAddress*) pp->getDestination(); break;
            case 2: address = (OMNeTAddress*) pp->getSender(); break;
        }

        if(address == NULL) {
            return "NULL";
        }
        else {
            return address->getAddress();
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
        //TODO implement this
        /*case 0: pp->setSomeField(string2long(value)); return true;
        case 1: pp->setAnotherField((value)); return true;
        case 2: pp->setArrayField1(i,string2double(value)); return true;
        case 3: pp->setArrayField2(i,string2double(value)); return true;*/
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
        //TODO imlement this
        /*case 0: return pp->getSource();
        case 1: return pp->getDestination();
        case 2: return pp->getSender();*/
        default: return NULL;
    }
}


