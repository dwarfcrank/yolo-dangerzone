#include "stdafx.h"
#include "quantization.h"

/**
The table used for quantizing luminance data.
Taken directly from the JPEG specification, table K.1.
*/
static const Uint8Block LuminanceTable =
{
    16, 11, 10, 16,  24,  40,  51,  61,
    12, 12, 14, 19,  26,  58,  60,  55,
    14, 13, 16, 24,  40,  57,  69,  56,
    14, 17, 22, 29,  51,  87,  80,  62,
    18, 22, 37, 56,  68, 109, 103,  77,
    24, 35, 55, 64,  81, 104, 113,  92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103,  99,
};

/**
The table used for quantizing chrominance data.
Taken directly from the JPEG specification, table K.2.
*/
static const Uint8Block ChrominanceTable =
{
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
};


/**
Quantizes a block using the supplied table and quality level.

@param[in]      Quality The quality factor to use for quantization.
                        Higher values result in less compression and better quality.
@param[in]      QuantizationTable The table used for quantization.
@param[in,out]  Block   The block to quantize.
*/
static void QuantizeBlock(int Quality, const Uint8Block& QuantizationTable, Int16Block* Block)
{
    const float qualityFactor = Jpeg::CalculateQualityFactor(Quality);
    Int16Block& block = *Block;

    for (int i = 0; i < 64; i++) {
        const float quantized = static_cast<float>(block[i]) / (QuantizationTable[i] * qualityFactor);
        block[i] = static_cast<std::int16_t>(quantized);
    }
}

namespace Jpeg
{

float CalculateQualityFactor(int Quality)
{
    if (Quality < 50) {
        return 50.0f / static_cast<float>(Quality);       
    }

    if (Quality > 50) {
        return static_cast<float>(100 - Quality) / 50.0f;
    }

    return 1.0f;
}

void QuantizeLuminanceBlock(int Quality, Int16Block* Block)
{
    QuantizeBlock(Quality, LuminanceTable, Block);
}

void QuantizeChrominanceBlock(int Quality, Int16Block* Block)
{
    QuantizeBlock(Quality, ChrominanceTable, Block);
}

const Uint8Block& GetChrominanceQuantizationMatrix()
{
    return ChrominanceTable;
}

const Uint8Block& GetLuminanceQuantizationMatrix()
{
    return LuminanceTable;
}

}