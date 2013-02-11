#ifndef TIME_MOCK_H_
#define TIME_MOCK_H_

#include "Time.h"

namespace ARA {
    class TimeMock : public Time {
        public:
            TimeMock();
            TimeMock(struct timeval* timestamp);

            int usleep(int seconds);

            TimeMock operator-(const TimeMock& right);
            TimeMock operator-=(const TimeMock& right);

            int toSeconds();
            //virtual long int toMilliseconds();
/*
   
            virtual ~TimeMock();

            int getTime();
            void setTime();
 



            virtual struct timeval* getTimestamp() const;

            virtual bool isInitialized();

            virtual void initialize();
*/
    };
}

#endif
