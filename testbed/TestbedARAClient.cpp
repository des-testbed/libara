/*
 * $FU-Copyright$
 */

#include "TestbedARAClient.h"


TESTBED_NAMESPACE_BEGIN

TestbedARAClient::TestbedARAClient(Configuration& configuration) : AbstractARAClient(configuration) {
    // set the clock to the standard clock (if it is not pre-set to the dummy clock, the tests fail)
    Environment::setClock(new StandardClock());
    //TODO Make configurable
    //Logger* logger = new SimpleLoggerExtended("ara");
    Logger* logger = new SimpleLogger("ara");
    setLogger(logger);
    logDebug("Initialized testbedARAClient");
    initializeNetworkInterfaces();
    logDebug("Initialized testbedARAClient network Interfaces");
}

TestbedARAClient::~TestbedARAClient() { }


std::string TestbedARAClient::toString() { 
    std::ostringstream result;
    result << std::endl;

    result << "initial pheromone value:                     " << initialPheromoneValue << std::endl;
    result << "maximum time to live:                        " << packetFactory->getMaximumNrOfHops() << std::endl;
//    result << "evporation policy:                           " << evaporationPolicy->toString() << std::endl;

    result << "max number of route discovery retries:       " << maxNrOfRouteDiscoveryRetries << std::endl;
    result << "packet delivery delay [ms]:                  " << packetDeliveryDelayInMilliSeconds << std::endl;
    result << "route discovery timeout [ms]:                " << routeDiscoveryTimeoutInMilliSeconds << std::endl;
    result << "neighbor activity check interval [ms]:       " << neighborActivityCheckIntervalInMilliSeconds << std::endl;
    result << "max neighbor inactivity check interval [ms]: " << maxNeighborInactivityTimeInMilliSeconds << std::endl;
    result << "pant interval [ms]:                          " << pantIntervalInMilliSeconds << std::endl;

    return result.str();
}

void TestbedARAClient::sendPacket(Packet* packet) {
    // DEBUG:
    std::cerr << "[TestbedARAClient::sendPacket] pass packet to client" << std::endl;
    AbstractARAClient::sendPacket(packet);
}

void TestbedARAClient::receivePacket(Packet* packet, ARA::NetworkInterface* interface) {
    logDebug("receiving packet # %u type %s over interface at %s", packet->getSequenceNumber(), PacketType::getAsString(packet->getType()).c_str(), interface->getLocalAddress()->toString().c_str());
    AbstractARAClient::receivePacket(packet, interface);
    //TODO: persistRoutingTableData
}

void TestbedARAClient::deliverToSystem(const Packet* packet) {
    logDebug("sending packet # %u to System via TAP", packet->getSequenceNumber());

    /// cast it to a testbed packet
    const TestbedPacket *testbedPacket = dynamic_cast<const TestbedPacket*>(packet);

    struct ether_header* payload = nullptr;
    size_t payloadLength = dessert_msg_ethdecap(testbedPacket->getMessage(), &payload);

    /// deliver the packet to the system
    if (dessert_syssend(payload, payloadLength) == DESSERT_OK){
        std::cerr << "[TestbedARAClient::deliverToSystem] sending packet to system was successful" << std::endl;
    } else {
        std::cerr << "[TestbedARAClient::deliverToSystem] sending packet to system failed" << std::endl;
    }

    /**
     * FIXME: we have to use free, since it was allocated using malloc in
     * dessert_msg_ethdecap
     */
    if (payload) {
       free(payload);
    }
}

void TestbedARAClient::packetNotDeliverable(const Packet* packet) {
    logDebug("packet # %u is undeliverable", packet->getSequenceNumber());
    delete packet;
}

void TestbedARAClient::initializeNetworkInterfaces() {
    dessert_meshif_t* dessertInterfaces = dessert_meshiflist_get();

    while(dessertInterfaces != nullptr) {
        TestbedNetworkInterface* newInterface = new TestbedNetworkInterface(dessertInterfaces, this, packetFactory, 400000);
        addNetworkInterface(newInterface);
        logDebug("initialized network interface: %s", dessertInterfaces->if_name);
        dessertInterfaces = dessertInterfaces->next;
    }

    tapAddress = std::make_shared<TestbedAddress>(dessert_l25_defsrc);
    std::cerr << "[initializeNetworkInterfaces] tap address is: " << tapAddress->toString() << std::endl;
}

