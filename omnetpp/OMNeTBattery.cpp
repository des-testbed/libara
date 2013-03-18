#include "omnetpp/OMNeTBattery.h"

namespace ARA {
    namespace omnetpp {
        Define_Module(OMNeTBattery);


        OMNeTBattery::OMNeTBattery(){

        }

        OMNeTBattery::~OMNeTBattery(){

        }


        void OMNeTBattery::initialize(int stage){
            BasicBattery::initialize(stage);

        }

        void OMNeTBattery::finish(){

        }

        void OMNeTBattery::handleMessage(cMessage *message){
            if (message->isSelfMessage()) {

            } else {

            }  
        }

        int OMNeTBattery::registerDevice(cObject *identifier, int numberOfAccounts){
            return InetSimpleBattery::registerDevice(identifier, numberOfAccounts);
        }

        void OMNeTBattery::registerWirelessDevice(int id, double usageRadioIdle, double usageRadioRecv,double usageRadioSend,double usageRadioSleep){

        }

    }
}
