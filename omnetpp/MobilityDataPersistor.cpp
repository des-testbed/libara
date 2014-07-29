/*
 * $FU-Copyright$
 */

#include <iostream>
#include "MobilityDataPersistor.h"

OMNETARA_NAMESPACE_BEGIN

using namespace std;

MobilityDataPersistor::MobilityDataPersistor(MobilityBase* mobility, cModule* hostModule) {
    string fileName = getFileName(hostModule);
    file.open(fileName.c_str(), ios::out | ios::binary | ios::trunc);

    if (file.is_open() == false) {
        throw cRuntimeError("Could not open file '%s', to write routing table data statistics", fileName.c_str());
    }

    this->signalName = std::string("mobilityStateChanged");
    mobility->subscribe("mobilityStateChanged", this);
}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, long l){
    throw("this method is not implemented since the mobility module does not create such a signal");
}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, double d){
    throw("this method is not implemented since the mobility module does not create such a signal");
}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, simtime_t t){
    throw("this method is not implemented since the mobility module does not create such a signal");
}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, const char *s){
    throw("this method is not implemented since the mobility module does not create such a signal");
}

void MobilityDataPersistor::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t){
    throw("this method is not implemented since the mobility module does not create such a signal");
}

void MobilityDataPersistor::receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l){
    throw("this method is not implemented since the mobility module does not create such a signal");
}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, cObject *obj){
    std::string signal(src->getSignalName(id));
    
    if (signalMatches(signal)) {
       MovingMobilityBase* mobility = check_and_cast<MovingMobilityBase*>(obj);
       this->write(mobility->getCurrentPosition());
    }
}

bool MobilityDataPersistor::signalMatches(std::string pSignalName){
    return (this->signalName == pSignalName);
}

MobilityDataPersistor::~MobilityDataPersistor() {
    file.close();
}

/**
 * The method determines the file name for the mobile trace. The file
 * name has the following schema:
 *
 *    <scenario>-<run>-<node_id>.mtr
 *
 * @param hostModule The host module which reprents the node in the current simulation context.
 */
string MobilityDataPersistor::getFileName(cModule* hostModule) const {
    stringstream fileName;
    cConfigurationEx* config = ev.getConfigEx();
    fileName << "results/" << config->getActiveConfigName() << "-" << config->getActiveRunNumber() << "-" << hostModule->getName() << hostModule->getIndex() << ".mtr";
    return fileName.str();
}

/**
 * Writes the current position of the node to a file
 *
 * The binary format of the written data is defined as follows:
 * | 8 byte timestamp | <entry_data> | ... | [<entry_data>]
 *
 * <entry_data> is defined as a triple in the following format:
 * | 8 byte x position | 8 byte y position | 8 byte z position |
 *
 * The format stores all <entry_data> triples directly one after another.
 *
 * @param position The position of the node
 */
void MobilityDataPersistor::write(Coord position) {
    OMNeTTime* currentTime = dynamic_cast<OMNeTTime*>(Environment::getClock()->makeTime());

    if (currentTime){
        currentTime->setToCurrentTime();
  
        int64 rawTime = currentTime->getRawTime();

        file.write((char*)&rawTime, sizeof(rawTime));
        file.write((char*)&position.x, 8);
        file.write((char*)&position.y, 8);
        file.write((char*)&position.z, 8);
    }
}

OMNETARA_NAMESPACE_END
