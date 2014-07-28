/*
 * $FU-Copyright$
 */

#ifndef NEXTHOP_H_
#define NEXTHOP_H_

#include <memory>
#include "Address.h"
#include "NetworkInterface.h"

namespace ARA {
    class NextHop {
        public:
            NextHop(std::shared_ptr<Address> address, NetworkInterface* interface);
            std::shared_ptr<Address> getAddress();
            NetworkInterface* getInterface();

        private:
            std::shared_ptr<Address> address;
            NetworkInterface* interface;
    };
} /* namespace ARA */
#endif /* NEXTHOP_H_ */
