#include <cstdint>
#include <vector>

namespace BMP
{

/**
Bitmap load error codes.
*/
enum BitmapLoadStatus
{
	LOAD_SUCCESS = 0,

	FILE_NOT_FOUND,
	UNEXPECTED_END_OF_FILE,

	INVALID_FILE_HEADER,
	INVALID_INFO_HEADER,
};

/**
Reads pixels from a bitmap file.

The resulting pixel data will be flipped upside down so that the pixel at (0, 0) will
be stored first in the buffer. Only 24-bit uncompressed BMP files are supported.

@param[in] Filename			Path of the file to load the image data from.
@param[in,out] DestBuffer	Pointer to a buffer to store the pixel data into.
@param[out]	Width			Pointer to a variable receiving the image width.
@param[out]	Height			Pointer to a variable receiving the image height.

@return	One of #BitmapLoadStatus codes.
*/
BitmapLoadStatus LoadBitmap(const char* Filename, std::vector<std::uint8_t>* DestBuffer,
							std::uint32_t* Width, std::uint32_t* Height);

}