/*
 * $FU-Copyright$
 */

#include "Environment.h"
#include "UnixClock.h"

namespace ARA {

Environment* Environment::instance = nullptr;

Environment::Environment() {
    clock = new UnixClock();
    packetFactory = new PacketFactory();
}

Environment::~Environment() {
    delete clock;
    delete packetFactory;
}

void Environment::setTheClock(Clock* newClock) {
    delete clock;
    clock = newClock;
}

void Environment::setThePacketFactory(PacketFactory* newFactory) {
    delete packetFactory;
    packetFactory = newFactory;
}

Environment& Environment::getInstance() {
    if (instance == nullptr) {
       instance = new Environment();
    }
    return *instance;
}

Clock* Environment::getClock() {
    return getInstance().clock;
}

void Environment::setClock(Clock* newClock) {
    getInstance().setTheClock(newClock);
}

void Environment::setPacketFactory(PacketFactory* newFactory) {
    getInstance().setThePacketFactory(newFactory);
}

PacketFactory* Environment::getPacketFactory() {
    return getInstance().packetFactory;
}

} /* namespace ARA */
