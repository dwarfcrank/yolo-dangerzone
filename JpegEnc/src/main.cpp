#include "stdafx.h"
#include "util/bmp.h"
#include <vector>
#include <cstdint>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include "util/block.h"
#include "jpeg/fdct.h"
#include "util/bitvector.h"
#include "jpeg/huffman.h"
#include "util/colorutil.h"
#include "jpeg/jpegfile.h"
#include "jpeg/quantization.h"
#include "util/blocksplitter.h"
#include <cstdio>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cout << "usage: " << argv[0] << " infile outfile quality" << std::endl;
        std::cout << "quality is in range 1-95" << std::endl;

        return 0;
    }
    
    Jpeg::HuffmanEncoder encoder;
    Util::BitVector bits;

    std::ofstream s(argv[2], std::ios::binary | std::ios::out);

    std::vector<std::uint8_t> pixels, y, cb, cr;
    std::uint32_t w, h;
    BMP::LoadBitmap(argv[1], &pixels, &w, &h);
    Util::ConvertRGBToYCbCr(&pixels);

    Util::BlockSplitter splitter(pixels, w, h);
    
    int quality = 50;
    std::istringstream is(argv[3]);
    
    is >> quality;

    if (quality <= 0 || quality > 95) {
        std::cout << "quality must be in range  1-95" << std::endl;

        return 0;
    }

    Jpeg::WriteStartOfImage(s);
    Jpeg::WriteJFIFHeader(s);
    Jpeg::WriteQuantizationTable(s, Jpeg::GetLuminanceQuantizationMatrix(), Jpeg::QUANT_LUMINANCE, quality);
    Jpeg::WriteQuantizationTable(s, Jpeg::GetChrominanceQuantizationMatrix(), Jpeg::QUANT_CHROMINANCE, quality);
    Jpeg::WriteFrameHeader(s, std::uint16_t(w), std::uint16_t(h));
    Jpeg::WriteHuffmanTable(s, encoder.GetLuminanceDCTable(), Jpeg::DC_TABLE, 0);
    Jpeg::WriteHuffmanTable(s, encoder.GetLuminanceACTable(), Jpeg::AC_TABLE, 0);
    Jpeg::WriteHuffmanTable(s, encoder.GetChrominanceDCTable(), Jpeg::DC_TABLE, 1);
    Jpeg::WriteHuffmanTable(s, encoder.GetChrominanceACTable(), Jpeg::AC_TABLE, 1);

    auto blockCount = splitter.GetNumBlocks();

    Int16Block result, result2;
    Uint8Block block;
    for (std::size_t i = 0; i < blockCount; i++) {
        splitter.ExtractBlock(i, Jpeg::COMPONENT_Y, &block);
        Jpeg::ComputeBlockDCT(block, &result);
        Jpeg::QuantizeLuminanceBlock(quality, &result);  
        Jpeg::ReorderBlock(result, &result2);
        encoder.EncodeBlock(result2, Jpeg::COMPONENT_Y, &bits);

        splitter.ExtractBlock(i, Jpeg::COMPONENT_CB, &block);
        Jpeg::ComputeBlockDCT(block, &result);
        Jpeg::QuantizeChrominanceBlock(quality, &result);  
        Jpeg::ReorderBlock(result, &result2);
        encoder.EncodeBlock(result2, Jpeg::COMPONENT_CB, &bits);

        splitter.ExtractBlock(i, Jpeg::COMPONENT_CR, &block);
        Jpeg::ComputeBlockDCT(block, &result);
        Jpeg::QuantizeChrominanceBlock(quality, &result);  
        Jpeg::ReorderBlock(result, &result2);
        encoder.EncodeBlock(result2, Jpeg::COMPONENT_CR, &bits);
    }

    Jpeg::WriteScan(s, bits);

    Jpeg::WriteEndOfImage(s);

    return 0;
}
