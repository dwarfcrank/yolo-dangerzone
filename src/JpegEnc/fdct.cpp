#include "stdafx.h"
#include "fdct.h"
#include "matrix.h"

#include <cmath>

typedef std::array<float, 64> FloatBlock;

/**
Shifts the range of each element in the block from [0, 255] to [-128, 127].

@param[in]  Source  The block to perform the range shift on.
@param[out] Destination The resulting level shifted block.
*/
static void ShiftLevels(const Uint8Block& Source, FloatBlock* Destination)
{
    FloatBlock& destination = *Destination;

    for (int i = 0; i < 64; i++) {
        destination[i] = static_cast<float>(Source[i]) - 128.0f;
    }
}

static FloatBlock DCTMatrix;
static FloatBlock DCTMatrixTranspose;

static const float Pi = 3.141592654f;

/**
Precomputes a DCT matrix to speed up per-block DCT computation.
*/
static void PrecomputeDCTMatrix()
{
    const float sqrt2N = std::sqrt(2.0f / 8.0f);
    const float invSqrt8 = 1.0f / std::sqrt(8.0f);

    // Each element on the first row will always be 1/sqrt(8).
    for (int x = 0; x < 8; x++) {
        DCTMatrix[x] = invSqrt8;
    }

    for (int y = 1; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            const float t = ((2.0f * x + 1.0f) * y * Pi) / 16.0f;
            DCTMatrix[y * 8 + x] = sqrt2N * std::cos(t);
        }
    }

    DCTMatrixTranspose = DCTMatrix;
    Util::MatrixTranspose(&DCTMatrixTranspose);
}

namespace Jpeg
{

void ComputeBlockDCT(const Uint8Block& Source, Int16Block* Destination)
{
    static bool dctMatrixComputed;

    if (!dctMatrixComputed) {
        PrecomputeDCTMatrix();
        dctMatrixComputed = true;
    }

    FloatBlock temp, temp2;

    ShiftLevels(Source, &temp);

    Util::MatrixMultiply(&temp2, DCTMatrix, temp);
    Util::MatrixMultiply(Destination, temp2, DCTMatrixTranspose);
}

}