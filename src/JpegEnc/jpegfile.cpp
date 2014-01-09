#include "stdafx.h"
#include "jpegfile.h"
#include "huffman.h"
#include "bitvector.h"

#include <iostream>
#include <cstdint>

template<typename TValue>
void Write(std::ostream& Stream, const TValue& Value)
{
    Stream.write(reinterpret_cast<const char*>(&Value), sizeof(TValue));
}

static void WriteMarker(std::uint8_t Marker, std::ostream& Stream)
{
    Write(Stream, std::uint8_t(0xFF));
    Write(Stream, Marker);
}

static std::uint16_t ByteSwap(std::uint16_t Value)
{
    return ((Value & 0xFF) << 8) | ((Value & 0xFF00) >> 8);
}

static const int JpegNaturalOrder[] =
{
     0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};

namespace Jpeg
{

void WriteStartOfImage(std::ostream& Stream)
{
    WriteMarker(0xD8, Stream);
}

void WriteEndOfImage(std::ostream& Stream)
{
    WriteMarker(0xD9, Stream);
}

void WriteHuffmanTable(std::ostream& Stream, const HuffmanTable& Table, TableClass Type, int Index)
{
    WriteMarker(0xC4, Stream);

    std::uint16_t codeCount = 0;
    for (int i = 0; i < 16; i++) {
        codeCount += Table.NumCodes[i];
    }

    // Write the size of the table segment.
    // The total size of the segment is the number of codes (one byte each) + the rest of the header.
    Write(Stream, ByteSwap(codeCount + 17));

    // The table class and destination index are encoded as 4 bits each, so they need
    // to be encoded in the same byte. 
    Write(Stream, std::uint8_t((Type << 4) | Index));

    for (int i = 0; i < 16; i++) {
        Write(Stream, Table.NumCodes[i]);
    }

    for (int i = 0; i < codeCount; i++) {
        Write(Stream, Table.Values[i]);
    }
}

void WriteQuantizationTable(std::ostream& Stream, const Uint8Block& Table, QuantizationTableType Type)
{
    WriteMarker(0xDB, Stream);

    Uint8Block reorderedTable;
    for (int i = 0; i < 64; i++) {
        reorderedTable[i] = Table[JpegNaturalOrder[i]];
    }

    // Write the size of the quantization table and its parameters.
    // The total size is 2 bytes for the size parameter + 1 for precision and destination
    // + 64 for all the table elements.
    Write(Stream, ByteSwap(2 + 1 + 64));

    // Write the table bit depth and destination index.
    // 8-bit JPEG images are only allowed to use 8-bit quantization tables, so the upper
    // 4 bits of this byte are zero. The table type determines the index.
    Write(Stream, std::uint8_t(Type));

    for (int i = 0; i < 64; i++) {
        Write(Stream, reorderedTable[i]);
    }
}

void WriteFrameHeader(std::ostream& Stream, std::uint16_t Width, std::uint16_t Height)
{
    // Write the SOF0 marker to mark this frame as a baseline DCT frame.
    WriteMarker(0xC0, Stream);

    // Write the size of the header, 8 + 3 * number of image components.
    Write(Stream, ByteSwap(8 + 3 * 3));

    // Write the image component precision.
    Write(Stream, std::uint8_t(8));

    auto a = ByteSwap(Width);
    auto b = ByteSwap(Height);

    // Write the height and width of the image.
    Write(Stream, ByteSwap(Height));
    Write(Stream, ByteSwap(Width));

    // Write the number of image components in the frame.
    Write(Stream, std::uint8_t(3));

    for (int i = 1; i <= 3; i++) {
        // Write the component identifier.
        Write(Stream, std::uint8_t(i));

        // Write the horizontal and vertical sampling factors. These are hardcoded to 4
        // as no subsampling is done for any of the channels.
        Write(Stream, std::uint8_t(0x11));

        // Write the quantization table index. The first channel is luminance, so that uses
        // a different table.
        Write(Stream, std::uint8_t((i == 1) ? 0 : 1));
    }
}

void WriteScan(std::ostream& Stream, const Util::BitVector& Data)
{
    // Write the SOS marker.
    WriteMarker(0xDA, Stream);

    // Write the scan header length.
    Write(Stream, ByteSwap(6 + 2 * 3));

    // Write the number of image components in this scan.
    Write(Stream, std::uint8_t(3));
    
    // Write the scan component selector
    for (int i = 0; i < 3; i++) {
        Write(Stream, std::uint8_t(i + 1));

        if (i == 0) {
            Write(Stream, std::uint8_t(0));
        } else {
            Write(Stream, std::uint8_t(0x11));
        }
    }

    Write(Stream, std::uint8_t(0));
    Write(Stream, std::uint8_t(63));

    Write(Stream, std::uint8_t(0));

    auto storage = Data.GetStorage();
    for (auto it = storage.cbegin(); (it + 1) != storage.cend(); ++it) {
        auto v = *it;

        Write(Stream, v);

        if (v == 0xFF) {
            Write(Stream, uint8_t(0));
        }
    }

    auto v = *(storage.crbegin());
    auto s = Data.GetSize() % 8;
    if (s != 0) {
        v |= (0xFF >> s);
    }

    Write(Stream, std::uint8_t(v));

    if (v == 0xFF) {
        Write(Stream, std::uint8_t(0));
    }
}

}