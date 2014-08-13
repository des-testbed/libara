/*
 * $FU-Copyright$
 */

#ifndef _STANDARD_TIMER_PROXY_H_
#define _STANDARD_TIMER_PROXY_H_

#include "ARAMacros.h"
#include "Timer.h"
#include "Environment.h"
#include "StandardClock.h"


ARA_NAMESPACE_BEGIN

class StandardTimerProxy : public Timer {
    public:
        StandardTimerProxy(char type, void* contextObject=nullptr);
        virtual ~StandardTimerProxy();

        virtual void run(unsigned long timeoutInMicroSeconds);
        virtual void interrupt();

        bool equals(const Timer* otherTimer) const;
        bool equals(const std::shared_ptr<Timer> otherTimer) const;

        size_t getHashValue() const;

	void setTimerIdentifier(unsigned long identifier);

	void notify();

    private:
        unsigned long timerIdentifier = -1;
};

ARA_NAMESPACE_END

#endif
