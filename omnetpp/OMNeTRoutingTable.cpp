#include "omnetpp/OMNeTRoutingTable.h"
#include "omnetpp/OMNeTClock.h"
#include "omnetpp/UnorderedMapWatcher.h"

namespace ARA {
    namespace omnetpp {
        Define_Module(OMNeTRoutingTable);

        void OMNeTRoutingTable::initialize(){
            WATCH_UNORDEREDMAP(table);
		}

        void OMNeTRoutingTable::handleMessage(cMessage *msg){
            throw cRuntimeError("OMNeTRoutingTable: handleMessage() should never be called!");
		}

        /**
         * The class implements a class descriptor for the OMNeTRoutingTable 
         * class. The class provides a description of members of the RoutingTable
         * which in turn is used by TKenv to display the content in the 
         * simulators graphical user interface.
         */
        class OMNeTRoutingTableDescriptor : public cClassDescriptor {
            public:
                OMNeTRoutingTableDescriptor();
                virtual ~OMNeTRoutingTableDescriptor();

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
                const int nrOfFields = 4;
        };

        Register_ClassDescriptor(OMNeTRoutingTableDescriptor);

        OMNeTRoutingTableDescriptor::OMNeTRoutingTableDescriptor() : cClassDescriptor("OMNeTRoutingTable", "cObject"){}

        OMNeTRoutingTableDescriptor::~OMNeTRoutingTableDescriptor(){ }
        
        bool OMNeTRoutingTableDescriptor::doesSupport(cObject *object) const{
            return dynamic_cast<OMNeTRoutingTable *>(object) != NULL;
        }

        const char *OMNeTRoutingTableDescriptor::getProperty(const char *propertyName) const{
            cClassDescriptor *basedesc = getBaseClassDescriptor();
            return basedesc ? basedesc->getProperty(propertyName) : NULL;
        }

        int OMNeTRoutingTableDescriptor::getFieldCount(void *object) const {
            cClassDescriptor *basedesc = getBaseClassDescriptor();
            return basedesc ? nrOfFields+basedesc->getFieldCount(object) : nrOfFields;
        }

        unsigned int OMNeTRoutingTableDescriptor::getFieldTypeFlags(void *object, int field) const {
            cClassDescriptor *basedesc = getBaseClassDescriptor();
            if(basedesc){
                if(field < basedesc->getFieldCount(object)) {
                    return basedesc->getFieldTypeFlags(object, field);
                }
                field -= basedesc->getFieldCount(object);
            }
            static unsigned int fieldTypeFlags[] = {
                FD_NONE,                        // char type;
                FD_NONE,                        // char type;
                FD_NONE,                        // char type;
                FD_NONE                         // char type;
            };
            return (field>=0 && field<nrOfFields) ? fieldTypeFlags[field] : 0;
        }

        const char *OMNeTRoutingTableDescriptor::getFieldName(void *object, int field) const {
            cClassDescriptor *basedesc = getBaseClassDescriptor();
            if(basedesc){
                if(field < basedesc->getFieldCount(object)) {
                    return basedesc->getFieldName(object, field);
                }

                field -= basedesc->getFieldCount(object);
            }

            static const char *fieldNames[] = {
                "source",
                "destination",
                "next hop",
                "phi",
            };

            return (field>=0 && field<nrOfFields) ? fieldNames[field] : NULL;
        }


        int OMNeTRoutingTableDescriptor::findField(void *object, const char *fieldName) const {
            cClassDescriptor *basedesc = getBaseClassDescriptor();
            int base = basedesc ? basedesc->getFieldCount(object) : 0;

            if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
            if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
            if (fieldName[0]=='n' && strcmp(fieldName, "next hop")==0) return base+2;
            if (fieldName[0]=='p' && strcmp(fieldName, "phi")==0) return base+3;

            return basedesc ? basedesc->findField(object, fieldName) : -1;
        }

	    const char *OMNeTRoutingTableDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const {
	        cClassDescriptor *basedesc = getBaseClassDescriptor();

            if(basedesc){
		        if(field < basedesc->getFieldCount(object)){
		            return basedesc->getFieldProperty(object, field, propertyname);
                }
                field -= basedesc->getFieldCount(object);
            }
           
            return NULL;
        }

