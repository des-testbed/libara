/*
 * $FU-Copyright$
 */

#include "RoutingTableDataPersistor.h"
#include "Environment.h"

#include <unistd.h>
#include <stdio.h>
#include <endian.h>

OMNETARA_NAMESPACE_BEGIN

using namespace std;

RoutingTableDataPersistor::RoutingTableDataPersistor(cModule* hostModule, int updateIntervallInMillis) {
    updateIntervall = updateIntervallInMillis;
    const char* fileName = getFileName(hostModule);
    file.open(fileName, ios::out | ios::binary);
    if(!file.good()) {
        throw cRuntimeError("could not open file %s, to write routing table data", fileName);
    }
}

RoutingTableDataPersistor::~RoutingTableDataPersistor() {
    if(lastWriteTime != nullptr) {
        delete lastWriteTime;
    }

    file.flush();
    file.close();
}

const char* RoutingTableDataPersistor::getFileName(cModule* hostModule) const {
    stringstream fileName;
    cConfigurationEx* config = ev.getConfigEx();
    fileName << "results/" << config->getActiveConfigName() << "-" << config->getActiveRunNumber() << "-" << hostModule->getName() << hostModule->getIndex() << "-RoutingTable.bin";
    return fileName.str().c_str();
}

/**
 * Writes the current entries of the given RoutingTable to file if at least
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
 *
 * Note that all binary data is stored in big endian format
 */
void RoutingTableDataPersistor::write(RoutingTable* routingTable) {
    OMNeTTime* currentTime = dynamic_cast<OMNeTTime*>(Environment::getClock()->makeTime());
    currentTime->setToCurrentTime();

    if(lastWriteTime == nullptr
    || currentTime->getDifferenceInMilliSeconds(lastWriteTime) >= updateIntervall) {

        int64 rawTime = htobe64(currentTime->getRawTime());
        file.write((char*)&rawTime, sizeof(rawTime));

        int nrOfEntries = htole32(routingTable->getTotalNumberOfEntries());
        file.write((char*)&nrOfEntries, 1);

        for (int i = 0; i < nrOfEntries; i++) {
            RoutingTableEntryTupel entryTupel = routingTable->getEntryAt(i); // FIXME getEntryAt is very inefficient. This function is called very often so it better use something with better performance
            OMNeTAddress* destination = dynamic_cast<OMNeTAddress*>(entryTupel.destination.get());
            uint32 destinationInt = htobe32(destination->getInt());

            RoutingTableEntry* entry = entryTupel.entry;
            OMNeTAddress* nextHop = dynamic_cast<OMNeTAddress*>(entry->getAddress().get());
            uint32 nextHopInt = htobe32(nextHop->getInt());

            float pheromoneValue = entry->getPheromoneValue();

            file.write((char*)&destinationInt, 4);
            file.write((char*)&nextHopInt, 4);
            file.write((char*)&pheromoneValue, sizeof(pheromoneValue));
        }

        // flush to file in case the simulation crashes or anything happens
        file.flush();

        delete lastWriteTime;
        lastWriteTime = currentTime;
    }
}

OMNETARA_NAMESPACE_END
