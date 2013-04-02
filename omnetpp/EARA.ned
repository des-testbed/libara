@namespace(ARA::omnetpp);
package ara;

simple EARA extends ARA {    
    parameters:
        @class(EARA);
        int helloMessageTimeout @unit(ms) = default(1000ms);
}
