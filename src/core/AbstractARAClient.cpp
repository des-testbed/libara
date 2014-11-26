/*
 * $FU-Copyright$
 */

#include "AbstractARAClient.h"
#include "PacketType.h"
#include "Timer.h"
#include "Environment.h"
#include "Exception.h"
#include "TimerType.h"
#include "TimerAddressInfo.h"

#include <system_error>

#include "StandardTimerProxy.h"

using namespace std;

ARA_NAMESPACE_BEGIN

AbstractARAClient::AbstractARAClient(Configuration& configuration) {
    initialize(configuration);
}

void AbstractARAClient::initialize(Configuration& configuration) {
    onlyOneRouteFailure = allRoutesHaveCollapsedFailure = 0;
    routingLoopFailure = 0;

    routingTable = configuration.getRoutingTable();
    packetFactory = configuration.getPacketFactory();
    packetFactory->setPreviousHopFeature(isPreviousHopFeatureActivated);
    forwardingPolicy = configuration.getForwardingPolicy();
    pathReinforcementPolicy = configuration.getReinforcementPolicy();
    evaporationPolicy = configuration.getEvaporationPolicy();
    initialPheromoneValue = configuration.getInitialPheromoneValue();
    maxNrOfRouteDiscoveryRetries = configuration.getMaxNrOfRouteDiscoveryRetries();
    packetDeliveryDelayInMilliSeconds = configuration.getPacketDeliveryDelayInMilliSeconds();
    routeDiscoveryTimeoutInMilliSeconds = configuration.getRouteDiscoveryTimeoutInMilliSeconds();
    neighborActivityCheckIntervalInMilliSeconds = configuration.getNeighborActivityCheckIntervalInMilliSeconds();
    maxNeighborInactivityTimeInMilliSeconds = configuration.getMaxNeighborInactivityTimeInMilliSeconds();
    pantIntervalInMilliSeconds = configuration.getPANTIntervalInMilliSeconds();
    isPreviousHopFeatureActivated = configuration.isPreviousHopFeatureActivated();
    packetTrap = configuration.getPacketTrap();

    if (neighborActivityCheckIntervalInMilliSeconds > 0) {
       neighborActivityTimer = getNewTimer(TimerType::NEIGHBOR_ACTIVITY_TIMER);
       neighborActivityTimer->addTimeoutListener(this);
       startNeighborActivityTimer();
    }
}

AbstractARAClient::~AbstractARAClient() {
    // delete the sequence number lists of the last received packets
    for (LastReceivedPacketsMap::iterator iterator=lastReceivedPackets.begin(); iterator!=lastReceivedPackets.end(); iterator++) {
        // the addresses are disposed of automatically by shared_ptr
        delete iterator->second;
    }
    lastReceivedPackets.clear();

    // delete the running discovery timers and their context objects
    for (RunningRouteDiscoveriesMap::iterator iterator=runningRouteDiscoveries.begin(); iterator!=runningRouteDiscoveries.end(); iterator++) {
        TimerPtr timer = iterator->second;
        delete (RouteDiscoveryInfo*) timer->getContextObject();
    }
    runningRouteDiscoveries.clear();

    // delete the known intermediate hop addresses for all sources
    for (KnownIntermediateHopsMap::iterator iterator=knownIntermediateHops.begin(); iterator!=knownIntermediateHops.end(); iterator++) {
        delete iterator->second;
    }
    knownIntermediateHops.clear();

    // delete running delivery timers
    for (DeliveryTimerSet::iterator iterator=runningDeliveryTimers.begin(); iterator!=runningDeliveryTimers.end(); iterator++) {
        TimerPtr timer = *iterator;
        delete (TimerAddressInfo*) timer->getContextObject();
    }
    runningDeliveryTimers.clear();

    // delete the last activity timers of all currently known neighbors
    for (NeighborActivityMap::iterator iterator=neighborActivityTimes.begin(); iterator!=neighborActivityTimes.end(); iterator++) {
        delete iterator->second.first;
    }
    neighborActivityTimes.clear();

    // delete all running pant timers
    for (ScheduledPANTsMap::iterator iterator=scheduledPANTs.begin(); iterator!=scheduledPANTs.end(); iterator++) {
        TimerPtr timer = iterator->second;
        delete (TimerAddressInfo*) timer->getContextObject();
    }
    scheduledPANTs.clear();

    /* The following members may have be deleted earlier, depending on the destructor of the implementing class */
    DELETE_IF_NOT_NULL(pathReinforcementPolicy);
    DELETE_IF_NOT_NULL(evaporationPolicy);
    DELETE_IF_NOT_NULL(forwardingPolicy);
}

void AbstractARAClient::startNeighborActivityTimer() {
    neighborActivityTimer->run(neighborActivityCheckIntervalInMilliSeconds * 1000);
}

