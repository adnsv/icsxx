#include "doctest.h"

#include <array>
#include <ics.hpp>
#include <string>

TEST_CASE("containment")
{
    using crange = ics<unsigned char>;

    auto test = [](std::string s, unsigned char v, bool want) -> bool {
        auto vec = std::vector<unsigned char>{s.begin(), s.end()};
        auto set = ics<unsigned char>{vec};
        auto have = set.contains(v);
        return have == want;
    };

    // empty
    CHECK(test("", 'f', false));

    // open for any
    CHECK(test("c", 'b', false));
    CHECK(test("c", 'c', true));
    CHECK(test("c", 'd', true));
    CHECK(test("c", '\xff', true));

    // open for any greater thant 0x01
    CHECK(test("\x01", '\x00', false));
    CHECK(test("\x01", '\x01', true));
    CHECK(test("\x01", 'a', true));
    CHECK(test("\x01", 'z', true));
    CHECK(test("\x01", '\xff', true));

    // single bounded
    CHECK(test("bd", '\x00', false));
    CHECK(test("bd", 'a', false));
    CHECK(test("bd", 'b', true));
    CHECK(test("bd", 'c', true));
    CHECK(test("bd", 'd', false));
    CHECK(test("bd", 'e', false));
    CHECK(test("bd", '\xff', false));

    // bounded then open
    CHECK(test("bdf", '\x00', false));
    CHECK(test("bdf", 'a', false));
    CHECK(test("bdf", 'b', true));
    CHECK(test("bdf", 'c', true));
    CHECK(test("bdf", 'd', false));
    CHECK(test("bdf", 'e', false));
    CHECK(test("bdf", 'f', true));
    CHECK(test("bdf", 'g', true));
    CHECK(test("bdf", '\xff', true));

    // double bounded
    CHECK(test("bdfg", '\x00', false));
    CHECK(test("bdfg", 'a', false));
    CHECK(test("bdfg", 'b', true));
    CHECK(test("bdfg", 'c', true));
    CHECK(test("bdfg", 'd', false));
    CHECK(test("bdfg", 'e', false));
    CHECK(test("bdfg", 'f', true));
    CHECK(test("bdfg", 'g', false));
    CHECK(test("bdfg", '\xff', false));
}

