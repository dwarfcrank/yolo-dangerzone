#pragma once

#include "block.h"

namespace Jpeg
{

/**
Performs quantization on a block of luminance data.

@param[in]      Quality The quality factor to use for quantization.
                        Higher values result in less compression and better quality.
@param[in,out]  Block   The block to be quantized.
*/
void QuantizeLumaBlock(int Quality, Int16Block* Block);

/**
Performs quantization on a block of chrominance data.

@param[in]      Quality The quality factor to use for quantization.
                        Higher values result in less compression and better quality.
@param[in,out]  Block   The block to be quantized.
*/
void QuantizeChromaBlock(int Quality, Int16Block* Block);

}