#include "stdafx.h"
#include "block.h"

#include <cmath>

/**
Calculates the number of 8x8 blocks per row and number of rows from image dimensions.

@param[in]  Width           Width of the image.
@param[in]  Height          Height of the image.
@param[out] BlocksPerRow    Number of blocks per row.
@param[out] RowCount        Number of rows.
*/
static void CalculateBlockCount(int Width, int Height, int* BlocksPerRow, int* RowCount)
{
    *BlocksPerRow = Width / 8;
    if (Width % 8 != 0) {
        (*BlocksPerRow)++;
    }

    *RowCount = Height / 8;
    if (Height % 8 != 0) {
        (*RowCount)++;
    }
}

/**
Repeats edge pixels of the block to fill the whole 8-pixel rows/columns.

@param[in]  BlockWidth  The width of the existing image data in the block.
@param[in]  BlockHeight The height of the existing image data in the block.
@param[in,out]  Result  The block to complete.
*/
static void RepeatEdgePixels(int BlockWidth, int BlockHeight, Uint8Block* Result)
{
    Uint8Block& block = *Result;

    if (BlockWidth < 8) {
        for (int y = 0; y < BlockHeight; y++) {
            std::uint8_t edgePixel = block[y * 8 + BlockWidth - 1];

            for (int x = BlockWidth; x < 8; x++) {
                block[y * 8 + x] = edgePixel;
            }
        }
    }
    
    if (BlockHeight < 8) {
        for (int x = 0; x < BlockWidth; x++) {
            std::uint8_t edgePixel = block[(BlockHeight - 1) * 8 + x];

            for (int y = BlockHeight; y < 8; y++) {
                block[y * 8 + x] = edgePixel;
            }
        }
    }
}

/**
Extracts a single 8x8 block from the pixel data, repeating edge pixels if necessary.
If BlockWidth or BlockHeight is less than 8, edge pixels will be repeated.

@param[in]  Pixels      The image data to extract the block from.
@param[in]  Stride      Number of bytes to advance in the buffer until the next row of pixels.
@param[in]  NumRows     Number of rows in the image.
@param[in]  BlockWidth  Width of the data to be copied into the destination block.
@param[in]  BlockHeight Height of the data to be copied into the destination block.
@param[out] Result      The 8x8 block extracted from the image data.
*/
static void ExtractBlock(const std::uint8_t* Pixels, int Stride, int NumRows,
                         int BlockWidth, int BlockHeight, Uint8Block* Result)
{
    Uint8Block& block = *Result;

    for (int y = 0; y < BlockHeight; y++) {
        for (int x = 0; x < BlockWidth; x++) {
            block[y * 8 + x] = Pixels[x];
        }

        Pixels += Stride;
    }

    if (BlockWidth < 8 || BlockHeight < 8) {
        RepeatEdgePixels(BlockWidth, BlockHeight, Result);
    }
}

namespace Util
{

void SplitBlocks(const std::vector<uint8_t>& Pixels, int Width, int Height, std::vector<Uint8Block>* Result)
{
    assert(Width > 0 && Height > 0);

    int blocksPerRow, rowCount;
    CalculateBlockCount(Width, Height, &blocksPerRow, &rowCount);

    Result->resize(blocksPerRow * rowCount);
    
    for (int y = 0; y < rowCount; y++) {
        for (int x = 0; x < blocksPerRow; x++) {
            int blockIndex = y * blocksPerRow + x;
            int blockWidth = std::min(8, Width - x * 8);
            int blockHeight = std::min(8, Height - y * 8);

            Uint8Block* block = &Result->at(blockIndex);
            const std::uint8_t* buffer = Pixels.data() + (y * 8 * Width + x * 8);

            ExtractBlock(buffer, Width, rowCount, blockWidth, blockHeight, block);
        }
    }
}

}

static const int ZigzagIndices[] =
{
     0,  1,  5,  6, 14, 15, 27, 28,
     2,  4,  7, 13, 16, 26, 29, 42,
     3,  8, 12, 17, 25, 30, 41, 43,
     9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63,
};

namespace Jpeg
{

void ReorderBlock(const Int16Block& Block, Int16Block* Result)
{
    Int16Block& result = *Result;

    for (int i = 0; i < 64; i++) {
        int index = ZigzagIndices[i];
        result[index] = Block[i];
    }
}

}