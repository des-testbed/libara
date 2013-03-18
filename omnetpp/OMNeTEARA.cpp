/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEARA.h"

namespace ARA {
    namespace omnetpp {
        typedef std::shared_ptr<Address> AddressPtr;

        Define_Module(OMNeTEARA);

        OMNeTEARA::OMNeTEARA(){

        }

        OMNeTEARA::~OMNeTEARA(){

        }

        void OMNeTEARA::initialize(int stage) {
            if(stage == 4) {
                Configuration config = OMNeTConfiguration::parseFrom(this);
                AbstractEARAClient::initialize(config);

                setLogger(OMNeTConfiguration::getLogger(this));
//                OMNeTConfiguration::initializeNetworkInterfacesOf(this);

                routingTable = OMNeTConfiguration::getRoutingTableFrom(this);
                routingTable->setEvaporationPolicy(evaporationPolicy);
            }
        }

        void OMNeTEARA::handleMessage(cMessage *message) {

        }

        void OMNeTEARA::handleRouteFailure(const Packet* packet, AddressPtr nextHop, NetworkInterface* interface){

        }

        void OMNeTEARA::deliverToSystem(const Packet* packet){

        }

        void OMNeTEARA::packetNotDeliverable(const Packet* packet){

        }

    }
}

