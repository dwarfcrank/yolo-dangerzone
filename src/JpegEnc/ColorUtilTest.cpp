#include "stdafx.h"

#include <vector>
#include <cstdint>

#include "colorutil.h"

#include <boost/test/unit_test.hpp>

// Base pixel data, 24-bpp RGB.
static const std::uint8_t TEST_PIXEL_DATA[] =
{
    // R  G     B
    0xAA, 0xBB, 0xCC,
    0xAA, 0xBB, 0xCC,
    0xBB, 0xAA, 0xBB,
    0xBB, 0xAA, 0xBB,
    0xCC, 0xCC, 0xAA,
    0xCC, 0xCC, 0xAA,
};

// Expected red channel values, see the R column in TEST_PIXEL_DATA.
static const std::uint8_t CHANNEL1_EXPECTED[] =
{
    0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC,
};

// Expected green channel values, see the G column in TEST_PIXEL_DATA.
static const std::uint8_t CHANNEL2_EXPECTED[] =
{
    0xBB, 0xBB, 0xAA, 0xAA, 0xCC, 0xCC,
};

// Expected blue channel values, see the B column in TEST_PIXEL_DATA.
static const std::uint8_t CHANNEL3_EXPECTED[] =
{
    0xCC, 0xCC, 0xBB, 0xBB, 0xAA, 0xAA,
};

BOOST_AUTO_TEST_CASE(SplitChannelsTest)
{
    std::vector<std::uint8_t> pixels(TEST_PIXEL_DATA, 
        TEST_PIXEL_DATA + sizeof(TEST_PIXEL_DATA));

    std::vector<std::uint8_t> channel1;
    std::vector<std::uint8_t> channel2;
    std::vector<std::uint8_t> channel3;

    Util::SplitChannels(pixels, &channel1, &channel2, &channel3);

    BOOST_CHECK_EQUAL_COLLECTIONS(channel1.cbegin(), channel1.cend(),
        CHANNEL1_EXPECTED, CHANNEL1_EXPECTED + sizeof(CHANNEL1_EXPECTED));
    BOOST_CHECK_EQUAL_COLLECTIONS(channel2.cbegin(), channel2.cend(),
        CHANNEL2_EXPECTED, CHANNEL2_EXPECTED + sizeof(CHANNEL2_EXPECTED));
    BOOST_CHECK_EQUAL_COLLECTIONS(channel3.cbegin(), channel3.cend(),
        CHANNEL3_EXPECTED, CHANNEL3_EXPECTED + sizeof(CHANNEL3_EXPECTED));
}