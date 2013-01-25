//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ARATrafficGenerator.h"
#include "IPAddressResolver.h"
#include "IPControlInfo.h"

using namespace ARA;

Define_Module(ARATrafficGenerator);

void ARATrafficGenerator::initialize() {
    protocolType = par("protocol");
    numberOfPackets = par("numberOfPackets");
    messageByteLength = par("packetLength");
    defaultBroadCastAddressInterface = par("defaultBroadCastAddressInterface");

    readDestinationAddresses(par("destAddresses"));

    if (numberOfPackets > 0) {
        EV << "Scheduling selftimer";
        cMessage* timer = new cMessage("sendTimer");
        scheduleAt(simTime(), timer);
    }
}

void ARATrafficGenerator::readDestinationAddresses(const char* destinationAddressesParameter) {
    cStringTokenizer tokenizer(destinationAddressesParameter);
    const char* token;
    while ((token = tokenizer.nextToken()) != NULL) {
        destinationAddresses.push_back(IPAddressResolver().resolve(token));
    }
}

void ARATrafficGenerator::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        sendPacket();

        if (numberOfPackets > 0) {
            scheduleAt(simTime()+(double)par("packetInterval"), msg);
        } else {
            delete msg;
        }
    }
}

void ARATrafficGenerator::sendPacket() {
    char msgName[32];
    sprintf(msgName,"appData-%d", numberOfSentPackets+1);

    cPacket *payload = new cPacket(msgName);
    payload->setByteLength(messageByteLength);

    IPvXAddress destAddr = chooseDestinationAddress();
    IPControlInfo* controlInfo = new IPControlInfo();
    controlInfo->setDestAddr(destAddr.get4());
    controlInfo->setProtocol(protocolType);
    if (controlInfo->getDestAddr()== IPAddress::ALLONES_ADDRESS) {
        controlInfo->setInterfaceId(defaultBroadCastAddressInterface);
    }
    payload->setControlInfo(controlInfo);

    EV << "Sending packet: ";
    printPacket(payload);

    send(payload, "out");

    numberOfSentPackets++;
}

IPvXAddress ARATrafficGenerator::chooseDestinationAddress() {
    int randomIndex = intrand(destinationAddresses.size());
    return destinationAddresses[randomIndex];
}

void ARATrafficGenerator::printPacket(cPacket *msg) {
    IPvXAddress src, dest;
    int protocol = -1;
    IPControlInfo* ctrl = (IPControlInfo*)msg->getControlInfo();
    src = ctrl->getSrcAddr();
    dest = ctrl->getDestAddr();
    protocol = ctrl->getProtocol();

    ev  << msg << endl;
    ev  << "Payload length: " << msg->getByteLength() << " bytes" << endl;
    if (protocol != -1) {
        ev  << "src: " << src << "  dest: " << dest << "  protocol=" << protocol << "\n";
    }
}
