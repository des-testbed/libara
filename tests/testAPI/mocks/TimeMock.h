#ifndef TIME_MOCK_H_
#define TIME_MOCK_H_

#include "Time.h"

namespace ARA {
    class TimeMock : public Time {
        public:
            TimeMock();
            TimeMock(Time* time);
            ~TimeMock();

            void usleep(int seconds);

            TimeMock operator-(const TimeMock& right);
            TimeMock operator-=(const TimeMock& right);

            int toSeconds();
            long int toMilliseconds();

            void update();
            void update(TimeMock t);

            bool isInitialized();
            void initialize();
            
            Time getTimestamp() const;

        private:
            Time* timestamp;
    };
}

#endif
