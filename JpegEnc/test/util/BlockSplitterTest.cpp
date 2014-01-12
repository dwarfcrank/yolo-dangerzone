#include "stdafx.h"
#include "util/blocksplitter.h"

#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>

BOOST_AUTO_TEST_CASE(_TestSplit1x1)
{
    std::vector<std::uint8_t> pixels;
    pixels.push_back(0xAA);
    pixels.push_back(0xBB);
    pixels.push_back(0xCC);

    Uint8Block result[3];
    Uint8Block expected[3];

    for (int i = 0; i < 3; i++) {
        memset(&expected[i][0], 0, 64);
        memset(&result[i][0], 0, 64);
    }

    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 8; x++) {
            expected[i][x] = 0xAA + (0x11 * i);
            expected[i][x * 8] = 0xAA + (0x11 * i);
        }
    }

    Util::BlockSplitter splitter(pixels, 1, 1);
    for (int i = 0; i < 3; i++) {
        splitter.ExtractBlock(0, i, &result[i]);
    }

    BOOST_CHECK_EQUAL(1, splitter.GetNumBlocks());
    BOOST_CHECK(result[0] == expected[0]);
    BOOST_CHECK(result[1] == expected[1]);
    BOOST_CHECK(result[2] == expected[2]);
}

BOOST_AUTO_TEST_CASE(_TestSplit9x8)
{
    std::vector<std::uint8_t> pixels;

    for (int i = 0; i < 9*8; i++) {
        pixels.push_back(0xAA);
        pixels.push_back(0xBB);
        pixels.push_back(0xCC);
    }

    Uint8Block result[6];
    Uint8Block expected[6];

    for (int i = 0; i < 2; i++) {
        memset(&expected[i * 3], 0xAA, 64);
        memset(&expected[i * 3 + 1], 0xBB, 64);
        memset(&expected[i * 3 + 2], 0xCC, 64);
    }

    Util::BlockSplitter splitter(pixels, 9, 8);

    for (int block = 0; block < 2; block++) {
        for (int channel = 0; channel < 3; channel++) {
            splitter.ExtractBlock(block, channel, &result[block * 3 + channel]);
        }
    }

    BOOST_CHECK(result[0] == expected[0]);
    BOOST_CHECK(result[1] == expected[1]);
    BOOST_CHECK(result[2] == expected[2]);
    BOOST_CHECK(result[3] == expected[3]);
    BOOST_CHECK(result[4] == expected[4]);
    BOOST_CHECK(result[5] == expected[5]);
}