void AbstractARAClient::sendPacket(Packet* packet) {
    // DEBUG: std::cerr << "[AbstractARAClient::sendPacket] got packet" << std::endl;
    // at first we need to trigger the evaporation (this has no effect if this has been done before in receivePacket(..) )
    routingTable->triggerEvaporation();

    if (packet->getTTL() > 0) {
        AddressPtr destination = packet->getDestination();

        if (isRouteDiscoveryRunning(destination)) {
            logDebug("Route discovery for %s is already running. Trapping packet %u", destination->toString().c_str(), packet->getSequenceNumber());
            packetTrap->trapPacket(packet);
        } else if (routingTable->isDeliverable(packet)) {
            NextHop* nextHop = forwardingPolicy->getNextHop(packet);
            NetworkInterface* interface = nextHop->getInterface();
            AddressPtr nextHopAddress = nextHop->getAddress();
            packet->setPreviousHop(packet->getSender());
            packet->setSender(interface->getLocalAddress());

            float newPheromoneValue = reinforcePheromoneValue(destination, nextHopAddress, interface);
            logDebug("Forwarding DATA packet %u from %s to %s via %s (phi=%.2f)", packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getDestinationString().c_str(), nextHopAddress->toString().c_str(), newPheromoneValue);

            sendUnicast(packet, interface, nextHopAddress);
        } else {
            // DEBUG:
            std::cerr << "[AbstractARAClient::sendPacket]: packet source address is: " << packet->getSource()->toString() << std::endl;

            // packet is not deliverable and no route discovery is yet running
            if(isLocalAddress(packet->getSource())) {
                logDebug("Packet %u from %s to %s is not deliverable. Starting route discovery phase", packet->getSequenceNumber(), packet->getSourceString().c_str(), destination->toString().c_str());
                packetTrap->trapPacket(packet);
                startNewRouteDiscovery(packet);
            } else {
                handleNonSourceRouteDiscovery(packet);
            }
        }
    } else {
        handlePacketWithZeroTTL(packet);
    }
}

void AbstractARAClient::sendUnicast(Packet* packet, NetworkInterface* interface, AddressPtr receiver) {
    interface->send(packet, receiver);
    registerActivity(receiver, interface);
}

float AbstractARAClient::reinforcePheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    float currentPheromoneValue = routingTable->getPheromoneValue(destination, nextHop, interface);
    float newPheromoneValue = pathReinforcementPolicy->calculateReinforcedValue(currentPheromoneValue);
    routingTable->update(destination, nextHop, interface, newPheromoneValue);
    return newPheromoneValue;
}

void AbstractARAClient::startNewRouteDiscovery(Packet* packet) {
    // DEBUG: 
    std::cerr << "[AbstractARAClient::startNewRouteDiscovery] start new route discovery" << std::endl;
    AddressPtr destination = packet->getDestination();
    forgetKnownIntermediateHopsFor(destination);
    startRouteDiscoveryTimer(packet);
    broadcastFANT(destination);
}

void AbstractARAClient::forgetKnownIntermediateHopsFor(AddressPtr destination) {
    if(knownIntermediateHops.find(destination) != knownIntermediateHops.end()) {
        std::unordered_set<AddressPtr>* seenNodesForThisDestination = knownIntermediateHops[destination];
        seenNodesForThisDestination->clear();
    }
}

void AbstractARAClient::broadcastFANT(AddressPtr destination) {
    unsigned int sequenceNr = getNextSequenceNumber();
    // DEBUG: std::cerr << "[AbstractARAClient::broadcastFANT] get new sequence number " <<  sequenceNr << std::endl;

    for(auto& interface: interfaces) {
        Packet* fant = packetFactory->makeFANT(interface->getLocalAddress(), destination, sequenceNr);
        // DEBUG: std::cerr << "[AbstractARAClient::broadcastFANT] broadcast fant" << std::endl;
        interface->broadcast(fant);
    }
}

void AbstractARAClient::startRouteDiscoveryTimer(const Packet* packet) {
    // DEBUG: std::cerr << "[AbstractARAClient::startRouteDiscoveryTimer] create new route discovery context object " << std::endl;
    RouteDiscoveryInfo* discoveryInfo = new RouteDiscoveryInfo(packet);
    // DEBUG: std::cerr << "[AbstractARAClient::startRouteDiscoveryTimer] get new timer " << std::endl;
    TimerPtr timer = getNewTimer(TimerType::ROUTE_DISCOVERY_TIMER, discoveryInfo);
    // DEBUG: std::cerr << "[AbstractARAClient::startRouteDiscoveryTimer] add client to listener " << std::endl;
    timer->addTimeoutListener(this);
    // DEBUG: std::cerr << "[AbstractARAClient::startRouteDiscoveryTimer] run timer " << std::endl;
    timer->run(routeDiscoveryTimeoutInMilliSeconds * 1001);

    AddressPtr destination = packet->getDestination();
    runningRouteDiscoveries[destination] = timer;
}

bool AbstractARAClient::isRouteDiscoveryRunning(AddressPtr destination) {
    return runningRouteDiscoveries.find(destination) != runningRouteDiscoveries.end();
}