	    int OMNeTRoutingTableDescriptor::getArraySize(void *object, int field) const{
	        cClassDescriptor *basedesc = getBaseClassDescriptor();

            if(basedesc){
		        if(field < basedesc->getFieldCount(object)){
                    return basedesc->getArraySize(object, field);
                }
                field -= basedesc->getFieldCount(object);
            }

            OMNeTRoutingTable *routingTable = (OMNeTRoutingTable *)object; (void)routingTable;

            return 0;
        }

        /// FIXME
        std::string OMNeTRoutingTableDescriptor::getFieldAsString(void *object, int field, int i) const{
            cClassDescriptor *basedesc = getBaseClassDescriptor();
            if(basedesc){
                if(field < basedesc->getFieldCount(object)){
                    return basedesc->getFieldAsString(object,field,i);
                }
                field -= basedesc->getFieldCount(object);
            } 
            /// FIXME
            OMNeTRoutingTable *routingTable = (OMNeTRoutingTable *)object; 
            (void)routingTable;

            /// FIXME
            if(field >= 0 && field < 3) {
/*
		std::shared_ptr<OMNeTAddress> address;
		switch (field) {A
		    case 0: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getSource())); break;
		    case 1: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getDestination())); break;
		    case 2: address = (std::dynamic_pointer_cast<OMNeTAddress>(pp->getSender())); break;
		}
		if(address == NULL) {
		    return "NULL";
		}
		else {
		    return address->str();
		}
*/
	    }

            switch (field) {
/*
		case 3: return PacketType::getAsString(pp->getType());
		case 4: return long2string(pp->getSequenceNumber());
		case 5: return pp->getPayload() == NULL ? "NULL" : pp->getPayload();
		case 6: return long2string(pp->getPayloadLength());
		case 7: return long2string(pp->getHopCount());
*/
                default: return "NOT IMPLEMENTED";
	        }
        }

 	    bool OMNeTRoutingTableDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const{
	        cClassDescriptor *basedesc = getBaseClassDescriptor();

	        if(basedesc){
		        if(field < basedesc->getFieldCount(object)){
		            return basedesc->setFieldAsString(object,field,i,value);
                }
		        field -= basedesc->getFieldCount(object);
	        }

            return false;
        }

	    const char *OMNeTRoutingTableDescriptor::getFieldStructName(void *object, int field) const{
	        cClassDescriptor *basedesc = getBaseClassDescriptor();

	        if(basedesc){
		        if(field < basedesc->getFieldCount(object)){
		            return basedesc->getFieldStructName(object, field);
                }
		        field -= basedesc->getFieldCount(object);
	        }

            /// FIXME
           static const char *fieldStructNames[] = {
               NULL,
               NULL,
               NULL,
               NULL
           };

           return (field>=0 && field<nrOfFields) ? fieldStructNames[field] : NULL;
        }

        /// FIXME
        void *OMNeTRoutingTableDescriptor::getFieldStructPointer(void *object, int field, int i) const{
	        cClassDescriptor *basedesc = getBaseClassDescriptor();
	        if(basedesc){
		        if(field < basedesc->getFieldCount(object)){
		            return basedesc->getFieldStructPointer(object, field, i);
                }
		        field -= basedesc->getFieldCount(object);
	        }

	        OMNeTRoutingTable* pp = (OMNeTRoutingTable *)object; 
            /// What's the purpose of that?
            (void)pp;

	        switch(field){
/*
		        case 0: return 0;
		        case 1: return 1;
		        case 2: return 2;
*/
		        default: return NULL;
	        }
	    }

        const char *OMNeTRoutingTableDescriptor::getFieldTypeString(void *object, int field) const {
            cClassDescriptor *basedesc = getBaseClassDescriptor();
            if(basedesc){
                if(field < basedesc->getFieldCount(object)){
                    return basedesc->getFieldTypeString(object, field);
                }
                field -= basedesc->getFieldCount(object);
            }

            static const char *fieldTypeStrings[] = {
                "int",
                "int",
                "int",
                "int"
            };
            return (field>=0 && field<nrOfFields) ? fieldTypeStrings[field] : NULL;
        }
    }
}
