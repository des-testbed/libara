/*
 * $FU-Copyright$
 */

#ifndef ADDRESSMOCK_H_
#define ADDRESSMOCK_H_

#include "Address.h"
#include <string>

namespace ARA {

class AddressMock: public Address {
public:
    AddressMock();
    AddressMock(std::string name);

    std::string toString() const;
    bool equals(const Address* otherAddress) const;
    bool equals(const std::shared_ptr<Address> otherAddress) const;
    size_t getHashValue() const;
    Address* clone();

    std::string getAddress();

private:
    std::string address;

};

} /* namespace ARA */
#endif /* ADDRESSMOCK_H_ */
