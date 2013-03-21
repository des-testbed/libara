/*
 * $FU-Copyright$
 */

#include "omnetpp/ARA.h"
#include "omnetpp/OMNeTPacket.h"
#include "NotificationBoard.h"
#include "ModuleAccess.h"

namespace ARA {
    namespace omnetpp {
        typedef std::shared_ptr<Address> AddressPtr;

        // Register the class with the OMNeT++ simulation
        Define_Module(ARA);

        ARA::ARA() {
            hasEnoughBattery = true;
            messageDispatcher = new MessageDispatcher(this);
        }

        ARA::~ARA() {
            delete messageDispatcher;
        }

        int ARA::numInitStages() const {
            return 5;
        }

        /**
         * The method initializes the ARA class. Typically, this is
         * a task which would be provided by a constructor, but it is one of the
         * main concepts of OMNeT++ to provide such a method (and to leave
         * constructors 'untouched'). The method parses the parameters
         * specified in the NED file and initializes the gates.
         */
        void ARA::initialize(int stage) {
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

        void ARA::initializeNetworkInterfacesOf(OMNeTConfiguration& config) {
            cGate* gateToARP = gate("arpOut");

            int nrOfInterfaces = interfaceTable->getNumInterfaces();
            if(nrOfInterfaces > 2) { // loopback + 1 other NIC
                // TODO remove this constraint
                throw cRuntimeError("ARA does currently not implement handling of more than one network card.");
            }

            for (int i=0; i < nrOfInterfaces; i++)         {
                InterfaceEntry* interfaceEntry = interfaceTable->getInterface(i);
                if (interfaceEntry->isLoopback() == false) {
                    addNetworkInterface(new OMNeTGate(this, gateToARP, interfaceEntry, config.getBroadCastDelay(), config.getUniCastDelay()));
                }
            }
        }

        void ARA::handleMessage(cMessage* message) {
            if (hasEnoughBattery) {
                messageDispatcher->dispatch(message);
            }
        }

        void ARA::deliverToSystem(const Packet* packet) {
            Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
            OMNeTPacket* omnetPacket = dynamic_cast<OMNeTPacket*>(pckt);
            ASSERT(omnetPacket);

            cPacket* encapsulatedData = omnetPacket->decapsulate();
            send(encapsulatedData, "upperLayerGate$o");
        }

        void ARA::takeAndSend(cMessage* msg, cGate* gate, double sendDelay) {
            Enter_Method_Silent("takeAndSend(msg)");
            take(msg);
            sendDelayed(msg, sendDelay, gate);
        }

        void ARA::packetNotDeliverable(const Packet* packet) {
            //TODO report to upper layer
        }

        void ARA::receiveChangeNotification(int category, const cObject* details) {
            if(category == NF_LINK_BREAK) {
                handleLinkBreak(check_and_cast<Ieee80211DataOrMgmtFrame*>(details));
            }
            else if(category == NF_BATTERY_CHANGED) {
                handleBatteryStatusChange(check_and_cast<Energy*>(details));
            }
        }

        void ARA::handleLinkBreak(Ieee80211DataOrMgmtFrame* frame) {
            cPacket* encapsulatedPacket = frame->decapsulate();

            if(messageDispatcher->isARAMessage(encapsulatedPacket)) {
                // extract the receiver address
                MACAddress receiverMACAddress = frame->getReceiverAddress();
                const IPAddress receiverIPAddress = arp->getInverseAddressResolution(receiverMACAddress);
                AddressPtr omnetAddress (new OMNeTAddress(receiverIPAddress));

                OMNeTPacket* omnetPacket = check_and_cast<OMNeTPacket*>(encapsulatedPacket);

                // TODO this does only work if we have only one network interface card
                NetworkInterface* interface = getNetworkInterface(0);
                handleRouteFailure(omnetPacket, omnetAddress, interface);
            }
        }

        void ARA::handleBatteryStatusChange(Energy* energyInformation) {
            if (energyInformation->GetEnergy() <= 0) {
               hasEnoughBattery = false;

               // change the node color
               cDisplayString& displayString = getParentModule()->getParentModule()->getDisplayString();
               displayString.setTagArg("i", 1, "#FF0000");
            }
        }

    } /* namespace omnetpp */
} /* namespace ARA */
