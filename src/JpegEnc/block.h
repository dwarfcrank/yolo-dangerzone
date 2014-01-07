#pragma once

#include <array>
#include <vector>
#include <cstdint>

/**
A 8x8 block of 8-bit unsigned integers.
*/
typedef std::array<std::uint8_t, 64> Uint8Block;

/**
A 8x8 block of 16-bit integers. Used as the result type of the forward DCT
on an 8-bit block.
*/
typedef std::array<std::int16_t, 64> Int16Block;

namespace Util
{

/**
Splits an image to 8x8 blocks. If the width or height (or both) are not multiples of 8,
edge pixels on edge blocks will be repeated.

@param[in]      Pixels  The pixel data to be split into blocks.
@param[in]      Width   Width of the image. 
@param[in]      Height  Height of the image. 
@param[in,out]  Result  The resulting array of blocks.
*/
void SplitBlocks(const std::vector<uint8_t>& Pixels, int Width, int Height, std::vector<Uint8Block>* Result);

}

namespace Jpeg
{

/**
Reorders the elements of a block according to the zigzag pattern specified
in the JPEG specification.

@param[in]  Block   The block to reorder.
*/
void ReorderBlock(const Int16Block& Block, Int16Block* Result);

}