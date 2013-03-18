#include "AbstractEARAClient.h"

namespace ARA {
    AbstractEARAClient::AbstractEARAClient(){

    }

    AbstractEARAClient::~AbstractEARAClient(){

    }

    void AbstractEARAClient::initialize(Configuration &configuration) {
        AbstractARAClient::initialize(configuration);
    }

    void AbstractEARAClient::setupRoutingTable() {
        this->routingTable = new EnergyAwareRoutingTable();
        this->routingTable->setEvaporationPolicy(evaporationPolicy);
    }

}
