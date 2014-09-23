/*
 * $FU-Copyright$
 */

#ifndef ACK_TIMER_DATA_H_
#define ACK_TIMER_DATA_H_

#include "Packet.h"
#include "Address.h"

#include <memory>

namespace ARA {
    class AckTimerData {
        public:
            AckTimerData(std::shared_ptr<Address> address, std::shared_ptr<Packet> packet, int retries);
            virtual ~AckTimerData();

            virtual std::shared_ptr<Address> getRecipient();
            virtual std::shared_ptr<const Packet> getPacket();

            virtual int getNrOfRetires();
            virtual void setNrOfRetires(int newNrOfRetries);

        private:
            int nrOfRetries;
            std::shared_ptr<Address> recipient;
            std::shared_ptr<const Packet> packet;
    };
} /* namespace ARA */
#endif /* ACK_TIMER_DATA_H_ */
