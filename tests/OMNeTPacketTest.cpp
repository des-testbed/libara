/*
 * OMNeTPacketTest.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: Friedrich Große
 */

#include "OMNeTPacketTest.h"

namespace ARA {

void OMNeTPacketTest::setUp() {
    testObject = new OMNeTPacket();
}

void OMNeTPacketTest::tearDown() {
    delete testObject;
}

void OMNeTPacketTest::testGetAddress() {
    CPPUNIT_ASSERT(true);
}

} /* namespace ARA */
