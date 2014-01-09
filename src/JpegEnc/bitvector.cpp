#include "stdafx.h"
#include "bitvector.h"

namespace Util
{

BitVector::BitVector(void) :
    m_BitPosition(7), m_Storage(1), m_Size(0)
{
}


BitVector::~BitVector(void)
{
}

void BitVector::WriteBit(std::uint32_t Bit)
{
    assert((Bit & 1) == Bit);

    if (m_BitPosition < 0) {
        m_Storage.push_back(0);
        m_BitPosition = 7;
    }

    m_Storage.back() |= (Bit << m_BitPosition);

    m_BitPosition--;
    m_Size++;
}

void BitVector::WriteBits(std::uint32_t Bits, int NumBits)
{
    assert(NumBits <= 32 && NumBits >= 0);

    for (int i = NumBits - 1; i >= 0; i--) {
        WriteBit((Bits >> i) & 1);
    }
}

void BitVector::Clear()
{
    m_Storage.clear();
    m_Storage.push_back(0);
    m_BitPosition = 7;
    m_Size = 0;
}

}