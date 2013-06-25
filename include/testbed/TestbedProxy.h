/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_PROXY_H_
#define _TESTBED_PROXY_H_

#include "CLibs.h"

template <int priority>
class TestbedProxy {
    public:
        static S& getInstance() {
            static TestbedProxy<priority> instance;
	    return instance;
        }


        static handleEventGlobal(dessert_msg_t* msg, size_t len, dessert_msg_proc_t* proc, dessert_sysif_t* tunif, dessert_frameid_t id)
//            getInstance().handleEvent(msg, len, proc, tunif, id);
        }

        //static registerHandler(Handler* handler){
        static registerHandler(){
            if(getInstance().registered()){
                dessert_sysrxcb_add(&TestbedProxy<priority>::handleEventGlobal, priority);
                getInstance().setRegisteredStatus(true);
            }
        }

	bool setRegisteredStatus(bool status){
	    registered = status;
	}

	bool registered(){
            return registered;
        }

    private:
        TestbedProxy():registered(false){};
	
	TestbedProxy(TestbedProxy const&);
	void operator=(Testbed const&);

	bool registered;
};

#endif 
