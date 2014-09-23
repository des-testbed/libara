/*
 * $FU-Copyright$
 */

#include "AckTimerData.h"

using namespace std;

ARA_NAMESPACE_BEGIN

AckTimerData::AckTimerData(std::shared_ptr<Address> address, std::shared_ptr<Packet> data, int retries){
    recipient = address;
    packet = data;
    nrOfRetries = retries;
}

AckTimerData::~AckTimerData(){

}

std::shared_ptr<Address> AckTimerData::getRecipient(){
    return recipient;
}

std::shared_ptr<const Packet> AckTimerData::getPacket(){
    return packet;
}

int AckTimerData::getNrOfRetires(){
    return nrOfRetries;
}

void AckTimerData::setNrOfRetires(int newNrOfRetries){
    nrOfRetries = newNrOfRetries;
}

ARA_NAMESPACE_END
