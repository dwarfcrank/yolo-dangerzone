#include "stdafx.h"
#include "util/bitvector.h"
#include "jpeg/huffman.h"

#include <cmath>
#include <cstdlib>

static std::uint16_t GetLength(std::int32_t Value)
{
    std::int32_t absValue = std::abs(Value);
    std::uint16_t length = 0;

    const std::int32_t ranges[] =
    {
        0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047,
    };

    for (int i = 0; i < 12; i++) {
        if (absValue <= ranges[i]) {
            return i;
        }
    }

    return length;
}

static std::size_t GetConsecutiveZeroCount(const Int16Block& Block, std::size_t StartIndex)
{
    std::size_t zeroes = 0;

    for (std::size_t i = StartIndex; i < 64; i++) {
        if (Block[i] != 0) {
            break;
        }

        zeroes++;
    }

    return zeroes;
}

namespace Jpeg
{

HuffmanEncoder::HuffmanEncoder(void)
{
    GenerateLuminanceTables();
    GenerateChrominanceTables();

    for (int i = 0; i < 3; i++) {
        m_PrevDCCoefficient[i] = 0;
    }
}

HuffmanEncoder::~HuffmanEncoder(void)
{
}

void HuffmanEncoder::EncodeBlock(const Int16Block& Block, ImageComponent Component, Util::BitVector* Result)
{
    if (Component == COMPONENT_Y) {
        EncodeBlock(Block, m_LuminanceDCTable, m_LuminanceACTable, Component, Result);
    } else {
        EncodeBlock(Block, m_ChrominanceDCTable, m_ChrominanceACTable, Component, Result);
    }
}

void HuffmanEncoder::EncodeBlock(const Int16Block& Block, const HuffmanTable& DCTable,
                                 const HuffmanTable& ACTable, ImageComponent Component, Util::BitVector* Result)
{
    EncodeDCCoefficient(Block, DCTable, Component, Result);
    EncodeACCoefficients(Block, ACTable, Result);
}

void HuffmanEncoder::EncodeDCCoefficient(const Int16Block& Block, const HuffmanTable& DCTable,
                                         ImageComponent Component, Util::BitVector* Result)
{
    std::int16_t difference = Block[0] - m_PrevDCCoefficient[Component];
    m_PrevDCCoefficient[Component] = Block[0];

    std::uint16_t length = GetLength(difference);
    const HuffmanCode& code = DCTable.Codes[length];

    Result->WriteBits(code.Code, code.Length);

    if (difference < 0) {
        difference -= 1;
    }

    Result->WriteBits(difference, length);
}

void HuffmanEncoder::EncodeACCoefficients(const Int16Block& Block, const HuffmanTable& ACTable,
                                          Util::BitVector* Result)
{
    bool eobWritten = false;

    for (std::size_t i = 1; i < 64; i++) {
        std::size_t zeroCount = GetConsecutiveZeroCount(Block, i);

        if (zeroCount == 64 - i) {
            const HuffmanCode& endOfBlock = ACTable.Codes[0];
            Result->WriteBits(endOfBlock.Code, endOfBlock.Length);
            eobWritten = true;

            break;
        } else if (zeroCount >= 16) {
            const HuffmanCode& zrl = ACTable.Codes[0xF0];
            Result->WriteBits(zrl.Code, zrl.Length);

            i += 16;
            zeroCount -= 16;
            continue;
        }

        i += zeroCount;

        std::uint16_t length = GetLength(Block[i]);
        std::uint8_t huffmanValue = ((zeroCount & 0xF) << 4) | (length & 0xF);

        const HuffmanCode& code = ACTable.Codes[huffmanValue];
        Result->WriteBits(code.Code, code.Length);

        std::int16_t value = 0;

        if (Block[i] < 0) {
            value = Block[i] - 1;
        } else {
            value = Block[i];
        }

        Result->WriteBits(value, length);
    }

    if (!eobWritten) {
        const HuffmanCode& endOfBlock = ACTable.Codes[0];
        Result->WriteBits(endOfBlock.Code, endOfBlock.Length);
    }
}

}