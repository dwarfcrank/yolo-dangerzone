#include "stdafx.h"
#include "bitvector.h"

#include <vector>
#include <cstdint>

#include <boost/test/unit_test.hpp>

static void Check(const Util::BitVector& BitVector, const std::uint32_t* Expected, std::size_t NumExpected)
{
    const std::vector<std::uint32_t>& storage = BitVector.GetStorage();

    BOOST_CHECK_EQUAL_COLLECTIONS(storage.cbegin(), storage.cend(),
        Expected, Expected + NumExpected);
}

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define CHECK(bitvector, expected) Check((bitvector), (expected), ARRAYSIZE((expected)))

BOOST_AUTO_TEST_CASE(WriteSingle)
{
    Util::BitVector bv;
    std::uint32_t expected[] = { 0x80000000 }; // 32-bit uint with MSB set

    bv.WriteBit(1);
    CHECK(bv, expected);
}

BOOST_AUTO_TEST_CASE(WriteMultiple)
{
    Util::BitVector bv;
    std::uint32_t expected[] = { 0xFFFFFFFF }; // 32-bit uint with all ones

    bv.WriteBits(0xFFFFFFFF, 32);
    CHECK(bv, expected);
}

BOOST_AUTO_TEST_CASE(WriteMultipleSplit)
{
    Util::BitVector bv;
    std::uint32_t expected[] = { 0xFFFFFF00, 0x00FFFF00 }; // 32-bit uint with all ones

    bv.WriteBits(0xFFFFFF, 24);
    bv.WriteBits(0, 16);
    bv.WriteBits(0xFFFF, 16);
    bv.WriteBits(0, 8);

    CHECK(bv, expected);
}

BOOST_AUTO_TEST_CASE(WritePattern)
{
    Util::BitVector bv;
    std::uint32_t expected[] = { 0xAAAAAAAA, 0x55555555 }; // 32-bit uint consisting of 101010..., then 010101...

    for (int i = 0; i < 32; i += 2) {
        bv.WriteBit(1);
        bv.WriteBit(0);
    }

    for (int i = 0; i < 32; i += 2) {
        bv.WriteBit(0);
        bv.WriteBit(1);
    }

    CHECK(bv, expected);
}