void AbstractARAClient::handleNonSourceRouteDiscovery(Packet* packet) {
    logWarn("Dropping packet %u from %s because no route is known (non-source RD disabled)", packet->getSequenceNumber(), packet->getSourceString().c_str());
    broadcastRouteFailure(packet->getDestination());
    delete packet;
}

void AbstractARAClient::handlePacketWithZeroTTL(Packet* packet) {
    logWarn("Dropping packet %u from %s because TTL reached zero", packet->getSequenceNumber(), packet->getSourceString().c_str());
    delete packet;
}

void AbstractARAClient::receivePacket(Packet* packet, NetworkInterface* interface) {
    updateRoutingTable(packet, interface);
    packet->decreaseTTL();

    if (hasBeenReceivedEarlier(packet)) {
        handleDuplicatePacket(packet, interface);
    } else {
        registerReceivedPacket(packet);
        handlePacket(packet, interface);
    }
}

void AbstractARAClient::handleDuplicatePacket(Packet* packet, NetworkInterface* interface) {
    if(packet->isDataPacket()) {
        sendDuplicateWarning(packet, interface);
    } else if(packet->getType() == PacketType::BANT && isDirectedToThisNode(packet)) {
        logDebug("Another BANT %u came back from %s via %s.", packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getSenderString().c_str());
    }

    delete packet;
}

void AbstractARAClient::sendDuplicateWarning(Packet* packet, NetworkInterface* interface) {
    routingLoopFailure++;
    logWarn("Routing loop for packet %u from %s detected. Sending duplicate warning back to %s", packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getSenderString().c_str());
    AddressPtr localhost = interface->getLocalAddress();
    Packet* duplicateWarningPacket = packetFactory->makeDuplicateWarningPacket(packet, localhost, getNextSequenceNumber());
    sendUnicast(duplicateWarningPacket, interface, packet->getSender());
}

