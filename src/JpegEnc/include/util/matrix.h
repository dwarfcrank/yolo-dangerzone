#pragma once

#include "util/block.h"

namespace Util
{

/**
Performs 8x8 matrix multiplication such that Result = MatrixA * MatrixB.

@param[out] Result  The resulting matrix.
@param[in]  MatrixA The matrix on the left side of the multiplication.
@param[in]  MatrixB The matrix on the right side of the multiplication.
*/
template<typename TSrcValue1, typename TSrcValue2, typename TDestValue>
void MatrixMultiply(std::array<TDestValue, 64>* Result, const std::array<TSrcValue1, 64>& MatrixA,
                    const std::array<TSrcValue2, 64>& MatrixB)
{
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            TDestValue& value = Result->at(y * 8 + x);
            value = 0;

            for (int i = 0; i < 8; i++) {
                value += static_cast<TDestValue>(MatrixA[y * 8 + i] * MatrixB[i * 8 + x]);
            }
        }
    }
}

/**
Computes the transpose of a matrix which will replace the matrix passed to the function.

@param[in,out]  Matrix  The matrix to transpose.
*/
template<typename TValue>
void MatrixTranspose(std::array<TValue, 64>* Matrix)
{
    std::array<TValue, 64> transpose;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            transpose[x * 8 + y] = Matrix->at(y * 8 + x);
        }
    }

    Matrix->swap(transpose);
}

}