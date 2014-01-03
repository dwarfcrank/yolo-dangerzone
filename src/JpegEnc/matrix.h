#pragma once

#include "block.h"

namespace Util
{

/**
Performs 8x8 matrix multiplication such that Result = MatrixA * MatrixB.
Each matrix must be a type based on std::array with 64 elements.

@param[out] Result  The resulting matrix.
@param[in]  MatrixA The matrix on the left side of the multiplication.
@param[in]  MatrixB The matrix on the right side of the multiplication.
*/
template<typename TSrcMatrix1, typename TSrcMatrix2, typename TDestMatrix>
void MatrixMultiply(TDestMatrix* Result, const TSrcMatrix1& MatrixA, const TSrcMatrix2& MatrixB)
{
    typedef TDestMatrix::value_type ResultType;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            ResultType& value = Result->at(y * 8 + x);
            value = 0;

            for (int i = 0; i < 8; i++) {
                value += static_cast<ResultType>(MatrixA[y * 8 + i] * MatrixB[i * 8 + x]);
            }
        }
    }
}

/**
Computes the transpose of a matrix which will replace the matrix passed to the function.
The matrix must be a type based on std::array with 64 elements.

@param[in,out]  Matrix  The matrix to transpose.
*/
template<typename TMatrix>
void MatrixTranspose(TMatrix* Matrix)
{
    TMatrix transpose;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            transpose[x * 8 + y] = Matrix->at(y * 8 + x);
        }
    }

    Matrix->swap(transpose);
}

}