TEST_CASE("insertion")
{

    auto cs = ics<uint8_t>{};

    auto prepare = [&](ics<uint8_t>& s, uint8_t l, uint8_t h) {
        s.insert(l, h);
        return to_string(s);
    };

    auto insert = [&](ics<uint8_t>& s, uint8_t l, uint8_t h) {
        auto tmp = s;
        tmp.insert(l, h);
        return to_string(tmp);
    };

    // insert open
    CHECK(insert(cs, 6, 6) == "[6...");

    // insert bounded
    CHECK(insert(cs, 4, 7) == "[4,7)");

    // ------------------------------
    // TARGET: a single open interval
    // ------------------------------
    cs.clear();

    CHECK(prepare(cs, 6, 6) == "[6...");

    // inserting open
    CHECK(insert(cs, 0, 0) == "[0...");
    CHECK(insert(cs, 4, 4) == "[4...");
    CHECK(insert(cs, 6, 6) == "[6...");
    CHECK(insert(cs, 7, 7) == "[6...");

    // inserting bounded
    CHECK(insert(cs, 0, 5) == "[0,5)[6...");
    CHECK(insert(cs, 0, 6) == "[0...");
    CHECK(insert(cs, 0, 7) == "[0...");
    CHECK(insert(cs, 5, 6) == "[5...");
    CHECK(insert(cs, 6, 7) == "[6...");
    CHECK(insert(cs, 7, 8) == "[6...");
    CHECK(insert(cs, 5, 8) == "[5...");
    CHECK(insert(cs, 5, 8) == "[5...");

    // -------------------------------
    // TARGET: single bounded interval
    // -------------------------------
    cs.clear();
    CHECK(prepare(cs, 3, 8) == "[3,8)");

    // inserting open
    CHECK(insert(cs, 0, 0) == "[0...");
    CHECK(insert(cs, 3, 3) == "[3...");
    CHECK(insert(cs, 5, 5) == "[3...");
    CHECK(insert(cs, 8, 8) == "[3...");
    CHECK(insert(cs, 9, 9) == "[3,8)[9...");
    CHECK(insert(cs, 10, 10) == "[3,8)[10...");

    // inserting bounded
    CHECK(insert(cs, 0, 1) == "[0,1)[3,8)");
    CHECK(insert(cs, 0, 3) == "[0,8)");
    CHECK(insert(cs, 0, 5) == "[0,8)");
    CHECK(insert(cs, 0, 8) == "[0,8)");
    CHECK(insert(cs, 0, 9) == "[0,9)");
    //
    CHECK(insert(cs, 3, 5) == "[3,8)");
    CHECK(insert(cs, 3, 8) == "[3,8)");
    CHECK(insert(cs, 3, 9) == "[3,9)");
    //
    CHECK(insert(cs, 5, 6) == "[3,8)");
    CHECK(insert(cs, 5, 8) == "[3,8)");
    CHECK(insert(cs, 5, 9) == "[3,9)");
    //
    CHECK(insert(cs, 8, 9) == "[3,9)");
    CHECK(insert(cs, 9, 10) == "[3,8)[9,10)");

    // -------------------------------------------------------
    // TARGET: a bounded interval followed by an open interval
    // -------------------------------------------------------
    cs.clear();
    CHECK(prepare(cs, 3, 5) == "[3,5)");
    CHECK(prepare(cs, 8, 8) == "[3,5)[8...");

    // inserting open
    CHECK(insert(cs, 0, 0) == "[0...");
    CHECK(insert(cs, 2, 2) == "[2...");
    CHECK(insert(cs, 3, 3) == "[3...");
    CHECK(insert(cs, 4, 4) == "[3...");
    CHECK(insert(cs, 5, 5) == "[3...");
    CHECK(insert(cs, 6, 6) == "[3,5)[6...");
    CHECK(insert(cs, 7, 7) == "[3,5)[7...");
    CHECK(insert(cs, 8, 8) == "[3,5)[8...");
    CHECK(insert(cs, 9, 9) == "[3,5)[8...");

    // inserting bounded
    CHECK(insert(cs, 0, 2) == "[0,2)[3,5)[8...");
    CHECK(insert(cs, 0, 3) == "[0,5)[8...");
    CHECK(insert(cs, 0, 4) == "[0,5)[8...");
    CHECK(insert(cs, 0, 5) == "[0,5)[8...");
    CHECK(insert(cs, 0, 6) == "[0,6)[8...");
    CHECK(insert(cs, 0, 7) == "[0,7)[8...");
    CHECK(insert(cs, 0, 8) == "[0...");
    CHECK(insert(cs, 0, 9) == "[0...");
    //
    CHECK(insert(cs, 0, 5) == "[0,5)[8...");
    CHECK(insert(cs, 0, 3) == "[0,5)[8...");
    CHECK(insert(cs, 2, 4) == "[2,5)[8...");
    CHECK(insert(cs, 2, 5) == "[2,5)[8...");
    CHECK(insert(cs, 2, 6) == "[2,6)[8...");
    CHECK(insert(cs, 2, 7) == "[2,7)[8...");
    CHECK(insert(cs, 2, 8) == "[2...");
    CHECK(insert(cs, 3, 8) == "[3...");
    CHECK(insert(cs, 4, 8) == "[3...");
    CHECK(insert(cs, 5, 8) == "[3...");
    CHECK(insert(cs, 6, 8) == "[3,5)[6...");
    CHECK(insert(cs, 7, 8) == "[3,5)[7...");
    CHECK(insert(cs, 8, 10) == "[3,5)[8...");

    // -----------------------------------
    // TARGET: a pair of bounded intervals
    // -----------------------------------
    cs.clear();
    CHECK(prepare(cs, 1, 3) == "[1,3)");
    CHECK(prepare(cs, 6, 8) == "[1,3)[6,8)");

    // insert open
    CHECK(insert(cs, 0, 0) == "[0...");
    CHECK(insert(cs, 1, 1) == "[1...");
    CHECK(insert(cs, 2, 2) == "[1...");
    CHECK(insert(cs, 3, 3) == "[1...");
    CHECK(insert(cs, 4, 4) == "[1,3)[4...");
    CHECK(insert(cs, 5, 5) == "[1,3)[5...");
    CHECK(insert(cs, 6, 6) == "[1,3)[6...");
    CHECK(insert(cs, 7, 7) == "[1,3)[6...");
    CHECK(insert(cs, 8, 8) == "[1,3)[6...");
    CHECK(insert(cs, 9, 9) == "[1,3)[6,8)[9...");
    CHECK(insert(cs, 10, 10) == "[1,3)[6,8)[10...");

    // insert bounded
    CHECK(insert(cs, 0, 1) == "[0,3)[6,8)");
    CHECK(insert(cs, 0, 3) == "[0,3)[6,8)");
    CHECK(insert(cs, 0, 4) == "[0,4)[6,8)");
    CHECK(insert(cs, 0, 6) == "[0,8)");
    CHECK(insert(cs, 0, 7) == "[0,8)");
    CHECK(insert(cs, 0, 8) == "[0,8)");
    CHECK(insert(cs, 0, 9) == "[0,9)");
    //
    CHECK(insert(cs, 1, 2) == "[1,3)[6,8)");
    CHECK(insert(cs, 1, 3) == "[1,3)[6,8)");
    CHECK(insert(cs, 1, 4) == "[1,4)[6,8)");
    CHECK(insert(cs, 1, 6) == "[1,8)");
    CHECK(insert(cs, 1, 7) == "[1,8)");
    CHECK(insert(cs, 1, 8) == "[1,8)");
    CHECK(insert(cs, 1, 9) == "[1,9)");
    //
    CHECK(insert(cs, 2, 3) == "[1,3)[6,8)");
    CHECK(insert(cs, 2, 4) == "[1,4)[6,8)");
    CHECK(insert(cs, 2, 6) == "[1,8)");
    CHECK(insert(cs, 2, 7) == "[1,8)");
    CHECK(insert(cs, 2, 8) == "[1,8)");
    CHECK(insert(cs, 2, 9) == "[1,9)");
    //
    CHECK(insert(cs, 3, 4) == "[1,4)[6,8)");
    CHECK(insert(cs, 3, 5) == "[1,5)[6,8)");
    CHECK(insert(cs, 3, 6) == "[1,8)");
    CHECK(insert(cs, 3, 7) == "[1,8)");
    CHECK(insert(cs, 3, 8) == "[1,8)");
    CHECK(insert(cs, 3, 9) == "[1,9)");
    //
    CHECK(insert(cs, 4, 5) == "[1,3)[4,5)[6,8)");
    CHECK(insert(cs, 4, 6) == "[1,3)[4,8)");
    CHECK(insert(cs, 4, 7) == "[1,3)[4,8)");
    CHECK(insert(cs, 4, 8) == "[1,3)[4,8)");
    CHECK(insert(cs, 4, 9) == "[1,3)[4,9)");
    //
    CHECK(insert(cs, 5, 6) == "[1,3)[5,8)");
    //
    CHECK(insert(cs, 6, 7) == "[1,3)[6,8)");
    CHECK(insert(cs, 6, 8) == "[1,3)[6,8)");
    CHECK(insert(cs, 6, 9) == "[1,3)[6,9)");
    //
    CHECK(insert(cs, 7, 8) == "[1,3)[6,8)");
    CHECK(insert(cs, 7, 9) == "[1,3)[6,9)");
    //
    CHECK(insert(cs, 8, 9) == "[1,3)[6,9)");
    //
    CHECK(insert(cs, 9, 10) == "[1,3)[6,8)[9,10)");
}