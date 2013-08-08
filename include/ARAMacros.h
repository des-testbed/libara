/*
 * $FU-Copyright$
 */

#ifndef ARA_MACROS_H_
#define ARA_MACROS_H_

#include <memory>

// define a macro for the namespace declaration
#define ARA_NAMESPACE_BEGIN namespace ARA {
#define ARA_NAMESPACE_END }

ARA_NAMESPACE_BEGIN
class Timer;
class Address;

typedef std::shared_ptr<Address> AddressPtr;
typedef std::shared_ptr<Timer> TimerPtr;
ARA_NAMESPACE_END

#define DELETE_IF_NOT_NULL(someThing) if(someThing!=nullptr) delete someThing;

#endif
