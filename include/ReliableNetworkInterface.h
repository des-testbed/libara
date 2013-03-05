/*
 * $FU-Copyright$
 */

#ifndef RELIABLE_NETWORK_INTERFACE_H_
#define RELIABLE_NETWORK_INTERFACE_H_

#include "AbstractNetworkInterface.h"
#include "AbstractARAClient.h"

#include <memory>

namespace ARA {

    /**
     * TODO write class description
     */
    class ReliableNetworkInterface : public AbstractNetworkInterface {
        public:
            /**
             * Creates a new ReliableNetworkInterface.
             *
             * @param client the AbstractARAClient to which all received packets are route and which is informed if a packet is not deliverable.
             * @param localAddress the Address which is returned when the interface is asked for its local address.
             * @param broadcastAddress the Address which is known to be the broadcast address of this interface.
             */
            ReliableNetworkInterface(AbstractARAClient* client, std::shared_ptr<Address> localAddress = nullptr, std::shared_ptr<Address> broadcastAddress = nullptr);
            virtual ~ReliableNetworkInterface() {}

    };

} /* namespace ARA */
#endif // ABSTRACT_NETWORK_INTERFACE_H_