void AbstractARAClient::updateRoutingTable(Packet* packet, NetworkInterface* interface) {
    // we do not want to send/reinforce routes that would send the packet back over ourselves
    // Please note that we deliberately decided to enable this check even if isPreviousHopFeatureActivated is false!
    if (isLocalAddress(packet->getPreviousHop()) == false) {
        // trigger the evaporation first so this does not effect the new route or update
        routingTable->triggerEvaporation();

        AddressPtr source = packet->getSource();
        AddressPtr sender = packet->getSender();
        if (routingTable->isNewRoute(source, sender, interface)) {

            if(hasPreviousNodeBeenSeenBefore(packet) == false) {
                createNewRouteFrom(packet, interface);
            }
            else {
                logTrace("Did not create new route to %s via %s (prevHop %s or sender has been seen before)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), packet->getPreviousHop()->toString().c_str());
            }
        }
        else {
            reinforcePheromoneValue(source, sender, interface);
        }
    }
}

void AbstractARAClient::createNewRouteFrom(Packet* packet, NetworkInterface* interface) {
    float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
    routingTable->update(packet->getSource(), packet->getSender(), interface, initialPheromoneValue);
    // DEBUG: logTrace("Created new route to %s via %s (phi=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue);

    if(hasPreviousNodeBeenSeenBefore(packet) == false) {
        float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
        routingTable->update(packet->getSource(), packet->getSender(), interface, initialPheromoneValue);
        logTrace("Created new route to %s via %s (phi=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue);
        //logDebug("Routing Table:");
        //logDebug("%s", routingTable->toString().c_str());
    } else {
        logTrace("Did not create new route to %s via %s (prevHop %s or sender has been seen before)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), packet->getPreviousHop()->toString().c_str());
    }
    //logTrace("Created new route to %s via %s (phi=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue);
    //logAllRoutingTableEntries();

    std::cerr << "[AbstractARAClient::createNewRouteFrom] dump routing table " << std::endl;
    std::cerr << *routingTable;
    std::cerr << std::endl;
}
/*
void AbstractARAClient::logAllRoutingTableEntries() {
    logTrace("Routing Table:");
    RoutingTableEntryTupel routingTableEntry;
    int nrOfEntries = routingTable->getTotalNumberOfEntries();
    for (int i = 0; i < nrOfEntries; ++i) {
        routingTableEntry = routingTable->getEntryAt(i);

        std::string destination = routingTableEntry.destination->toString();
        std::string nextHop = routingTableEntry.entry->getNextHop()->getAddress()->toString();
        float phi = routingTableEntry.entry->getPheromoneValue();
        logDebug("[%d] next hop: %s, destination %s, phi: %f", i, nextHop.c_str(), destination.c_str(), phi);
    }    
    //logTrace("Created new route to %s via %s (phi=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue);
    if(hasPreviousNodeBeenSeenBefore(packet) == false) {
        float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
        routingTable->update(packet->getSource(), packet->getSender(), interface, initialPheromoneValue);
       // logTrace("Created new route to %s via %s (phi=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue);
       // logDebug("Routing Table:");
        RoutingTableEntryTupel routingTableEntry;
        for (int i = 0; i < routingTable->getTotalNumberOfEntries(); ++i) {
            routingTableEntry = routingTable->getEntryAt(i);
            /// get the destination
            std::string destination = routingTableEntry.destination.get()->toString();
            /// get the next hop
            std::string nextHop = routingTableEntry.entry->getNextHop()->getAddress().get()->toString();
            /// get the pheromone value
            float phi = routingTableEntry.entry->getPheromoneValue();

            //logDebug("[%d] next hop: %s, destination %s, phi: %f", i, nextHop.c_str(), destination.c_str(), phi);
        }
    }
    else {
        logTrace("Did not create new route to %s via %s (prevHop %s or sender has been seen before)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), packet->getPreviousHop()->toString().c_str());
    }
}
    */

bool AbstractARAClient::hasPreviousNodeBeenSeenBefore(const Packet* packet) {
    AddressPtr source = packet->getSource();

    if (isNewRouteDiscovery(packet)) {
        //TODO this might become a problem for overlapping route discoveries
        delete knownIntermediateHops[source];
        knownIntermediateHops.erase(source);
        return false;
    }
    else {
        KnownIntermediateHopsMap::const_iterator found = knownIntermediateHops.find(source);
        if (found == knownIntermediateHops.end()) {
            // we have never seen any packet for this source address so we can not have seen this previous node before
            return false;
        }

        unordered_set<AddressPtr>* listOfKnownNodes = found->second;

        // have we seen this sender, or the previous hop before?
        bool senderHasBeenSeen = listOfKnownNodes->find(packet->getSender()) != listOfKnownNodes->end();

        if (isPreviousHopFeatureActivated) {
            bool prevHopHasBeenSeen = listOfKnownNodes->find(packet->getPreviousHop()) != listOfKnownNodes->end();
            return senderHasBeenSeen || prevHopHasBeenSeen;
        }
        else {
            return senderHasBeenSeen;
        }
    }
}

bool AbstractARAClient::isNewRouteDiscovery(const Packet* packet) {
    if (packet->isAntPacket()) {
        AddressPtr source = packet->getSource();
        unsigned int sequenceNumber = packet->getSequenceNumber();

        LastRouteDiscoveriesMap::const_iterator foundLastRouteDiscovery = lastRouteDiscoverySeqNumbers.find(source);
        if (foundLastRouteDiscovery == lastRouteDiscoverySeqNumbers.end()) {
            // we have never seen any route discovery from this source so far
            lastRouteDiscoverySeqNumbers[source] = sequenceNumber;
            return true;
        }
        else {
            unsigned int seqNrOfLastRouteDiscovery = foundLastRouteDiscovery->second;

            if (sequenceNumber == seqNrOfLastRouteDiscovery) {
                // this is the same route discovery we already saw
                return false;
            }
            else {
                // this seems to be a new route discovery
                lastRouteDiscoverySeqNumbers[source] = sequenceNumber;
                return true;
            }
        }
    }
    else {
        // this is not even an ant packet so it can't be part of a route discovery
        return false;
    }
}

void AbstractARAClient::handlePacket(Packet* packet, NetworkInterface* interface) {
    if (packet->isDataPacket()) {
        registerActivity(packet->getSender(), interface);
        handleDataPacket(packet);
    } else if(packet->isAntPacket()) {
        registerActivity(packet->getSender(), interface);
        handleAntPacket(packet, interface);
    } else if (packet->getType() == PacketType::DUPLICATE_ERROR) {
        handleDuplicateErrorPacket(packet, interface);
    } else if (packet->getType() == PacketType::ROUTE_FAILURE) {
        handleRouteFailurePacket(packet, interface);
    } else if (packet->getType() == PacketType::HELLO) {
        // this has already been acknowledged on the layer 2 so we can ignore this one
        delete packet;
    } else {
        throw Exception("Can not handle packet");
    }
}

void AbstractARAClient::handleDataPacket(Packet* packet) {
    // DEBUG: 
    std::cerr << "[AbstractARAClient::handleDataPacket] source " << packet->getSource()->toString() << " destination " << packet->getDestination()->toString() << std::endl;

    if(isDirectedToThisNode(packet)) {
        handleDataPacketForThisNode(packet);
    } else {
        sendPacket(packet);
    }
}

void AbstractARAClient::handleDataPacketForThisNode(Packet* packet) {
    logInfo("Packet %u from %s reached its destination", packet->getSequenceNumber(), packet->getSourceString().c_str());
    deliverToSystem(packet);
    checkPantTimer(packet);
}

void AbstractARAClient::checkPantTimer(const Packet* packet) {
    // this should only be called for arrived DATA packets for this node
    if (pantIntervalInMilliSeconds > 0) {
        // only send PANTs if this feature is enabled
        AddressPtr pantDestination = packet->getSource();
        if (scheduledPANTs.find(pantDestination) == scheduledPANTs.end()) {
            // only start PANT if no timer is already running
            logDebug("Scheduled PANT to be sent in %u ms", pantIntervalInMilliSeconds);

            Clock* clock = Environment::getClock();
            TimerPtr pantTimer = clock->getNewTimer(TimerType::PANTS_TIMER, new TimerAddressInfo(pantDestination));
            pantTimer->addTimeoutListener(this);
            pantTimer->run(pantIntervalInMilliSeconds * 1000);

            scheduledPANTs[pantDestination] = pantTimer;
        }
    }
}

void AbstractARAClient::handleAntPacket(Packet* packet, NetworkInterface* interface) {
    if (hasBeenSentByThisNode(packet)) {
        // do not process ant packets we have sent ourselves
        delete packet;
        return;
    }

    if (isDirectedToThisNode(packet)) {
        handleAntPacketForThisNode(packet);
    }
    else if (packet->getTTL() > 0) {
        logDebug("Broadcasting %s %u from %s to %s (came from %s)", PacketType::getAsString(packet->getType()).c_str(), packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getDestinationString().c_str(), packet->getSenderString().c_str());
        broadCast(packet);
    }
    else {
        // do not broadcast this ANT packet any further (TTL = 0)
        delete packet;
    }
}

void AbstractARAClient::handleAntPacketForThisNode(Packet* packet) {
    char packetType = packet->getType();

    if(packetType == PacketType::FANT) {
        logDebug("FANT %u from %s reached its destination. Broadcasting BANT", packet->getSequenceNumber(), packet->getSourceString().c_str());
        broadcastBANT(packet);
    }
    else if(packetType == PacketType::BANT) {
        handleBANTForThisNode(packet);
    }
    else if(packetType == PacketType::PANT) {
        // don't do anything other than deleting the packet
    }
    else {
        logError("Can not handle ANT packet %u from %s (unknown type %u)", packet->getSequenceNumber(), packet->getSourceString().c_str(), packetType);
    }

    delete packet;
}

void AbstractARAClient::broadcastBANT(Packet* fant) {
    Packet* bant = packetFactory->makeBANT(fant, getNextSequenceNumber());
    for(auto& interface: interfaces) {
        Packet* newBant = packetFactory->makeClone(bant);
        interface->broadcast(newBant);
    }
    delete bant;
}

void AbstractARAClient::handleBANTForThisNode(Packet* bant) {
    AddressPtr routeDiscoveryDestination = bant->getSource();

    if(packetTrap->getNumberOfTrappedPackets(routeDiscoveryDestination) == 0) {
        logWarn("Received BANT %u from %s via %s but there are no trapped packets for this destination.", bant->getSequenceNumber(), bant->getSourceString().c_str(), bant->getSenderString().c_str());
    } else {
        logDebug("First BANT %u came back from %s via %s. Waiting %u ms until delivering the trapped packets", bant->getSequenceNumber(), bant->getSourceString().c_str(), bant->getSenderString().c_str(), packetDeliveryDelayInMilliSeconds);
        stopRouteDiscoveryTimer(routeDiscoveryDestination);
        startDeliveryTimer(routeDiscoveryDestination);
    }
}

void AbstractARAClient::stopRouteDiscoveryTimer(AddressPtr destination) {
    RunningRouteDiscoveriesMap::const_iterator discovery;
    discovery = runningRouteDiscoveries.find(destination);

    if(discovery != runningRouteDiscoveries.end()) {
        TimerPtr timer = discovery->second;
        timer->interrupt();
        /**
         * the route discovery is not completely finished until the delivery timer expired.
         * only then is runningRouteDiscoveries.erase(discovery) called!
         */
        delete (RouteDiscoveryInfo*) timer->getContextObject();

        // set it explictely to null
        timer->setContextObject(nullptr);
    } else {
        logError("Could not stop route discovery timer (not found for destination %s)", destination->toString().c_str());
    }
}

void AbstractARAClient::startDeliveryTimer(AddressPtr destination) {
    TimerAddressInfo* contextObject = new TimerAddressInfo(destination);
    TimerPtr timer = getNewTimer(TimerType::DELIVERY_TIMER, contextObject);
    timer->addTimeoutListener(this);
    timer->run(packetDeliveryDelayInMilliSeconds * 1000);
    runningDeliveryTimers.insert(timer);
}

void AbstractARAClient::sendDeliverablePackets(AddressPtr destination) {
    PacketQueue deliverablePackets = packetTrap->untrapDeliverablePackets(destination);
    logInfo("Sending %u trapped packet(s) for destination %s", deliverablePackets.size(), destination->toString().c_str());

    for(auto& deliverablePacket : deliverablePackets) {
        sendPacket(deliverablePacket);
    }
}

void AbstractARAClient::handleDuplicateErrorPacket(Packet* duplicateErrorPacket, NetworkInterface* interface) {
    logInfo("Received DUPLICATE_ERROR from %s. Deleting route to %s via %s", duplicateErrorPacket->getSourceString().c_str(), duplicateErrorPacket->getDestinationString().c_str(), duplicateErrorPacket->getSenderString().c_str());
    deleteRoutingTableEntry(duplicateErrorPacket->getDestination(), duplicateErrorPacket->getSender(), interface);
    delete duplicateErrorPacket;
}

bool AbstractARAClient::hasBeenReceivedEarlier(const Packet* packet) {
    AddressPtr source = packet->getSource();
    unsigned int sequenceNumber = packet->getSequenceNumber();

    LastReceivedPacketsMap::const_iterator receivedPacketSeqNumbersFromSource = lastReceivedPackets.find(source);
    if(receivedPacketSeqNumbersFromSource != lastReceivedPackets.end()) {
        unordered_set<unsigned int>* sequenceNumbers = receivedPacketSeqNumbersFromSource->second;
        unordered_set<unsigned int>::const_iterator got = sequenceNumbers->find(sequenceNumber);
        if(got != sequenceNumbers->end()) {
            return true;
        }
    }
    return false;
}

void AbstractARAClient::registerReceivedPacket(const Packet* packet) {
    AddressPtr source = packet->getSource();
    AddressPtr sender = packet->getSender();

    // first check the lastReceived sequence numbers for this source
    LastReceivedPacketsMap::const_iterator foundPacketSeqNumbersFromSource = lastReceivedPackets.find(source);
    unordered_set<unsigned int>* listOfSequenceNumbers;
    if(foundPacketSeqNumbersFromSource == lastReceivedPackets.end()) {
        // There is no record of any received packet from this source address ~> create new
        listOfSequenceNumbers = new unordered_set<unsigned int>();
        listOfSequenceNumbers->insert(packet->getSequenceNumber());
        lastReceivedPackets[source] = listOfSequenceNumbers;
    }
    else {
        listOfSequenceNumbers = foundPacketSeqNumbersFromSource->second;
        listOfSequenceNumbers->insert(packet->getSequenceNumber());
    }

    // now check the known intermediate hops for this destination
    KnownIntermediateHopsMap::const_iterator foundIntermediateHopsForSource = knownIntermediateHops.find(source);
    unordered_set<AddressPtr>* listOfKnownIntermediateNodes;
    if(foundIntermediateHopsForSource == knownIntermediateHops.end()) {
        // There is no record of any known intermediate node for this source address ~> create new
        listOfKnownIntermediateNodes = new unordered_set<AddressPtr>();
        listOfKnownIntermediateNodes->insert(packet->getSender());
        knownIntermediateHops[source] = listOfKnownIntermediateNodes;
    }
    else {
        listOfKnownIntermediateNodes = foundIntermediateHopsForSource->second;
        listOfKnownIntermediateNodes->insert(sender);
    }

    if(isPreviousHopFeatureActivated) {
        AddressPtr previousHop = packet->getPreviousHop();
        if(packet->getPreviousHop()->equals(sender) == false) {
            listOfKnownIntermediateNodes->insert(previousHop);
        }
    }
}

float AbstractARAClient::calculateInitialPheromoneValue(unsigned int ttl) {
    int alpha = 1; // may change in the future implementations
    return alpha * ttl + initialPheromoneValue;
}

void AbstractARAClient::setMaxNrOfRouteDiscoveryRetries(int maxNrOfRouteDiscoveryRetries) {
    this->maxNrOfRouteDiscoveryRetries = maxNrOfRouteDiscoveryRetries;
}

void AbstractARAClient::timerHasExpired(std::weak_ptr<Timer> responsibleTimer) {
    std::shared_ptr<Timer> timer = responsibleTimer.lock();

    if (timer->getType() == TimerType::NEIGHBOR_ACTIVITY_TIMER) {
        checkInactiveNeighbors();
        startNeighborActivityTimer();

    } else if(timer->getType() == TimerType::ROUTE_DISCOVERY_TIMER) {
        handleExpiredRouteDiscoveryTimer(responsibleTimer);

    } else if(timer->getType() == TimerType::PANTS_TIMER) {
        handleExpiredPANTTimer(responsibleTimer);

    } else if(timer->getType() == TimerType::DELIVERY_TIMER) {
        handleExpiredDeliveryTimer(responsibleTimer);

    } else {
       // if this happens its a bug in our code
       logError("Could not identify expired timer");
    }
}

void AbstractARAClient::handleExpiredRouteDiscoveryTimer(std::weak_ptr<Timer> routeDiscoveryTimer) {
    std::shared_ptr<Timer> timer = routeDiscoveryTimer.lock();

    if (timer->getContextObject() != nullptr) {
        RouteDiscoveryInfo* discoveryInfo = (RouteDiscoveryInfo*) timer->getContextObject();
        AddressPtr destination = discoveryInfo->getPacket()->getDestination();
        logInfo("Route discovery for destination %s timed out", destination->toString().c_str());

        int nrOfRetries = discoveryInfo->getNumberOfRetries();

        if (nrOfRetries < maxNrOfRouteDiscoveryRetries) {
            nrOfRetries++;
            // restart the route discovery
            discoveryInfo->setNumberOfRetries(nrOfRetries);
            logInfo("Restarting discovery for destination %s (%u/%u)", destination->toString().c_str(), nrOfRetries, maxNrOfRouteDiscoveryRetries);
            forgetKnownIntermediateHopsFor(destination);
            broadcastFANT(destination);

            timer->run(routeDiscoveryTimeoutInMilliSeconds * 1000);
        } else {
            // delete the route discovery timer
            runningRouteDiscoveries.erase(destination);

            if (!discoveryInfo) {
                delete discoveryInfo;
            }

            forgetKnownIntermediateHopsFor(destination);
            deque<Packet*> undeliverablePackets = packetTrap->removePacketsForDestination(destination);
            logWarn("Route discovery for destination %s unsuccessful. Dropping %u packet(s)", destination->toString().c_str(), undeliverablePackets.size());
            for (auto& packet: undeliverablePackets) {
                packetNotDeliverable(packet);
            }
        }
    /**
     * This is so unlikely that it should actually never happen. The route
     * discovery timer is stopped if the route discovery never found a
     * destination or if a BANT came back and initialized a delivery timer.
     * In the latter case the route discovery object is removed but the timer
     * still exists (and is interrupted). However, in some cases the timer
     * continues to run, triggering at some point the callback which calls the
     * handleExpiredRouteDiscoveryTimer(). 
     */
    } else {
      // TODO: Urgent! We need to investigate this further! 
      logWarn("A route discovery timer expired, but no route discovery info was found!");
    }
}

void AbstractARAClient::handleExpiredDeliveryTimer(std::weak_ptr<Timer> deliveryTimer) {
    std::shared_ptr<Timer> timer = deliveryTimer.lock();
    TimerAddressInfo* timerInfo = (TimerAddressInfo*) timer->getContextObject();
    AddressPtr destination = timerInfo->getDestination();

    RunningRouteDiscoveriesMap::const_iterator discovery;
    discovery = runningRouteDiscoveries.find(destination);

    if (discovery != runningRouteDiscoveries.end()) {
        // its important to delete the discovery info first or else the client will always think the route discovery is still running and never send any packets
        runningRouteDiscoveries.erase(discovery);
        runningDeliveryTimers.erase(timer);

        if (!timerInfo) {
            delete timerInfo;
        }

        sendDeliverablePackets(destination);
    } else {
        logError("Could not find running route discovery object for destination %s)", destination->toString().c_str());
    }
}

void AbstractARAClient::handleExpiredPANTTimer(std::weak_ptr<Timer> pantTimer) {
    std::shared_ptr<Timer> timer = pantTimer.lock();
    TimerAddressInfo* timerInfo = (TimerAddressInfo*)timer->getContextObject();
    scheduledPANTs.erase(timerInfo->getDestination());
    broadcastPANT(timerInfo->getDestination());
    delete timerInfo;
}

bool AbstractARAClient::handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    logInfo("Link over %s is broken", nextHop->toString().c_str());

    // delete all known routes via this next hop
    std::deque<RoutingTableEntryTupel> allRoutesOverNextHop = routingTable->getAllRoutesThatLeadOver(nextHop);
    for (auto& route: allRoutesOverNextHop) {
        deleteRoutingTableEntry(route.destination, nextHop, route.entry->getNetworkInterface());
    }

    NeighborActivityMap::const_iterator foundNeighbor = neighborActivityTimes.find(nextHop);
    if(foundNeighbor != neighborActivityTimes.end()) {
        // delete the associated Time object first
        delete foundNeighbor->second.first;
        neighborActivityTimes.erase(nextHop);
    }

    // Try to deliver the packet on an alternative route
    if (routingTable->isDeliverable(packet)) {
        logDebug("Sending %u from %s over alternative route", packet->getSequenceNumber(), packet->getSourceString().c_str());
        sendPacket(packet);
        return true;
    } else if(packet->isDataPacket() && isLocalAddress(packet->getSource())) {
        packetTrap->trapPacket(packet);

        if (isRouteDiscoveryRunning(packet->getDestination())) {
            logDebug("No alternative route is available. Trapping packet %u from %s because route discovery is already running for destination %s.", packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getDestinationString().c_str());
        } else {
            logDebug("No alternative route is available. Starting new route discovery for packet %u from %s.", packet->getSequenceNumber(), packet->getSourceString().c_str());
            startNewRouteDiscovery(packet);
        }
        return true;
    }
    else {
        // packet can not be delivered on another route so we can only drop it
        delete packet;
        return false;
    }
}

void AbstractARAClient::registerActivity(AddressPtr neighbor, NetworkInterface* interface) {
    NeighborActivityMap::const_iterator foundNeighbor = neighborActivityTimes.find(neighbor);
    if(foundNeighbor == neighborActivityTimes.end()) {
        // we have never heard from this neighbor before
        Clock* clock = Environment::getClock();
        Time* currentTime = clock->makeTime();
        currentTime->setToCurrentTime();
        neighborActivityTimes[neighbor] = std::pair<Time*, NetworkInterface*>(currentTime, interface);
    }
    else {
        // just update the activity time for one of the currently known neighbors
        foundNeighbor->second.first->setToCurrentTime();
    }
}

void AbstractARAClient::checkInactiveNeighbors() {
    Clock* clock = Environment::getClock();
    Time* currentTime = clock->makeTime();
    currentTime->setToCurrentTime();

    NeighborActivityMap::iterator iterator;
    for (iterator=neighborActivityTimes.begin(); iterator!=neighborActivityTimes.end(); iterator++) {
        std::pair<AddressPtr, std::pair<Time*, NetworkInterface*>> entryPair = *iterator;
        Time* lastActiveTime = entryPair.second.first;
        long timeDifference = currentTime->getDifferenceInMilliSeconds(lastActiveTime);
        if (timeDifference >= maxNeighborInactivityTimeInMilliSeconds) {
            AddressPtr addressofNeighbor = entryPair.first;
            NetworkInterface* interface = entryPair.second.second;
            unsigned int sequenceNumber = getNextSequenceNumber();
            Packet* helloPacket = packetFactory->makeHelloPacket(interface->getLocalAddress(), addressofNeighbor, sequenceNumber);
            logDebug("Sending HELLO packet to inactive neighbor %s", addressofNeighbor->toString().c_str());
            sendUnicast(helloPacket, interface, addressofNeighbor);
        }
    }

    delete currentTime;
}

void AbstractARAClient::handleRouteFailurePacket(Packet* packet, NetworkInterface* interface) {
    AddressPtr destination = packet->getDestination();
    AddressPtr nextHop = packet->getSource();

    if (routingTable->exists(destination, nextHop, interface)) {
        logInfo("Received ROUTE_FAILURE from %s. Deleting route to %s via %s", packet->getSourceString().c_str(), packet->getDestinationString().c_str(), packet->getSourceString().c_str());
        deleteRoutingTableEntry(destination, nextHop, interface);
    }

    delete packet;
}

void AbstractARAClient::deleteRoutingTableEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    if(routingTable->exists(destination, nextHop, interface)) {
        routingTable->removeEntry(destination, nextHop, interface);

        deque<RoutingTableEntry*> possibleNextHops = routingTable->getPossibleNextHops(destination);
        if (possibleNextHops.size() == 1) {
            onlyOneRouteFailure++;
            RoutingTableEntry* lastRemainingRoute = possibleNextHops.front();
            AddressPtr remainingNextHop = lastRemainingRoute->getAddress();
            logDebug("Only one last route is known to %s. Notifying %s with ROUTE_FAILURE packet", destination->toString().c_str(), remainingNextHop->toString().c_str());
            AddressPtr source = interface->getLocalAddress();
            unsigned int sequenceNr = getNextSequenceNumber();
            Packet* routeFailurePacket = packetFactory->makeRouteFailurePacket(source, destination, sequenceNr);
            lastRemainingRoute->getNetworkInterface()->send(routeFailurePacket, remainingNextHop);
        }
        else if (possibleNextHops.empty()) {
            allRoutesHaveCollapsedFailure++;
            logInfo("All known routes to %s have collapsed. Sending ROUTE_FAILURE packet", destination->toString().c_str());
            broadcastRouteFailure(destination);
        }
    }
}

void AbstractARAClient::broadcastRouteFailure(AddressPtr destination) {
    for(auto& interface: interfaces) {
        AddressPtr localAddress = interface->getLocalAddress();
        unsigned int sequenceNr = getNextSequenceNumber();
        Packet* routeFailurePacket = packetFactory->makeRouteFailurePacket(localAddress, destination, sequenceNr);
        interface->broadcast(routeFailurePacket);
    }
}

void AbstractARAClient::broadcastPANT(AddressPtr destination) {
    logDebug("Sending new PANT over all interfaces");
    for(auto& interface: interfaces) {
        AddressPtr source = interface->getLocalAddress();
        unsigned int sequenceNr = getNextSequenceNumber();
        Packet* pant = packetFactory->makePANT(source, destination, sequenceNr);
        interface->broadcast(pant);
    }
}

void AbstractARAClient::setForwardingPolicy(ForwardingPolicy* newForwardingPolicy) {
    DELETE_IF_NOT_NULL(forwardingPolicy);
    forwardingPolicy = newForwardingPolicy;
}

int AbstractARAClient::getMaxTTL() const {
    return packetFactory->getMaximumNrOfHops();
}

TimerPtr AbstractARAClient::getNewTimer(char timerType, void* contextObject) const {
    return Environment::getClock()->getNewTimer(timerType, contextObject);
}

std::string AbstractARAClient::getStatistics() {
    std::ostringstream result;

    result << "General ARA Statistics:" << std::endl;
    result << " Route Failures" << std::endl;
    result << "   only one last route:             " << onlyOneRouteFailure << std::endl;
    result << "   all known routes have collapsed: " << allRoutesHaveCollapsedFailure << std::endl;

    result << " Duplicate Warnings" << std::endl;
    result << "   routing loops:                   " << routingLoopFailure << std::endl;
    return result.str();
}

ARA_NAMESPACE_END
