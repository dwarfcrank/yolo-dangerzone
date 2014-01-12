#pragma once

#include <cstdint>
#include <vector>

namespace Util
{

/**
Performs in-place color space transformation from RGB to YCbCr.

The pixel data is assumed to be stored as tightly packed 24-bpp RGB. The formula
used for pixel conversion is taken from http://en.wikipedia.org/wiki/YCbCr#JPEG_conversion.

@param[in,out]  Pixels  An array of 24-bpp RGB pixels. The size of the array must be a multiple of 3.
*/
void ConvertRGBToYCbCr(std::vector<std::uint8_t>* Pixels);

}