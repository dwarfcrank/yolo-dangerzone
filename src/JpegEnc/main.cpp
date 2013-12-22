#include "stdafx.h"
#include "bmp.h"
#include <vector>
#include <cstdint>

int main(int argc, char* argv[])
{
	std::vector<std::uint8_t> pixels;
	std::uint32_t width, height;

	auto status = BMP::LoadBitmap("test_3x3_24bpp.bmp", &pixels, &width, &height);
	return 0;
}
