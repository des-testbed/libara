#ifndef TIME_MOCK_H_
#define TIME_MOCK_H_

#include "Time.h"

namespace ARA {
    class TimeMock : public Time {
        public:
            TimeMock();
            TimeMock(long seconds, long int milliseconds);

            virtual void setToCurrentTime();
            virtual Time* subtract(const Time* right) const;
            virtual long getSeconds() const;
            virtual long getMilliSeconds() const;

            void letTimePass(long milliSeconds);

        private:
            long seconds;
            long milliseconds;
    };
}

#endif
