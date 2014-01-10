#include "stdafx.h"
#include "bmp.h"
#include <vector>
#include <cstdint>
#include <array>
#include <iostream>
#include <fstream>
#include "block.h"
#include "fdct.h"
#include "bitvector.h"
#include "huffman.h"
#include "bmp.h"
#include "colorutil.h"
#include "jpegfile.h"
#include "quantization.h"
#include <cstdio>

int main(int argc, char* argv[])
{
    Jpeg::HuffmanEncoder encoder;
    Util::BitVector bits;

    std::ofstream s(argv[2], std::ios::binary | std::ios::out);

    std::vector<std::uint8_t> pixels, y, cb, cr;
    std::uint32_t w, h;
    BMP::LoadBitmap(argv[1], &pixels, &w, &h);

    Util::ConvertRGBToYCbCr(&pixels);

    Util::SplitChannels(pixels, &y, &cb, &cr);
    
    std::vector<Uint8Block> yBlocks;
    std::vector<Uint8Block> cbBlocks;
    std::vector<Uint8Block> crBlocks;

    Jpeg::WriteStartOfImage(s);
    Jpeg::WriteFrameHeader(s, std::uint16_t(w), std::uint16_t(h));
    Jpeg::WriteQuantizationTable(s, Jpeg::GetLuminanceQuantizationMatrix(), Jpeg::QUANT_LUMINANCE);
    Jpeg::WriteQuantizationTable(s, Jpeg::GetChrominanceQuantizationMatrix(), Jpeg::QUANT_CHROMINANCE);
    Jpeg::WriteHuffmanTable(s, encoder.GetLuminanceDCTable(), Jpeg::DC_TABLE, 0);
    Jpeg::WriteHuffmanTable(s, encoder.GetLuminanceACTable(), Jpeg::AC_TABLE, 0);
    Jpeg::WriteHuffmanTable(s, encoder.GetChrominanceDCTable(), Jpeg::DC_TABLE, 1);
    Jpeg::WriteHuffmanTable(s, encoder.GetChrominanceACTable(), Jpeg::AC_TABLE, 1);

    Util::SplitBlocks(y, w, h, &yBlocks);
    Util::SplitBlocks(cb, w, h, &cbBlocks);
    Util::SplitBlocks(cr, w, h, &crBlocks);

    for (int i = 0; i < yBlocks.size(); i++) {
        Int16Block result, result2;

        Jpeg::ComputeBlockDCT(yBlocks[i], &result);
        Jpeg::QuantizeLuminanceBlock(50, &result);  
        Jpeg::ReorderBlock(result, &result2);
        encoder.EncodeBlock(result2, Jpeg::COMPONENT_Y, &bits);

        Jpeg::ComputeBlockDCT(cbBlocks[i], &result);
        Jpeg::QuantizeChrominanceBlock(50, &result);  
        Jpeg::ReorderBlock(result, &result2);
        encoder.EncodeBlock(result2, Jpeg::COMPONENT_CB, &bits);

        Jpeg::ComputeBlockDCT(crBlocks[i], &result);
        Jpeg::QuantizeChrominanceBlock(50, &result);  
        Jpeg::ReorderBlock(result, &result2);
        encoder.EncodeBlock(result2, Jpeg::COMPONENT_CR, &bits);
    }

    Jpeg::WriteScan(s, bits);

    Jpeg::WriteEndOfImage(s);

    return 0;
}
