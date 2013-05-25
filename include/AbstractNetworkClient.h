/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_NETWORK_CLIENT_H_
#define ABSTRACT_NETWORK_CLIENT_H_

#include "ARAMacros.h"
#include "NetworkInterface.h"
#include "PacketFactory.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "Configuration.h"
#include "Packet.h"
#include "Logger.h"

#include <string>
#include <deque>

ARA_NAMESPACE_BEGIN

/**
 * The AbstractNetworkClient bundles general functionality for any Ad-Hoc network client.
 * It contains a variable number of NetworkInterfaces, a RoutingTable, a PacketTrap, a PacketFactory
 * and a Logger.
 *
 * There is not much logic implemented in this class, because this does only serve as base class to
 * more sophisticated network clients which route packets through the network.
 */
class AbstractNetworkClient {

public:
    /**
     * The standard virtual destructor of this abstract class.
     */
    virtual ~AbstractNetworkClient();

    /**
     * This method will initialize this client with the given configuration, ~RoutingTable and ~PacketFactory.
     * It must be called before any call to AbstractNetworkClient::sendPacket or
     * AbstractNetworkClient::receivePacket. If this object has been created by the
     * standard constructor this method must be called manually.
     */
    virtual void initialize(Configuration& configuration, RoutingTable *routingTable, PacketFactory* packetFactory) = 0;

    /**
     * Somehow sends the packet to the packets destination.
     */
    virtual void sendPacket(Packet* packet) = 0;

    /**
     * Receives and processes a Packet.
     */
    virtual void receivePacket(Packet* packet, NetworkInterface* interface) = 0;

    /**
     * Sets a logger for this network client.
     *
     * This logger will be used to log messages during the routing algorithm.
     * It will be deleted in the destructor of this client.
     */
    void setLogger(Logger* logger);

    /**
     * Registers a new NetworkInterface at this client.
     */
    void addNetworkInterface(NetworkInterface* newInterface);

    /**
     * Returns a specific NetworkInterface by index.
     */
    NetworkInterface* getNetworkInterface(unsigned int index);

    /**
     * Returns the amount of registered network interfaces of this client.
     */
    unsigned int getNumberOfNetworkInterfaces();

    /**
     * Returns the packet factory of this node.
     */
    PacketFactory* getPacketFactory() const;

    /**
     * Assigns a new RoutingTable object to this NetworkClient
     */
    void setRoutingTable(RoutingTable *routingTable);

    /**
     * Sends the given packet over all available interfaces of this node.
     * The sender is automatically set to the corresponding interface address.
     * The previous hop will be the original sender of the packet.
     */
    void broadCast(Packet* packet);

    /**
     * Returns the next sequence number and advances the sequence number counter by one.
     */
    unsigned int getNextSequenceNumber();

protected:

    /**
     * Checks if a logger has been assigned to this ARA client and if so
     * delegates the call to it with Logger::LEVEL_TRACE.
     *
     * The optional varargs are handled as parameters to the logMessage string.
     * If no logger has been set via setLogger() nothing happens.
     *
     * @see AbstractARAClient::logTrace
     * @see AbstractARAClient::logDebug
     * @see AbstractARAClient::logInfo
     * @see AbstractARAClient::logWarn
     * @see AbstractARAClient::logError
     * @see AbstractARAClient::logFatal
     */
    void logMessage(const std::string &logMessage, Logger::Level level, ...) const;

    /**
     * Logs with trace level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logTrace(const std::string &logMessage, ...) const;

    /**
     * Logs with debug level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logDebug(const std::string &logMessage, ...) const;

    /**
     * Logs with info level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logInfo(const std::string &logMessage, ...) const;

    /**
     * Logs with warn level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logWarn(const std::string &logMessage, ...) const;

    /**
     * Logs with error level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logError(const std::string &logMessage, ...) const;

    /**
     * Logs with fatal level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logFatal(const std::string &logMessage, ...) const;

    /**
     * Checks if the destination of the given packet equals the local address
     * of one of this nodes interfaces.
     */
    bool isDirectedToThisNode(const Packet* packet) const;

    /**
     * Checks if the source of the given packet equals the local address
     * of one of this nodes interfaces.
     */
    bool hasBeenSentByThisNode(const Packet* packet) const;

    /**
     * Checks if the given address equals the local address of one of
     * this nodes interfaces.
     */
    bool isLocalAddress(AddressPtr address) const;

protected:
    std::deque<NetworkInterface*> interfaces;
    PacketFactory* packetFactory;
    RoutingTable* routingTable;
    PacketTrap* packetTrap;

private:
    Logger* logger = nullptr;
    unsigned int nextSequenceNumber = 1;
};

ARA_NAMESPACE_END

#endif /* ABSTRACT_NETWORK_CLIENT_H_ */
