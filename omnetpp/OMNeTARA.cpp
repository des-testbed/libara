/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTARA.h"
#include "omnetpp/OMNeTPacket.h"
#include "NotificationBoard.h"
#include "ModuleAccess.h"
#include "Ieee80211Frame_m.h"

namespace ARA {
    namespace omnetpp {
        typedef std::shared_ptr<Address> AddressPtr;

        // Register the class with the OMNeT++ simulation
        Define_Module(OMNeTARA);

        OMNeTARA::OMNeTARA() {
            hasEnoughBattery = true;
            messageDispatcher = new MessageDispatcher(this);
        }

        OMNeTARA::~OMNeTARA() {
            delete messageDispatcher;
        }

        int OMNeTARA::numInitStages() const {
            return 5;
        }

        /**
         * The method initializes the ARA class. Typically, this is
         * a task which would be provided by a constructor, but it is one of the
         * main concepts of OMNeT++ to provide such a method (and to leave
         * constructors 'untouched'). The method parses the parameters
         * specified in the NED file and initializes the gates.
         */
        void OMNeTARA::initialize(int stage) {
            if(stage == 4) {
                NotificationBoard* notificationBoard = NotificationBoardAccess().get();
                notificationBoard->subscribe(this, NF_LINK_BREAK);
                notificationBoard->subscribe(this, NF_BATTERY_CHANGED);

                OMNeTConfiguration config = OMNeTConfiguration(this);
                setLogger(config.getLogger());

                interfaceTable = ModuleAccess<IInterfaceTable>("interfaceTable").get();
                arp = ModuleAccess<ARP>("arp").get();
                routingTable = ModuleAccess<RoutingTable>("routingTableStatistics").get();
                routingTable->setEvaporationPolicy(evaporationPolicy);

                AbstractARAClient::initialize(config, routingTable);
                initializeNetworkInterfacesOf(config);
                WATCH(hasEnoughBattery);
            }
        }

        void OMNeTARA::initializeNetworkInterfacesOf(OMNeTConfiguration& config) {
            cGate* gateToARP = gate("arpOut");

            int nrOfInterfaces = interfaceTable->getNumInterfaces();
            for (int i=0; i < nrOfInterfaces; i++)         {
                InterfaceEntry* interfaceEntry = interfaceTable->getInterface(i);
                if (interfaceEntry->isLoopback() == false) {
                    addNetworkInterface(new OMNeTGate(this, gateToARP, interfaceEntry, config.getBroadCastDelay(), config.getUniCastDelay()));
                }
            }
        }

        void OMNeTARA::handleMessage(cMessage* message) {
            if (hasEnoughBattery) {
                messageDispatcher->dispatch(message);
            }
        }

        void OMNeTARA::deliverToSystem(const Packet* packet) {
            Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
            OMNeTPacket* omnetPacket = dynamic_cast<OMNeTPacket*>(pckt);
            ASSERT(omnetPacket);

            cPacket* encapsulatedData = omnetPacket->decapsulate();
            send(encapsulatedData, "upperLayerGate$o");
        }

        void OMNeTARA::takeAndSend(cMessage* msg, cGate* gate, double sendDelay) {
            Enter_Method_Silent("takeAndSend(msg)");
            take(msg);
            sendDelayed(msg, sendDelay, gate);
        }

        void OMNeTARA::packetNotDeliverable(const Packet* packet) {
            //TODO report to upper layer
        }

        void OMNeTARA::receiveChangeNotification(int category, const cObject* details) {
            EV << " category is " << category << "\n";
            if(category == NF_LINK_BREAK) {
                EV << " category is NF_LINK_BREAK" << "\n";
                Ieee80211DataOrMgmtFrame* frame = (Ieee80211DataOrMgmtFrame*) details;
                cPacket* encapsulatedPacket = frame->decapsulate();

                if(messageDispatcher->isARAMessage(encapsulatedPacket)) {
                    // extract the receiver address
                    MACAddress receiverMACAddress = frame->getReceiverAddress();
                    const IPAddress receiverIPAddress = arp->getInverseAddressResolution(receiverMACAddress);
                    AddressPtr omnetAddress (new OMNeTAddress(receiverIPAddress));

                    OMNeTPacket* omnetPacket = check_and_cast<OMNeTPacket*>(encapsulatedPacket);

                    //TODO somehow get the NetworkInterface*
                    //handleRouteFailure(omnetPacket, omnetAddress, interface);
                }
                EV << "\nFOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n";
            }

            if(category == NF_BATTERY_CHANGED) {
                EV << " category is NF_BATTERY_CHANGED" << "\n";
                Energy *energy = (Energy*) details;
                double currentEnergyLevel = energy->GetEnergy();

                if(currentEnergyLevel <= 0) {
                   /// deactivate the node
                   hasEnoughBattery = false;
                   /// draw the node in a different color
            //       cDisplayString& displayString = getParentModule()->getDisplayString(); 
             //      displayString.parse("i=device/wifilaptop,red,80;bgb=366,335");
                } else {
                   /// set the energy value for the hello messages

                }
            }
        }

    } /* namespace omnetpp */
} /* namespace ARA */
