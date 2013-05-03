/*
 * $FU-Copyright$
 */

#include "NextHop.h"

using namespace std;

namespace ARA {

NextHop::NextHop(){

}

NextHop::NextHop(shared_ptr<Address> address, NetworkInterface* interface) {
    this->address = shared_ptr<Address>(address);
    this->interface = interface;
}

shared_ptr<Address> NextHop::getAddress() {
    return address;
}

NetworkInterface* NextHop::getInterface() {
    return interface;
}

} /* namespace ARA */
