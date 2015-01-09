/*
 * $FU-Copyright$
 */

#include "ReliableNetworkInterface.h"
#include "Environment.h"
#include "Timer.h"


#include <thread>

using namespace std;

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

ReliableNetworkInterface::ReliableNetworkInterface(AbstractNetworkClient* client, int ackTimeoutInMicroSeconds, AddressPtr localAddress, AddressPtr broadcastAddress) : AbstractNetworkInterface(client, localAddress, broadcastAddress) {
    this->ackTimeoutInMicroSeconds = ackTimeoutInMicroSeconds;
    this->packetFactory = client->getPacketFactory();
}

ReliableNetworkInterface::~ReliableNetworkInterface() {
    for(auto& packet : unacknowledgedPackets) {
        delete packet;
    }

    runningTimers.clear();
}

void ReliableNetworkInterface::send(const Packet* packet, AddressPtr recipient) {
    // DEBUG: std::cerr << "[ReliableNetworkInterface::send] send packet to  " << recipient->toString() << std::endl;
    unacknowledgedPackets.push_back(packet);
    // DEBUG: std::cerr << "[ReliableNetworkInterface::send] packet dump " << std::endl;
    // DEBUG: std::cerr << packet << std::endl;
    doSend(packet, recipient);
    // DEBUG: std::cerr << "[ReliableNetworkInterface::send] start acknowledgment timer " << std::endl;
    startAcknowledgmentTimer(packet, recipient);
}

void ReliableNetworkInterface::startAcknowledgmentTimer(const Packet* packet, AddressPtr recipient) {
    assert(packet != nullptr);
    // DEBUG: std::cerr << "[ReliableNetworkInterface::startAcknowledgmentTimer] get a new timer " << std::endl;
    TimerPtr ackTimer = Environment::getClock()->getNewTimer(TimerType::ACK_TIMER);
    // DEBUG: std::cerr << "[ReliableNetworkInterface::startAcknowledgmentTimer] add to timeout listener " << std::endl;
    ackTimer->addTimeoutListener(this);
    // DEBUG: std::cerr << "[ReliableNetworkInterface::startAcknowledgmentTimer] run timer " << std::endl;
    ackTimer->run(ackTimeoutInMicroSeconds);

    AckTimerData timerData; 
    timerData.nrOfRetries = 0;
    // DEBUG: std::cerr << "[ReliableNetworkInterface::startAcknowledgmentTimer] store packet " << std::endl;
    timerData.packet = packet;
    timerData.recipient = recipient;

    // DEBUG: std::cerr << "[ReliableNetworkInterface::startAcknowledgmentTimer] start ack timer for " << packet->getSource()->toString() << " and seq nr " << packet->getSequenceNumber() << std::endl;
    runningTimers[ackTimer] = timerData;
}

void ReliableNetworkInterface::timerHasExpired(std::weak_ptr<Timer> ackTimer) {
    AckTimerData timerData;
    std::shared_ptr<Timer> timer = ackTimer.lock();
    
    // some acknowledgment timed out so we need to send the packet again or tell the client
    timerData = runningTimers[timer];

    // DEBUG: std::cerr << "[ReliableNetworkInterface::timerHasExpired] timer has expired. retry nr.: " << timerData.nrOfRetries << std::endl;
    
    if (timerData.nrOfRetries < maxNrOfRetransmissions) {
        timerData.nrOfRetries++;
        runningTimers[timer] = timerData;
        assert(timerData.packet != nullptr);
        doSend(timerData.packet, timerData.recipient);
        timer->run(ackTimeoutInMicroSeconds);
    } else {
        handleUndeliverablePacket(ackTimer, timerData);
    }
}

