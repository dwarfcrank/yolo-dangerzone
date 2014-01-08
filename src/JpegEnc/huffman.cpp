#include "stdafx.h"
#include "huffman.h"
#include "bitvector.h"

#include <cmath>

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

HuffmanEncoder::HuffmanEncoder(void) :
    m_PrevDCCoefficient(0)
{
    GenerateLuminanceTables();
    GenerateChrominanceTables();
}

HuffmanEncoder::~HuffmanEncoder(void)
{
}

void HuffmanEncoder::EncodeChrominanceBlock(const Int16Block& Block, Util::BitVector* Result)
{
    EncodeBlock(Block, m_ChrominanceDCTable, m_ChrominanceACTable, Result);
}

void HuffmanEncoder::EncodeLuminanceBlock(const Int16Block& Block, Util::BitVector* Result)
{
    EncodeBlock(Block, m_LuminanceDCTable, m_LuminanceACTable, Result);
}

void HuffmanEncoder::EncodeBlock(const Int16Block& Block, const HuffmanTable& DCTable,
                                 const HuffmanTable& ACTable, Util::BitVector* Result)
{
    EncodeDCCoefficient(Block, DCTable, Result);
    EncodeACCoefficients(Block, ACTable, Result);
}

void HuffmanEncoder::EncodeDCCoefficient(const Int16Block& Block, const HuffmanTable& DCTable,
                                         Util::BitVector* Result)
{
    std::int16_t difference = Block[0] - m_PrevDCCoefficient;
    m_PrevDCCoefficient = Block[0];

    std::uint16_t length = GetLength(difference);
    const HuffmanCode& code = DCTable.Codes[length];

    Result->WriteBits(code.Code, code.Length);
    Result->WriteBits(difference, length);
}

void HuffmanEncoder::EncodeACCoefficients(const Int16Block& Block, const HuffmanTable& ACTable,
                                          Util::BitVector* Result)
{
    for (std::size_t i = 1; i < 64; i++) {
        std::size_t zeroCount = GetConsecutiveZeroCount(Block, i);

        if (zeroCount == 64 - i) {
            const HuffmanCode& endOfBlock = ACTable.Codes[0];
            Result->WriteBits(endOfBlock.Code, endOfBlock.Length);

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
    }
}

}