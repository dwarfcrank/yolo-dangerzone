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
void QuantizeLuminanceBlock(int Quality, Int16Block* Block);

/**
Performs quantization on a block of chrominance data.

@param[in]      Quality The quality factor to use for quantization.
                        Higher values result in less compression and better quality.
@param[in,out]  Block   The block to be quantized.
*/
void QuantizeChrominanceBlock(int Quality, Int16Block* Block);

/**
Gets the chrominance quantization matrix.

@return A const reference to the chrominance quantization matrix.
*/
const Uint8Block& GetChrominanceQuantizationMatrix();

/**
Gets the luminance quantization matrix.

@return A const reference to the luminance quantization matrix.
*/
const Uint8Block& GetLuminanceQuantizationMatrix();

/**
Calculates the quantum multiplier for the desired quality level.

@param[in]  Quality The desired quality level in the range [1, 100].
@return     The quantum multiplier.
*/
float CalculateQualityFactor(int Quality);

}