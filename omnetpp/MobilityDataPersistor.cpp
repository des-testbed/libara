/*
 * $FU-Copyright$
 */

#include "MobilityDataPersistor.h"
#include "Environment.h"

#include <unistd.h>
#include <stdio.h>
#include <endian.h>

#include "MovingMobilityBase.h"


OMNETARA_NAMESPACE_BEGIN

using namespace std;

//mobilityStateChanged

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

}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, double d){

}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, simtime_t t){

}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, const char *s){

}

void MobilityDataPersistor::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t){

}

void MobilityDataPersistor::receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l){

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

/**
 * The method unsubscribes the class from the 'mobilityStateChanged'
 * signal and is called via class 'AbstractOMNeTARAClient'.
 *
 * @param mobility The mobility module of a node 
 */
void MobilityDataPersistor::unsubscribe(MobilityBase *mobility) {
    mobility->unsubscribe("mobilityStateChanged", this);
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
 * Writes the current entries of the given Mobility to file if at least
 * n milliseconds have passed since the last write (where n is the updateIntervall
 * given the class constructor).
 *
 * The binary format of the written data is defined as follows:
 * | 8 Byte timestamp | 1 Byte nrOfEntries | <entry_data> | ... | [<entry_data>]
 *
 * <entry_data> is defined as a triple in the following format:
 * | 4 Byte x position | 4 Byte y position | 4 Byte z position |
 *
 * The format stores all <entry_data> triples directly one after another.
 */
void MobilityDataPersistor::write(Coord position) {
    std::cout << position.x << ", " << position.y << "," << position.z << " : " << sizeof(position.x) << std::endl;

}

OMNETARA_NAMESPACE_END
