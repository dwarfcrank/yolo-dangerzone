#include "stdafx.h"
#include "bmp.h"

#include <vector>
#include <cstdint>

#define BOOST_TEST_MODULE BitmapTest
#include <boost/test/unit_test.hpp>

// Normal 24bpp 3x3 pixel BMP image.
static const char* TEST_3X3_24BPP_NAME = "test_3x3_24bpp.bmp";

// 24bpp 3x3 pixel BMP image with incomplete pixel data.
static const char* TEST_3X3_24BPP_INVALID_NAME = "test_3x3_24bpp_invalid.bmp";

// Normal 4bpp 3x3 pixel BMP image.
static const char* TEST_3X3_4BPP_NAME = "test_3x3_4bpp.bmp";

BOOST_AUTO_TEST_CASE(LoadNon24BppTest)
{
	std::vector<std::uint8_t> pixels;
	std::uint32_t width, height;

	// Loading a non-24bpp image should fail.
	auto status = BMP::LoadBitmap(TEST_3X3_4BPP_NAME, &pixels, &width, &height);
	BOOST_CHECK_EQUAL(status, BMP::INVALID_INFO_HEADER);
}

// Raw pixel values for test_3x3_24bpp.bmp
static const std::uint8_t EXPECTED_PIXELS[] =
{
//  X                                                          Y
//  0                 1                 2
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,	// 0
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,	// 1
	0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,	// 2
};

BOOST_AUTO_TEST_CASE(Load24BppTest)
{
	std::vector<std::uint8_t> pixels;
	std::uint32_t width, height;

	auto status = BMP::LoadBitmap(TEST_3X3_24BPP_NAME, &pixels, &width, &height);

	BOOST_CHECK_EQUAL(status, BMP::LOAD_SUCCESS);
	BOOST_CHECK_EQUAL(width, 3);
	BOOST_CHECK_EQUAL(height, 3);

	BOOST_CHECK_EQUAL(pixels.size(), sizeof(EXPECTED_PIXELS));

	BOOST_CHECK_EQUAL_COLLECTIONS(pixels.begin(), pixels.end(),
		EXPECTED_PIXELS, (EXPECTED_PIXELS + sizeof(EXPECTED_PIXELS)));
}

BOOST_AUTO_TEST_CASE(Load24BppInvalidTest)
{
	std::vector<std::uint8_t> pixels;
	std::uint32_t width, height;

	auto status = BMP::LoadBitmap(TEST_3X3_24BPP_INVALID_NAME, &pixels, &width, &height);

	// Loading file with incomplete pixel data should fail.
	BOOST_CHECK_EQUAL(status, BMP::UNEXPECTED_END_OF_FILE);
}