#include "stdafx.h"
#include "util/blocksplitter.h"
#include <cmath>

static const int BLOCK_WIDTH = 8;
static const int BLOCK_HEIGHT = 8;
static const int NUM_CHANNELS = 3;

/**
Calculates the number of 8x8 blocks per row and number of rows from image dimensions.

@param[in]  Width           Width of the image.
@param[in]  Height          Height of the image.
@param[out] BlocksPerRow    Number of blocks per row.
@param[out] NumRows         Number of rows.
*/
static void CalculateBlockCount(int Width, int Height, int* NumBlocksPerRow, int* NumRows)
{
    *NumBlocksPerRow = Width / 8;
    if (Width % 8 != 0) {
        (*NumBlocksPerRow)++;
    }

    *NumRows = Height / 8;
    if (Height % 8 != 0) {
        (*NumRows)++;
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

namespace Util
{

BlockSplitter::BlockSplitter(const std::vector<std::uint8_t>& Pixels, int Width, int Height) :
    m_Pixels(Pixels), m_Width(Width), m_Height(Height),
    m_NumBlocksPerRow(0), m_NumRows(0)
{
    CalculateBlockCount(Width, Height, &m_NumBlocksPerRow, &m_NumRows);
}

std::size_t BlockSplitter::CalculateBlockOffset(int BlockX, int BlockY, int Channel)
{
    return BlockY * BLOCK_HEIGHT * m_Width * NUM_CHANNELS + BlockX * BLOCK_WIDTH * NUM_CHANNELS + Channel;
}

void BlockSplitter::ExtractBlock(std::size_t Index, int Channel, Uint8Block* Block)
{
    Uint8Block& block = *Block;

    int blockX = Index % m_NumBlocksPerRow;
    int blockY = Index / m_NumBlocksPerRow;
    int blockWidth = std::min(8, m_Width - blockX * BLOCK_WIDTH);
    int blockHeight = std::min(8, m_Height - blockY * BLOCK_HEIGHT);

    std::vector<std::uint8_t>::const_iterator data = m_Pixels.cbegin();
    data += CalculateBlockOffset(blockX, blockY, Channel);

    for (int y = 0; y < blockHeight; y++) {
        for (int x = 0; x < blockWidth; x++) {
            block[y * BLOCK_WIDTH + x] = *(data + x * NUM_CHANNELS);
        }

        // Bounds checked iterators will break here without this if...
        if (y < blockHeight - 1) {
            data += m_Width * NUM_CHANNELS;
        }
    }

    if (blockWidth < BLOCK_WIDTH || blockHeight < BLOCK_HEIGHT) {
        RepeatEdgePixels(blockWidth, blockHeight, Block);
    }
}

}
