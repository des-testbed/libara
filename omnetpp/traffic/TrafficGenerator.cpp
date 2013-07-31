/*
 * $FU-Copyright$
 */

#include "TrafficGenerator.h"
#include "TrafficControllInfo.h"
#include "TrafficPacket_m.h"
#include "NotificationBoard.h"
#include "Energy.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(TrafficGenerator);

void TrafficGenerator::initialize(int level) {
    TrafGen::initialize(level);
    if(level == 0) {
        NotificationBoard* notificationBoard = NotificationBoardAccess().get();
        notificationBoard->subscribe(this, NF_BATTERY_CHANGED);
        nrOfPacketsToSend = par("nrOfPackets").longValue();
        isEnergyDepleted = false;
        WATCH(nrOfSentMessages);
    }
}

void TrafficGenerator::handleSelfMsg(cMessage* message) {
    if (isEnergyDepleted == false) {
        TrafGen::handleSelfMsg(message);
    }
}

void TrafficGenerator::SendTraf(cPacket* message, const char* destination) {
    if(nrOfPacketsToSend > 0) {
        if(nrOfSentMessages < nrOfPacketsToSend) {
            sendTraffic(message, destination);
        }
        else {
            delete message;
        }
    }
    else {
        sendTraffic(message, destination);
    }
}

void TrafficGenerator::sendTraffic(cPacket* originalTrafGenMessage, const char* destination) {
    TrafficPacket* packet = new TrafficPacket("Traffic");
    SimTime now = simTime();
    packet->setCreationTime(now);
    long packetSizeInBytes = PacketSize();
    packet->setByteLength(packetSizeInBytes);

    TrafficControlInfo* controlInfo = new TrafficControlInfo();
    controlInfo->setDestination(destination);
    packet->setControlInfo(controlInfo);

    send(packet, "toLowerGate");
    nrOfSentMessages++;

    delete originalTrafGenMessage;
}

void TrafficGenerator::receiveChangeNotification(int category, const cObject* details) {
    if(category == NF_BATTERY_CHANGED) {
        Energy* energyInformation = check_and_cast<Energy*>(details);
        if (energyInformation->GetEnergy() <= 0) {
            EV << "TrafficGenerator detected that the battery is depleted and will stop sending traffic" << std::endl;
            isEnergyDepleted = true;
        }
    }
}

OMNETARA_NAMESPACE_END
