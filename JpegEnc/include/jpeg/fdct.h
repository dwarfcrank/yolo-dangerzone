#pragma once

#include "util/block.h"

namespace Jpeg
{

/**
Computes a normalized two-dimensional type-II discrete cosine transform for a block,
temporarily increasing its bit-depth and thus requiring the values to be stored in a
block with 16-bit elements.

@param[in]  Source  The block to be transformed.
@param[out] Destination The transformed block.
*/
void ComputeBlockDCT(const Uint8Block& Source, Int16Block* Destination);

}