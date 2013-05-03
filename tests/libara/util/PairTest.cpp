/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include <string>
#include "Pair.h"

using namespace ARA;
using namespace std;

TEST_GROUP(PairTest) {};

TEST(PairTest, testPair) {
    string string1 = string("Foo");
    string string2 = string("Bar");

    Pair<string*, string*> pair (&string1, &string2);
    CHECK(pair.getLeft()  == &string1);
    CHECK(pair.getRight() == &string2);
}
