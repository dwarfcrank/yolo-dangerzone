#include "stdafx.h"
#include "jpeg/huffman.h"

/**
These tables are precomputed from various images and are taken directly
from the JPEG spec. The array names BITS and HUFFVAL are also directly from
the spec.
*/
namespace Luminance
{
    namespace DC
    {
        const std::array<int, 16> BITS =
        {
            0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        };

        const int HUFFVAL[] =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 
        };
    }

    namespace AC
    {
        const std::array<int, 16> BITS =
        {
            0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 125,
        };

        const int HUFFVAL[] =
        {
              1,   2,   3,   0,   4,  17,   5,  18,
             33,  49,  65,   6,  19,  81,  97,   7,
             34, 113,  20,  50, 129, 145, 161,   8,
             35,  66, 177, 193,  21,  82, 209, 240,
             36,  51,  98, 114, 130,   9,  10,  22,
             23,  24,  25,  26,  37,  38,  39,  40,
             41,  42,  52,  53,  54,  55,  56,  57,
             58,  67,  68,  69,  70,  71,  72,  73,
             74,  83,  84,  85,  86,  87,  88,  89,
             90,  99, 100, 101, 102, 103, 104, 105,
            106, 115, 116, 117, 118, 119, 120, 121,
            122, 131, 132, 133, 134, 135, 136, 137,
            138, 146, 147, 148, 149, 150, 151, 152,
            153, 154, 162, 163, 164, 165, 166, 167,
            168, 169, 170, 178, 179, 180, 181, 182,
            183, 184, 185, 186, 194, 195, 196, 197,
            198, 199, 200, 201, 202, 210, 211, 212,
            213, 214, 215, 216, 217, 218, 225, 226,
            227, 228, 229, 230, 231, 232, 233, 234,
            241, 242, 243, 244, 245, 246, 247, 248,
            249, 250, 
        };
    }
}

namespace Chrominance
{
    namespace DC
    {
        const std::array<int, 16> BITS =
        {
            0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        };

        const int HUFFVAL[] =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        };
    }

    namespace AC
    {
        const std::array<int, 16> BITS =
        {
            0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119, 
        };

        const int HUFFVAL[] =
        {
              0,   1,   2,   3,  17,   4,   5,  33,
             49,   6,  18,  65,  81,   7,  97, 113,
             19,  34,  50, 129,   8,  20,  66, 145,
            161, 177, 193,   9,  35,  51,  82, 240,
             21,  98, 114, 209,  10,  22,  36,  52,
            225,  37, 241,  23,  24,  25,  26,  38,
             39,  40,  41,  42,  53,  54,  55,  56,
             57,  58,  67,  68,  69,  70,  71,  72,
             73,  74,  83,  84,  85,  86,  87,  88,
             89,  90,  99, 100, 101, 102, 103, 104,
            105, 106, 115, 116, 117, 118, 119, 120,
            121, 122, 130, 131, 132, 133, 134, 135,
            136, 137, 138, 146, 147, 148, 149, 150,
            151, 152, 153, 154, 162, 163, 164, 165,
            166, 167, 168, 169, 170, 178, 179, 180,
            181, 182, 183, 184, 185, 186, 194, 195,
            196, 197, 198, 199, 200, 201, 202, 210,
            211, 212, 213, 214, 215, 216, 217, 218,
            226, 227, 228, 229, 230, 231, 232, 233,
            234, 242, 243, 244, 245, 246, 247, 248,
            249, 250, 
        };
    }
}

/**
Generates Huffman codes for a set of values.

@param[out] Table   The resulting Huffman table.
@param[in]  BITS    An array containing the number of codes for each code length.
@param[in]  HUFFVAL The values to generate codes for.
*/
static void GenerateHuffmanTable(Jpeg::HuffmanTable* Table, const std::array<int, 16>& BITS, const int* HUFFVAL)
{
    int totalCount = 0;

    for (int i = 0; i < 16; i++) {
        totalCount += BITS[i];
    }

    std::vector<Jpeg::HuffmanCode> codes(totalCount);

    std::uint16_t code = 0;
    int index = 0;

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < BITS[i]; j++) {
            Jpeg::HuffmanCode& huffcode = codes[index];

            huffcode.Code = code;
            huffcode.Length = i + 1;

            index++;
            code++;
        }

        code <<= 1;
        Table->NumCodes[i] = BITS[i];
    }

    for (int i = 0; i < totalCount; i++) {
        int value = HUFFVAL[i];

        Table->Codes[value] = codes[i];
        Table->Values[i] = value;
    }
}

namespace Jpeg
{
    
void HuffmanEncoder::GenerateLuminanceTables()
{
    GenerateHuffmanTable(&m_LuminanceACTable, Luminance::AC::BITS, Luminance::AC::HUFFVAL);
    GenerateHuffmanTable(&m_LuminanceDCTable, Luminance::DC::BITS, Luminance::DC::HUFFVAL);
}

void HuffmanEncoder::GenerateChrominanceTables()
{
    GenerateHuffmanTable(&m_ChrominanceACTable, Chrominance::AC::BITS, Chrominance::AC::HUFFVAL);
    GenerateHuffmanTable(&m_ChrominanceDCTable, Chrominance::DC::BITS, Chrominance::DC::HUFFVAL);
}

}