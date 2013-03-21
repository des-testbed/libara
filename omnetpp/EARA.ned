@namespace(ARA::omnetpp);
package ara;

simple EARA extends ARA {    
    parameters:
        @class(EARA);
        double helloMessageTimeout @unit(s) = default(1s);
}
