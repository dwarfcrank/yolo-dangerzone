#pragma once

#include <cstdint>
#include <vector>

namespace Util
{

/**
Performs color space transformation from RGB to YCbCr.

The pixel data is assumed to be stored as tightly packed 24-bpp RGB. The formula
used for pixel conversion is taken from http://en.wikipedia.org/wiki/YCbCr#JPEG_conversion.

@param[in] Pixels   An array of 24-bpp RGB pixels. The size of the array must be a multiple of 3.
@return             The pixel data converted to YCbCr or an empty array in case of incomplete data. 
*/
std::vector<uint8_t> ConvertRGBToYCbCr(const std::vector<std::uint8_t>& Pixels);

/**
Splits packed 24bpp pixels into three separate arrays, each containing only a single channel.

@param[in] Pixels       The original pixel data in unspecified color space.
@param[out] Channel1    The resulting data for the first channel.
@param[out] Channel2    The resulting data for the second channel.
@param[out] Channel3    The resulting data for the third channel.
*/
void SplitChannels(const std::vector<std::uint8_t>& Pixels, std::vector<std::uint8_t>* Channel1,
                   std::vector<std::uint8_t>* Channel2, std::vector<std::uint8_t>* Channel3);

}