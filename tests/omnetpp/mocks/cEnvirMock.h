/*
 * $FU-Copyright$
 */

#ifndef ENVIR_MOCK_H_
#define ENVIR_MOCK_H_

#include <omnetpp.h>

namespace ARA {
namespace test {
namespace omnetpp {

    class cEnvirMock : public cEnvir {
    protected:
        void sputn(const char *s, int n) {}
        void putsmsg(const char *msg) {}
        bool askyesno(const char *msg) { return false; }
        void flushLastLine() {}

    public:
        void objectDeleted(cObject *object) {}
        void componentInitBegin(cComponent *component, int stage) {}
        void simulationEvent(cMessage *msg) {}
        void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL) {}
        void messageScheduled(cMessage *msg) {}
        void messageCancelled(cMessage *msg) {}
        void beginSend(cMessage *msg) {}
        void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay) {}
        void messageSendHop(cMessage *msg, cGate *srcGate) {}
        void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay) {}
        void endSend(cMessage *msg) {}
        void messageCreated(cMessage *msg) {}
        void messageCloned(cMessage *msg, cMessage* clone) {}
        void messageDeleted(cMessage *msg) {}
        void moduleReparented(cModule *module, cModule *oldparent) {}
        void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent) {}
        void componentMethodEnd() {}
        void moduleCreated(cModule *newmodule) {}
        void moduleDeleted(cModule *module) {}
        void gateCreated(cGate *newgate) {}
        void gateDeleted(cGate *gate) {}
        void connectionCreated(cGate *srcgate) {}
        void connectionDeleted(cGate *srcgate) {}
        void displayStringChanged(cComponent *component) {}
        void undisposedObject(cObject *obj) {}
        void configure(cComponent *component) {}
        void readParameter(cPar *parameter) {}
        bool isModuleLocal(cModule *parentmod, const char *modname, int index) { return false; }
        cXMLElement *getXMLDocument(const char *filename, const char *xpath=NULL) { return nullptr; }
        cXMLElement *getParsedXMLString(const char *content, const char *xpath=NULL) { return nullptr; }
        void forgetXMLDocument(const char *filename) {}
        void forgetParsedXMLString(const char *content) {}
        void flushXMLDocumentCache() {}
        void flushXMLParsedContentCache() {}
        unsigned getExtraStackForEnvir() { return 0; };
        cConfiguration *getConfig() { return nullptr; }
        bool isGUI() const { return false; }
        void bubble(cComponent *component, const char *text) {}
        cEnvir& flush() { }
        std::string gets(const char *prompt, const char *defaultreply=NULL) { return nullptr; }
        int getNumRNGs() { return 0; }
        cRNG *getRNG(int k) { return nullptr; }
        void getRNGMappingFor(cComponent *component) {}
        void *registerOutputVector(const char *modulename, const char *vectorname) { return nullptr; }
        void deregisterOutputVector(void *vechandle) {}
        void setVectorAttribute(void *vechandle, const char *name, const char *value) {}
        bool recordInOutputVector(void *vechandle, simtime_t t, double value) { return false; }
        void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL) {}
        void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL) {}
        std::ostream *getStreamForSnapshot() { return nullptr; }
        void releaseStreamForSnapshot(std::ostream *os) {}
        int getArgCount() { return 0; }
        int getParsimProcId() { return 0; }
        int getParsimNumPartitions() { return 0; }
        unsigned long getUniqueNumber() { return counter++; }
        bool idle() { return false; }

        char **getArgVector() {
            return &argVector;
        }

    private:
        char* argVector = "foobar";
        unsigned long counter = 0;
    };

}}} // namespace ARA::test::omnetpp
#endif /* ENVIR_MOCK_H_ */
