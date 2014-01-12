#pragma once

#include "util/block.h"

#include <array>
#include <cstdint>
#include <vector>

namespace Util
{
    class BitVector;
}

namespace Jpeg
{

struct HuffmanCode
{
    std::uint16_t Code;
    std::uint16_t Length;
};

struct HuffmanTable
{
    /**
    Each element represents the number of codes of that length,
    e.g. NumCodes[8] is the number of 8-bit codes.
    The JPEG spec refers to this array as BITS.
    */
    std::uint8_t NumCodes[16];

    /**
    The Huffman codes to be used for encoding. Some of these may not be used,
    but having a constant sized array makes indexing easier. This table corresponds
    to the tables EHUFCO and EHUFSI mentioned in the JPEG spec.
    */
    HuffmanCode Codes[256];

    /**
    The values associated with each Huffman code. Used only when writing the
    table into a file.
    */
    std::uint8_t Values[256];
};

enum ImageComponent
{
    COMPONENT_Y = 0,
    COMPONENT_CB,
    COMPONENT_CR,
    NUM_COMPONENTS,
};

/**
A class for Huffman coding image blocks.
The Huffman coding process has some state associated with it, so
the encoder is written as a class rather than free functions.
*/
class HuffmanEncoder
{
public:
    HuffmanEncoder(void);
    ~HuffmanEncoder(void);

    /**
    Encodes a single block.

    @param[in]  Block       The block to encode.
    @param[in]  Component   The component to encode.
    @param[out] Result      The bit vector to encode the block into.
    */
    void EncodeBlock(const Int16Block& Block, ImageComponent Component, Util::BitVector* Result);

    const HuffmanTable& GetLuminanceDCTable()
    {
        return m_LuminanceDCTable;
    }

    const HuffmanTable& GetLuminanceACTable()
    {
        return m_LuminanceACTable;
    }

    const HuffmanTable& GetChrominanceDCTable()
    {
        return m_ChrominanceDCTable;
    }

    const HuffmanTable& GetChrominanceACTable()
    {
        return m_ChrominanceACTable;
    }

private:
    /**
    The difference between DC coefficients of consecutive blocks
    is coded rather than the coefficients themselves, so the last coded
    DC coefficient needs to be tracked.
    */
    std::int16_t m_PrevDCCoefficient[NUM_COMPONENTS];

    /**
    The Huffman codes used to encode DC coefficients.
    */
    HuffmanTable m_LuminanceDCTable;
    HuffmanTable m_ChrominanceDCTable;

    /**
    The Huffman codes used to encode AC coefficients.
    */
    HuffmanTable m_LuminanceACTable;
    HuffmanTable m_ChrominanceACTable;

    /**
    Generates the Huffman tables for luminance DC and AC coefficients.
    Implemented in hufftable.cpp.
    */
    void GenerateLuminanceTables();

    /**
    Generates the Huffman tables for chrominance DC and AC coefficients.
    Implemented in hufftable.cpp.
    */
    void GenerateChrominanceTables();

    /**
    Encodes a single image block using the supplied tables.

    @param[in]  Block   The block to encode.
    @param[in]  DCTable The Huffman table to use for encoding DC coefficients.
    @param[in]  ACTable The Huffman table to use for encoding AC coefficients.
    @param[out] Result  The bit vector to write the encoded block into.
    */
    void EncodeBlock(const Int16Block& Block, const HuffmanTable& DCTable,
                     const HuffmanTable& ACTable, ImageComponent Component, Util::BitVector* Result);

    void EncodeDCCoefficient(const Int16Block& Block, const HuffmanTable& DCTable,
                             ImageComponent Component, Util::BitVector* Result);

    void EncodeACCoefficients(const Int16Block& Block, const HuffmanTable& ACTable,
                              Util::BitVector* Result);
};

}
