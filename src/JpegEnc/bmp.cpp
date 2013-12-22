#include "stdafx.h"
#include "bmp.h"

#include <fstream>
#include <cstring>

namespace
{

// These structures are not in bmp.h as they will not be exposed outside this file.
// Structure definitions taken from http://en.wikipedia.org/wiki/BMP_file_format

#pragma pack(1)	// Force 1-byte alignment.
struct BitmapFileHeader
{
    std::uint16_t   Magic;
    std::uint32_t   Filesize;
    std::uint16_t   Reserved1;
    std::uint16_t   Reserved2;
    std::uint32_t   PixelArrayOffset;
};

#pragma pack(1)	// Force 1-byte alignment.
struct BitmapInfoHeader
{
    std::uint32_t   HeaderSize;
    std::int32_t    Width;
    std::int32_t    Height;
    std::uint16_t   NumColorPlanes;
    std::uint16_t   NumBitsPerPixel;
    std::uint32_t   CompressionMethod;
    std::uint32_t   PixelDataSize;
    std::int32_t    HorizontalResolution;
    std::int32_t    VerticalResolution;
    std::uint32_t   NumPaletteColors;
    std::uint32_t   NumImportantColors;
};

}

namespace BMP
{

// The first two bytes of a BMP file are always 0x42 and 0x4D.
// 0x4D42 is the 16-bit little endian representation.
static const uint16_t BMP_MAGIC = 0x4D42;

// Only 24-bit images are supported.
static const uint32_t BYTES_PER_PIXEL = 3;

/**
Validates the bitmap file header.

@param[in] Header   The bitmap file header to be validated.

@return True if the file header is valid and the image can be loaded, otherwise false.
*/
static bool ValidateFileHeader(const BitmapFileHeader& Header)
{
    // TODO: Better validation. 
    return (Header.Magic == BMP_MAGIC);
}

/**
Validates the bitmap info header.

This function checks if the following requirements are met:
\li Header size is 40 bytes
\li Image width and height are positive
\li Number of color planes is 1
\li Number of bits per pixel is 242
\li The image is uncompressed.

@param[in] Header   The bitmap info header to be validated.

@return True if the requirements are met and the image can be loaded, otherwise false.
*/
static bool ValidateInfoHeader(const BitmapInfoHeader& Header)
{
    return (Header.HeaderSize == sizeof(BitmapInfoHeader))
        && (Header.Width > 0)               // Width and height can be negative, but
        && (Header.Height > 0)              // we don't support those either.
        && (Header.NumColorPlanes == 1)     // 1 is the only valid value according to Wikipedia.
        && (Header.NumBitsPerPixel == 24)   // Only 24-bit images are supported.
        && (Header.CompressionMethod == 0); // Nonzero means compression is used.
}

/**
Loads and checks both bitmap file and info headers.

@param[in] FileStream   The input file stream.
@param[out] FileHeader  The bitmap file header read from the file.
@param[out] InfoHeader  The bitmap info header read from the file.

@return One of #BitmapLoadStatus codes.
*/
static BitmapLoadStatus LoadAndValidateHeaders(std::ifstream& FileStream, BitmapFileHeader* FileHeader,
                                               BitmapInfoHeader* InfoHeader)
{
    FileStream.read(reinterpret_cast<char*>(FileHeader), sizeof(*FileHeader));
    if (FileStream.bad()) {
        return INVALID_FILE_HEADER;
    }

    if (!ValidateFileHeader(*FileHeader)) {
        return INVALID_FILE_HEADER;
    }

    FileStream.read(reinterpret_cast<char*>(InfoHeader), sizeof(*InfoHeader));
    if (FileStream.bad() || !ValidateInfoHeader(*InfoHeader)) {
        return INVALID_INFO_HEADER;
    }

    return LOAD_SUCCESS;
}

/**
Reads the pixel rows from the bitmap file.

@param[in] FileStream   The input file stream.
@param[out] DestBuffer  The buffer to write the pixels to.
@param[in] FileHeader   The bitmap file header.
@param[in] InfoHeader   The bitmap info header.

@return One of #BitmapLoadStatus codes.
*/
static BitmapLoadStatus LoadPixels(std::ifstream& FileStream, std::vector<std::uint8_t>* DestBuffer,
                                   const BitmapFileHeader& FileHeader, const BitmapInfoHeader& InfoHeader)
{
    FileStream.seekg(FileHeader.PixelArrayOffset);

    DestBuffer->resize(InfoHeader.PixelDataSize);
    auto buffer = reinterpret_cast<char*>(&DestBuffer->at(0));

    FileStream.read(buffer, InfoHeader.PixelDataSize);

    if (FileStream.gcount() != InfoHeader.PixelDataSize) {
        return UNEXPECTED_END_OF_FILE;
    }

    return LOAD_SUCCESS;
}

/**
Calculates the byte length of a single row.

The BMP format requires the length of each row to be a multiple of 4.

@param[in] InfoHeader   The bitmap info header.

@return Returns the length of a row in bytes, taking possible padding into account.
*/
static std::uint32_t CalculateStride(const BitmapInfoHeader& InfoHeader)
{
    auto result = InfoHeader.Width * BYTES_PER_PIXEL;

    if (result % 4 != 0) {
        result += (4 - (result % 4));
    }

    return result;
}

/**
Flips the image rows and removes padding from the end.

The BMP format stores the rows of pixels in the image in reverse order, so the topmost row
is the last one in the file. This function reverses the order and removes padding from the end.

@param[in,out] DestBuffer   Pointer to the buffer containing the image data.
@param[in] InfoHeader       The bitmap info header.
*/
static void FlipRowsAndRemovePadding(std::vector<std::uint8_t>* DestBuffer, const BitmapInfoHeader& InfoHeader)
{
    std::vector<std::uint8_t> reversed;

    size_t numRows = InfoHeader.Height;
    size_t rowLength = InfoHeader.Width * BYTES_PER_PIXEL;
    reversed.resize(numRows * rowLength);

    // Stride is needed to advance in the source buffer.
    size_t stride = CalculateStride(InfoHeader);

    std::uint8_t* dest = &reversed[0];
    const std::uint8_t* src = DestBuffer->data() + (numRows - 1) * stride;

    for (size_t i = numRows; i > 0; i--) {
        std::memcpy(dest, src, rowLength);
        dest += rowLength;
        src -= stride;
    }

    DestBuffer->swap(reversed);
}

BitmapLoadStatus LoadBitmap(const char* Filename, std::vector<std::uint8_t>* DestBuffer,
                            std::uint32_t* Width, std::uint32_t* Height)
{
    std::ifstream fileStream(Filename, std::ios::binary | std::ios::in);

    if (!fileStream) {
        return FILE_NOT_FOUND;
    }

    BitmapFileHeader fileHeader;
    BitmapInfoHeader infoHeader;

    auto status = LoadAndValidateHeaders(fileStream, &fileHeader, &infoHeader);
    if (status != LOAD_SUCCESS) {
        return status;
    }

    status = LoadPixels(fileStream, DestBuffer, fileHeader, infoHeader);
    if (status != LOAD_SUCCESS) {
        return status;
    }

    FlipRowsAndRemovePadding(DestBuffer, infoHeader);

    *Width = infoHeader.Width;
    *Height = infoHeader.Height;

    return LOAD_SUCCESS;
}

}