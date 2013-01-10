/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

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
    
    enum PacketTypeEnum {
        DATA,
        FANT,
        BANT,
        PANT,
        DUPLICATE_ERROR
    };

    static bool isAntPacket(char type);
    static bool isDataPacket(char type);

    static std::string getAsString(char type) {
        switch (type) {
            case PacketType::FANT: return "FANT";
            case PacketType::BANT: return "BANT";
            case PacketType::PANT: return "PANT";
            case PacketType::DATA: return "DATA";
            case PacketType::DUPLICATE_ERROR: return "DUPLICATE_ERROR";
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
