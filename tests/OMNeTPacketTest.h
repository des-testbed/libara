#ifndef OMNETPACKETTEST_H_
#define OMNETPACKETTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "OMNeTPacket.h"

using namespace ARA;

namespace ARA {

class OMNeTPacketTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(OMNeTPacketTest);
    CPPUNIT_TEST(testGetAddress);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testGetAddress();

private:
    OMNeTPacket *testObject;

};

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
#endif /* OMNETPACKETTEST_H_ */
