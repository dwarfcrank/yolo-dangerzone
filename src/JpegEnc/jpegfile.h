#pragma once

#include "block.h"
#include "huffman.h"

#include <iostream>

namespace Jpeg
{
    enum TableClass
    {
        DC_TABLE = 0,
        AC_TABLE,
    };

    enum QuantizationTableType
    {
        QUANT_LUMINANCE = 0,
        QUANT_CHROMINANCE,
    };

void WriteStartOfImage(std::ostream& Stream);

void WriteJFIFHeader(std::ostream& Stream);

void WriteEndOfImage(std::ostream& Stream);

void WriteHuffmanTable(std::ostream& Stream, const HuffmanTable& Table, TableClass Type, int Index);

void WriteQuantizationTable(std::ostream& Stream, const Uint8Block& Table, QuantizationTableType Type, int Quality);

void WriteFrameHeader(std::ostream& Stream, std::uint16_t Width, std::uint16_t Height);

void WriteScan(std::ostream& Stream, const Util::BitVector& Data);
}