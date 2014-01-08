#pragma once

#include <cstdint>
#include <vector>

namespace Util
{

/**
A vector where each element is represented by a single bit.
Internally it is backed by 32-bit unsigned integers, with each bit stored
starting from the most significant bit of the integer.
*/
class BitVector
{
public:
    BitVector(void);
    ~BitVector(void);

    /**
    Writes multiple bits into the vector.

    @param[in]  Bits    The bit pattern to write into the vector.
    @param[in]  NumBits Number of bits to write.
    */
    void WriteBits(std::uint32_t Bits, int NumBits);

    /**
    Writes a single bit into the vector.

    @param[in]  Bit     The bit to write. Only values allowed are 0 or 1.
    */
    void WriteBit(std::uint32_t Bit);

    /**
    Gets the underlying storage object of the vector.
    Used for serializing the bit vector into a stream.

    @return A read-only reference to the storage object.
    */
    const std::vector<std::uint8_t>& GetStorage() const
    {
        return m_Storage;
    }

private:
    std::vector<std::uint8_t> m_Storage;
    int m_BitPosition;
};

}