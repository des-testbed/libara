/*
 * $FU-Copyright$
 */

#ifndef PACKETTYPE_H_
#define PACKETTYPE_H_

#include <string>

namespace ARA {

/**
 * The PacketType is an abstract class that encapsulates the PacketTypeEnum and
 * some utility functions for checking the packet type.
 *
 * The enum can be used like this:
 * <code>int i = PacketType::FANT</code>
 */
class PacketType {
public:
    virtual ~PacketType() {}
    
    //TODO move all the PacketType class code into the Enum and then rename the enum

    enum PacketTypeEnum {
        DATA,
        FANT,
        BANT,
        PANT,
        DUPLICATE_ERROR,
        ACK,
        ROUTE_FAILURE,
        HELLO
    };

    static bool isAntPacket(char type);
    static bool isDataPacket(char type);

    static std::string getAsString(char type) {
        switch (type) {
            case PacketType::FANT: return "FANT";
            case PacketType::BANT: return "BANT";
            case PacketType::PANT: return "PANT";
            case PacketType::DATA: return "DATA";
            case PacketType::ACK: return "ACK";
            case PacketType::DUPLICATE_ERROR: return "DUPLICATE_ERROR";
            case PacketType::ROUTE_FAILURE: return "ROUTE_FAILURE";
            case PacketType::HELLO: return "HELLO";
            default: return "UNKOWN";
        }
    }
};

/**
 * Returns TRUE if the given type is a FANT, BANT or PANT and FALSE otherwise.
 */
inline bool PacketType::isAntPacket(char type) {
    switch (type) {
        case PacketType::FANT:
        case PacketType::BANT:
        case PacketType::PANT:
            return true;
        default:
            return false;
    }
}

/**
 * Returns TRUE if the given type is a DATA packet and FALSE otherwise.
 */
inline bool PacketType::isDataPacket(char type) {
    switch (type) {
        case PacketType::DATA:
            return true;
        default:
            return false;
    }
}

} /* namespace ARA */
#endif // PACKETTYPE_H_