void ReliableNetworkInterface::handleUndeliverablePacket(std::weak_ptr<Timer> ackTimer, AckTimerData& timerData) {
    std::shared_ptr<Timer> timer = ackTimer.lock();
    runningTimers.erase(timer);

    // delete the packet from the list of unacknowledged packets
    deque<const Packet*>::iterator currentPacket;
    for (currentPacket = unacknowledgedPackets.begin(); currentPacket!=unacknowledgedPackets.end(); currentPacket++) {
        if ((*currentPacket)->equals(timerData.packet)) {
            unacknowledgedPackets.erase(currentPacket);
            break;
        }
    }

    client->handleBrokenLink(const_cast<Packet*>(timerData.packet), timerData.recipient, this);
}

void ReliableNetworkInterface::broadcast(const Packet* packet) {
    doSend(packet, broadcastAddress);
    delete packet;
}

void ReliableNetworkInterface::receive(Packet* packet) {
    assert(packet != nullptr);

    if (packet->getType() != PacketType::ACK) {
        // DEBUG: std::cerr << "[ReliableNetworkInterface::receive] handling non ack packet " << std::endl;
        handleNonAckPacket(packet);
    } else {
        // DEBUG: std::cerr << "[ReliableNetworkInterface::receive] handling ack packet " << std::endl;
        handleAckPacket(packet);
    }
}

void ReliableNetworkInterface::handleNonAckPacket(Packet* packet) {
    AddressPtr destination = packet->getDestination();

    if(packet->isAntPacket() == false) { // TODO actually we want to test if the packet has been sent via a broadcast but this is currently not possible with the API
        Packet* ackPacket = packetFactory->makeAcknowledgmentPacket(packet, getLocalAddress());
        // DEBUG: std::cerr << "[ReliableNetworkInterface::handleNonAckPacket] we are going to send an ack packet. the content is" << std::endl;
        // DEBUG: std::cerr << ackPacket->toString();
        doSend(ackPacket, packet->getSender());
        delete ackPacket;
    }

    client->receivePacket(packet, this);
}

void ReliableNetworkInterface::handleAckPacket(Packet* ackPacket) {
    // DEBUG: std::cerr << "[ReliableNetworkInterface::handleAckPacket] trying to find packet. the content of the ack packet is" << std::endl;
    // DEBUG: std::cerr << ackPacket->toString();

    unsigned int acknowledgedSeqNr = ackPacket->getSequenceNumber();
    AddressPtr acknowledgedSource = ackPacket->getSource();

    // stop the timer
    unordered_map<TimerPtr, AckTimerData>::iterator iterator;
    for (iterator=runningTimers.begin(); iterator!=runningTimers.end(); iterator++) {
        pair<TimerPtr, AckTimerData> entryPair = *iterator;
        TimerPtr timer = entryPair.first;
        AckTimerData timerData = entryPair.second;

        // DEBUG: std::cerr << "[ReliableNetworkInterface::handleAckPacket] (1/2). " << timerData.packet->getSequenceNumber() << " and " << acknowledgedSeqNr << std::endl;
        // DEBUG: std::cerr << "[ReliableNetworkInterface::handleAckPacket] (2/2). " << timerData.packet->getSource()->toString() << " and " <<  acknowledgedSource->toString() << std::endl;
        if(timerData.packet->getSequenceNumber() == acknowledgedSeqNr
           && timerData.packet->getSource()->equals(acknowledgedSource) ) {
            // DEBUG: std::cerr << "[ReliableNetworkInterface::handleAckPacket] found an ack timer. trying to stop it" << std::endl;
            timer->interrupt();
            runningTimers.erase(timer);
            break;
        }
    }

    // remove the packet from the list of unacknowledged packets
    for (std::deque<const Packet*>::iterator iterator = unacknowledgedPackets.begin(); iterator!=unacknowledgedPackets.end(); iterator++) {
        const Packet* currentPacket = *iterator;

        if (currentPacket->getSequenceNumber() == acknowledgedSeqNr && currentPacket->getSource()->equals(acknowledgedSource)) {
            // remove the packet from the list of unacknowledged packets
            unacknowledgedPackets.erase(iterator);

            // free the memory
            delete currentPacket;
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
