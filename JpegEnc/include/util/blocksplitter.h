#pragma once

#include "util/block.h"
#include <cstdint>
#include <vector>

namespace Util
{

class BlockSplitter
{
public:
    /**
    Constructor.

    @param[in]  Pixels  The array of pixels to read from.
    @param[in]  Width   Width of the image.
    @param[in]  Height  Height of the image.
    */
    BlockSplitter(const std::vector<std::uint8_t>& Pixels, int Width, int Height);

    /**
    Calculates the number of blocks in the image.

    @return Number of blocks in the image.
    */
    std::size_t GetNumBlocks() const
    {
        return m_NumBlocksPerRow * m_NumRows;
    }

    /**
    Extracts the i-th block of a channel.

    @param[in]  Index   The index of the block to extract.
    @param[in]  Channel The channel to extract the block from.
    @param[out] Block   The extracted block.
    */
    void ExtractBlock(std::size_t Index, int Channel, Uint8Block* Block);

private:
    std::size_t CalculateBlockOffset(int BlockX, int BlockY, int Channel);

    const std::vector<std::uint8_t>& m_Pixels;

    int m_Width;
    int m_Height;

    int m_NumBlocksPerRow;
    int m_NumRows;
};

}