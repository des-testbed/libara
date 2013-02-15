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

            virtual Time operator-(const Time& right);
            virtual Time operator-=(const Time& right);

            virtual int toSeconds();
            virtual long int toMilliseconds();

            virtual void update();
            virtual void update(TimeMock t);

            virtual bool isInitialized();
            virtual void initialize();
            
            virtual Time getTimestamp() const;

        private:
            Time* timestamp;
    };
}

#endif