bool TestbedARAClient::isLocalAddress(AddressPtr address) const {
    return (address.get()->equals(tapAddress) || AbstractNetworkClient::isLocalAddress(address));
}

void TestbedARAClient::broadcastFANT(AddressPtr destination) {
    unsigned int sequenceNr = getNextSequenceNumber();

    for(auto& interface: interfaces) {
        Packet* fant = packetFactory->makeFANT(tapAddress, destination, sequenceNr);
        interface->broadcast(fant);
    }
}

std::string TestbedARAClient::routingTableToString() {
    return this->routingTable->toString();
}

void TestbedARAClient::handleExpiredRouteDiscoveryTimer(std::weak_ptr<Timer> routeDiscoveryTimer){
    std::lock_guard<std::mutex> lock(routeDiscoveryTimerMutex);
    AbstractARAClient::handleExpiredRouteDiscoveryTimer(routeDiscoveryTimer);
}

void TestbedARAClient::handleExpiredDeliveryTimer(std::weak_ptr<Timer> deliveryTimer){
    std::lock_guard<std::mutex> lock(deliveryTimerMutex);
    std::shared_ptr<Timer> timer = deliveryTimer.lock();

    if (timer) {
        // DEBUG:
        std::cerr << "[TestbedARAClient::handleExpiredDeliveryTimer] "<< std::endl;
        TestbedTimerAddressInfo* timerInfo = (TestbedTimerAddressInfo*) timer->getContextObject();
        AddressPtr destination = timerInfo->getAddress();

        // DEBUG: std::cerr << "[TestbedARAClient::handleExpiredDeliveryTimer] use count is: " << destination.use_count() << std::endl;

        RunningRouteDiscoveriesMap::const_iterator discovery;
        // find the destination in the running route discoverys map
        std::lock_guard<std::mutex> routeDiscoveryTimerLock(routeDiscoveryTimerMutex);
        discovery = runningRouteDiscoveries.find(destination);


        if (discovery != runningRouteDiscoveries.end()) {
            // its important to delete the discovery info first or else the client will always think the route discovery is still running and never send any packets
            runningRouteDiscoveries.erase(discovery);
            runningDeliveryTimers.erase(timer);

            delete timerInfo;
            sendDeliverablePackets(destination);
        } else {
            logError("Could not find running route discovery object for destination %s)", destination->toString().c_str());
        }
    } else {
        // DEBUG: 
        std::cerr << "[TestbedARAClient::handleExpiredDeliveryTimer] shared_ptr expired " << std::endl;
    }

}

void TestbedARAClient::handleExpiredPANTTimer(std::weak_ptr<Timer> pantTimer){
    std::lock_guard<std::mutex> lock(pantTimerMutex);
    AbstractARAClient::handleExpiredPANTTimer(pantTimer);
}

void TestbedARAClient::stopRouteDiscoveryTimer(AddressPtr destination){
    std::lock_guard<std::mutex> lock(routeDiscoveryTimerMutex);
    AbstractARAClient::stopRouteDiscoveryTimer(destination);
}

TestbedNetworkInterface* TestbedARAClient::getTestbedNetworkInterface(std::shared_ptr<TestbedAddress> address){
    std::lock_guard<std::mutex> lock(networkInterfaceMutex);
    
    if (interfaces.size() == 1) {
        return dynamic_cast<TestbedNetworkInterface*>(interfaces[0]);
    }

    /** 
     * if we really use at some point multiple interfaces, we have to improve
     * the management of the interfaces
     */
    if (interfaces.size() > 1) {
        for (unsigned int i = 0; i < interfaces.size(); i++) {
            if (interfaces[i]->getLocalAddress() == address) {
                return dynamic_cast<TestbedNetworkInterface*>(interfaces[i]);
            }
        }
    }

    return nullptr;
}

void TestbedARAClient::startDeliveryTimer(TestbedAddressPtr destination) {
    TestbedTimerAddressInfo* contextObject = new TestbedTimerAddressInfo(destination);
    TimerPtr timer = getNewTimer(TimerType::DELIVERY_TIMER, contextObject);
    timer->addTimeoutListener(this);
    timer->run(packetDeliveryDelayInMilliSeconds * 1000);
    runningDeliveryTimers.insert(timer);
}

TESTBED_NAMESPACE_END
