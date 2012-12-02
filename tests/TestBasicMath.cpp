#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "../src/CBasicMath.hpp"

using namespace CppUnit;
using namespace std;

//-----------------------------------------------------------------------------

class TestBasicMath : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestBasicMath);
    CPPUNIT_TEST(testAddition);
    CPPUNIT_TEST(testMultiply);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testAddition(void);
    void testMultiply(void);

private:

    CBasicMath *mTestObj;
};

//-----------------------------------------------------------------------------

void
TestBasicMath::testAddition(void)
{
    CPPUNIT_ASSERT(5 == mTestObj->Addition(2,3));
}

void
TestBasicMath::testMultiply(void)
{
    CPPUNIT_ASSERT(6 == mTestObj->Multiply(2,3));
}

void TestBasicMath::setUp(void)
{
    mTestObj = new CBasicMath();
}

void TestBasicMath::tearDown(void)
{
    delete mTestObj;
}
