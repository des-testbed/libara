/*
 * $FU-Copyright$
 */

#include "ReliableNetworkInterface.h"
#include "Environment.h"
#include "Timer.h"

using namespace std;

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

ReliableNetworkInterface::ReliableNetworkInterface(AbstractARAClient* client, AddressPtr localAddress, AddressPtr broadcastAddress, int ackTimeoutInMillis) : AbstractNetworkInterface(client, localAddress, broadcastAddress) {
    unacknowledgedPackets = deque<const Packet*>();
    runningTimers = unordered_map<Timer*, AckTimerData>();
    this->ackTimeoutInMillis = ackTimeoutInMillis;
}

ReliableNetworkInterface::~ReliableNetworkInterface() {
    for(auto& packet : unacknowledgedPackets) {
        delete packet;
    }

    unordered_map<Timer*, AckTimerData>::iterator iterator;
    for (iterator=runningTimers.begin(); iterator!=runningTimers.end(); iterator++) {
        pair<Timer*, AckTimerData> entryPair = *iterator;
        Timer* timer = entryPair.first;
        delete timer;
    }
    runningTimers.clear();
}

void ReliableNetworkInterface::send(const Packet* packet, AddressPtr recipient) {
    unacknowledgedPackets.push_back(packet);
    doSend(packet, recipient);
    startAcknowledgmentTimer(packet, recipient);
}

void ReliableNetworkInterface::startAcknowledgmentTimer(const Packet* packet, AddressPtr recipient) {
    Timer* ackTimer = Environment::getClock()->getNewTimer();
    ackTimer->addTimeoutListener(this);
    ackTimer->run(ackTimeoutInMillis);

    AckTimerData timerData;
    timerData.nrOfRetries = 0;
    timerData.packet = packet;
    timerData.recipient = recipient;

    runningTimers[ackTimer] = timerData;
}

void ReliableNetworkInterface::timerHasExpired(Timer* ackTimer) {
    // some acknowledgment timed out so we need to send the packet again or tell the client
    AckTimerData timerData = runningTimers[ackTimer];
    if(timerData.nrOfRetries < maxNrOfRetransmissions) {
        timerData.nrOfRetries++;
        runningTimers[ackTimer] = timerData;
        doSend(timerData.packet, timerData.recipient);
    }
    else {
        handleUndeliverablePacket(ackTimer, timerData);
    }
}

void ReliableNetworkInterface::handleUndeliverablePacket(Timer* ackTimer, AckTimerData& timerData) {
    // remove the acknowledgment timer
    runningTimers.erase(ackTimer);
    delete ackTimer;

    // delete the packet from the list of unacknowledged packets
    //TODO make this more efficient with a hashmap
    deque<const Packet*>::iterator currentPacket;
    for (currentPacket = unacknowledgedPackets.begin(); currentPacket!=unacknowledgedPackets.end(); currentPacket++) {
        if ((*currentPacket)->equals(timerData.packet)) {
            unacknowledgedPackets.erase(currentPacket);
            break;
        }
    }

    client->packetIsNotDeliverable(timerData.packet, timerData.recipient, this);
}

void ReliableNetworkInterface::broadcast(const Packet* packet) {
    doSend(packet, broadcastAddress);
    delete packet;
}

void ReliableNetworkInterface::receive(Packet* packet) {
    if (packet->getType() != PacketType::ACK) {
        handleNonAckPacket(packet);
    }
    else {
        handleAckPacket(packet);
    }
}

void ReliableNetworkInterface::handleNonAckPacket(Packet* packet) {
    AddressPtr destination = packet->getDestination();

    if(isBroadcastAddress(destination) == false) {
        Packet* ackPacket = packet->createAcknowledgment();
        send(ackPacket, packet->getSender());
    }

    client->receivePacket(packet, this);
}

void ReliableNetworkInterface::handleAckPacket(Packet* ackPacket) {
    unsigned int acknowledgedSeqNr = ackPacket->getSequenceNumber();
    AddressPtr acknowledgedSource = ackPacket->getSource();
    for (std::deque<const Packet*>::iterator iterator = unacknowledgedPackets.begin(); iterator!=unacknowledgedPackets.end(); iterator++) {
        const Packet* currentPacket = *iterator;

        if (currentPacket->getSequenceNumber() == acknowledgedSeqNr && currentPacket->getSource()->equals(acknowledgedSource)) {
            // remove the packet from the list of unacknowledged packets
            unacknowledgedPackets.erase(iterator);

            // free the memory
            delete currentPacket;

            // break out of the for loop
            break;
        }
    }

    delete ackPacket;
}

std::deque<const Packet*> ReliableNetworkInterface::getUnacknowledgedPackets() const {
    return unacknowledgedPackets;
}

void ReliableNetworkInterface::setMaxNrOfRetransmissions(int n) {
    maxNrOfRetransmissions = n;
}

} /* namespace ARA */
