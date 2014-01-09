#include "stdafx.h"
#include "bitvector.h"

#include <vector>
#include <cstdint>

#include <boost/test/unit_test.hpp>

static void Check(const Util::BitVector& BitVector, const std::uint8_t* Expected, std::size_t NumExpected)
{
    const std::vector<std::uint8_t>& storage = BitVector.GetStorage();

    BOOST_CHECK_EQUAL_COLLECTIONS(storage.cbegin(), storage.cend(),
        Expected, Expected + NumExpected);
}

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define CHECK(bitvector, expected) Check((bitvector), (expected), sizeof((expected)))

BOOST_AUTO_TEST_CASE(WriteSingle)
{
    Util::BitVector bv;
    std::uint8_t expected[] = { 0x80 }; // 8-bit uint with MSB set

    bv.WriteBit(1);
    CHECK(bv, expected);
}

BOOST_AUTO_TEST_CASE(WriteMultiple)
{
    Util::BitVector bv;
    std::uint8_t expected[] = { 0xFF, 0xFF, 0xFF, 0xFF }; // 8-bit uints with all ones

    bv.WriteBits(0xFFFFFFFF, 32);

    BOOST_CHECK_EQUAL(bv.GetSize(), 32);
    CHECK(bv, expected);
}

BOOST_AUTO_TEST_CASE(WriteMultipleSplit)
{
    Util::BitVector bv;
    std::uint8_t expected[] = { 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00 };

    bv.WriteBits(0xFFFFFF, 24);
    bv.WriteBits(0, 16);
    bv.WriteBits(0xFFFF, 16);
    bv.WriteBits(0, 8);

    BOOST_CHECK_EQUAL(bv.GetSize(), 64);
    CHECK(bv, expected);
}

BOOST_AUTO_TEST_CASE(WritePattern)
{
    Util::BitVector bv;
    std::uint8_t expected[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55 };

    for (int i = 0; i < 32; i += 2) {
        bv.WriteBit(1);
        bv.WriteBit(0);
    }

    for (int i = 0; i < 32; i += 2) {
        bv.WriteBit(0);
        bv.WriteBit(1);
    }

    BOOST_CHECK_EQUAL(bv.GetSize(), 64);
    CHECK(bv, expected);
}

BOOST_AUTO_TEST_CASE(Clear)
{
    Util::BitVector bv;

    bv.WriteBits(0xFF, 8);
    BOOST_CHECK_EQUAL(bv.GetSize(), 8);

    bv.Clear();
    BOOST_CHECK_EQUAL(bv.GetSize(), 0);
}