/*
 * $FU-Copyright$
 */

#ifndef TIME_MOCK_H_
#define TIME_MOCK_H_

#include "Time.h"

namespace ARA {
    class TimeMock : public Time {
        public:
            TimeMock(long seconds=0, long int milliseconds=0);

            virtual void setToCurrentTime();
            virtual long getDifferenceInMilliSeconds(const Time* right) const;

            long getSeconds() const;
            long getMilliSeconds() const;

            static TimeMock currentTime;
            static void setCurrentTime(long seconds, long int milliseconds) {
                currentTime.seconds = 0;
                currentTime.milliseconds = 0;
            }

            static void letTimePass(long milliSeconds) {
                long passedSeconds = (currentTime.milliseconds + milliSeconds)/1000;
                long passedMilliseconds = milliSeconds-passedSeconds*1000;

                currentTime.seconds += passedSeconds;
                currentTime.milliseconds += passedMilliseconds;
            }

        private:
            long seconds;
            long milliseconds;

    };
}

#endif
