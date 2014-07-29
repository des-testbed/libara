/*
 * $FU-Copyright$
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

//TODO: this class could inherit from std::exception

namespace ARA {
    class Exception {
        public:
            Exception(const char* message);
            const char* getMessage();

        private:
            const char* message;
    };
} /* namespace ARA */
#endif /* EXCEPTION_H_ */
