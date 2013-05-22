/*
 * $FU-Copyright$
 */

#include "RoutingTableDataPersistor.h".h"
#include "Environment.h"

OMNETARA_NAMESPACE_BEGIN

using namespace std;

RoutingTableDataPersistor::RoutingTableDataPersistor(cModule* hostModule, int updateIntervallInMillis) {
    const char* fileName = getFileName(hostModule);
    updateIntervall = updateIntervallInMillis;
    file.open(fileName, ios::in | ios::out | ios::binary);
}

RoutingTableDataPersistor::~RoutingTableDataPersistor() {
    file.close();
}

const char* RoutingTableDataPersistor::getFileName(cModule* hostModule) const {
    stringstream fileName;
    fileName << hostModule->getFullName() << "-RoutingTable.bin";
    return fileName.str().c_str();
}

void RoutingTableDataPersistor::write(RoutingTable* routingTable) {
    OMNeTTime* currentTime = dynamic_cast<OMNeTTime*>(Environment::getClock()->makeTime());
    currentTime->setToCurrentTime();

    if(lastWriteTime == nullptr
    || lastWriteTime->getDifferenceInMilliSeconds(currentTime) >= updateIntervall) {

        char nullByte = 0x00;
        int64 rawTime = currentTime->getRawTime();
        float pheromoneValue = 123.456;

        file.write((char*)&rawTime, sizeof(rawTime));
        int nrOfEntries = routingTable->getTotalNumberOfEntries();
        for (int i = 0; i < nrOfEntries; i++) {
            RoutingTableEntryTupel entryTupel = routingTable->getEntryAt(i);
            AddressPtr destination = entryTupel.destination;
            RoutingTableEntry* entry = entryTupel.entry;

            //TODO write dest
            //TODO write next
            file.write((char*)&pheromoneValue, sizeof(pheromoneValue));
        }
        file.write((char*)&nullByte, 1);

        lastWriteTime = currentTime;
    }
}

OMNETARA_NAMESPACE_END
