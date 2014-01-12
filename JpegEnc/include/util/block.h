#pragma once

#include <array>
#include <cstdint>
#include <vector>

/**
A 8x8 block of 8-bit unsigned integers.
*/
typedef std::array<std::uint8_t, 64> Uint8Block;

/**
A 8x8 block of 16-bit integers. Used as the result type of the forward DCT
on an 8-bit block.
*/
typedef std::array<std::int16_t, 64> Int16Block;

namespace Jpeg
{

/**
Reorders the elements of a block according to the zigzag pattern specified
in the JPEG specification.

@param[in]  Block   The block to reorder.
*/
void ReorderBlock(const Int16Block& Block, Int16Block* Result);

}