/*
 * $FU-Copyright$
 */

#include "MobilityDataPersistor.h"
#include "Environment.h"

#include <unistd.h>
#include <stdio.h>
#include <endian.h>

OMNETARA_NAMESPACE_BEGIN

using namespace std;

//mobilityStateChanged

MobilityDataPersistor::MobilityDataPersistor(cModule* hostModule, std::string signalName) {
    string fileName = getFileName(hostModule);
    file.open(fileName.c_str(), ios::out | ios::binary | ios::trunc);

    if (file.is_open() == false) {
        throw cRuntimeError("Could not open file '%s', to write routing table data statistics", fileName.c_str());
    }

    this->signalName = signalName;
}

void MobilityDataPersistor::receiveSignal(cComponent *src, simsignal_t id, cObject *obj){
    std::string signal(src->getSignalName(id));
    
    if(signalMatches(signal)){
      std::cout << "received mobility signal" << std::endl;
    }
}

bool MobilityDataPersistor::signalMatches(std::string pSignalName){
    return (this->signalName == pSignalName);
}

MobilityDataPersistor::~MobilityDataPersistor() {
    file.close();
}

string MobilityDataPersistor::getFileName(cModule* hostModule) const {
    stringstream fileName;
    cConfigurationEx* config = ev.getConfigEx();
    fileName << "results/" << config->getActiveConfigName() << "-" << config->getActiveRunNumber() << "-" << hostModule->getName() << hostModule->getIndex() << ".md";
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
 * | 4 Byte destination | 4 Byte nextHop | 4 Byte pheromone value |
 *
 * The format stores all <entry_data> triples directly one after another.
 */
/*
void MobilityDataPersistor::write(Mobility* routingTable) {
    OMNeTTime* currentTime = dynamic_cast<OMNeTTime*>(Environment::getClock()->makeTime());
    currentTime->setToCurrentTime();

    if(lastWriteTime == nullptr
    || currentTime->getDifferenceInMilliSeconds(lastWriteTime) >= updateIntervall) {

        int64 rawTime = currentTime->getRawTime();
        file.write((char*)&rawTime, sizeof(rawTime));

        int nrOfEntries = routingTable->getTotalNumberOfEntries();
        file.write((char*)&nrOfEntries, 1);

        for (int i = 0; i < nrOfEntries; i++) {
            MobilityEntryTupel entryTupel = routingTable->getEntryAt(i); // FIXME getEntryAt is very inefficient. This function is called very often so it better use something with better performance
            OMNeTAddress* destination = dynamic_cast<OMNeTAddress*>(entryTupel.destination.get());
            uint32 destinationInt = destination->getInt();

            MobilityEntry* entry = entryTupel.entry;
            OMNeTAddress* nextHop = dynamic_cast<OMNeTAddress*>(entry->getAddress().get());
            uint32 nextHopInt = nextHop->getInt();

            float pheromoneValue = entry->getPheromoneValue();

            file.write((char*)&destinationInt, 4);
            file.write((char*)&nextHopInt, 4);
            file.write((char*)&pheromoneValue, sizeof(pheromoneValue));
        }

        delete lastWriteTime;
        lastWriteTime = currentTime;
    }
}
*/

OMNETARA_NAMESPACE_END
