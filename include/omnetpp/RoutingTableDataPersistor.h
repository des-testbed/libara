/*
 * $FU-Copyright$
 */

#ifndef ROUTINGTABLE_DATA_PERSISTOR_H_
#define ROUTINGTABLE_DATA_PERSISTOR_H_

#include "OMNeTARAMacros.h"
#include "OMNeTTime.h"
#include "RoutingTable.h"

#include <fstream>
#include <string>

OMNETARA_NAMESPACE_BEGIN

class RoutingTableDataPersistor {
    public:
        RoutingTableDataPersistor(cModule* hostModule, int updateIntervallInMillis);
        ~RoutingTableDataPersistor();

        void write(RoutingTable* routingTable);

    private:
        std::string getFileName(cModule* hostModule) const;

        long updateIntervall;
        OMNeTTime* lastWriteTime = nullptr;
        std::ofstream file;

};

OMNETARA_NAMESPACE_END

#endif /* ROUTINGTABLE_DATA_PERSISTOR_H_ */
