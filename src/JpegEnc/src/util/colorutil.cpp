#include "stdafx.h"
#include "util/colorutil.h"

namespace Util
{

void ConvertRGBToYCbCr(std::vector<std::uint8_t>* Pixels)
{
    assert(Pixels->size() % 3 == 0);

    for (auto it = Pixels->begin(); it != Pixels->end(); it += 3) {
        std::uint8_t b = *it;
        std::uint8_t g = *(it + 1);
        std::uint8_t r = *(it + 2);

        auto y  = static_cast<std::uint8_t>(  0 +    0.299f * r +    0.587f * g +    0.114f * b);
        auto cb = static_cast<std::uint8_t>(128 - 0.168736f * r - 0.331264f * g +      0.5f * b);
        auto cr = static_cast<std::uint8_t>(128 +      0.5f * r - 0.418688f * g - 0.081312f * b);

        *it = y;
        *(it + 1) = cb;
        *(it + 2) = cr;
    }
}

void SplitChannels(const std::vector<std::uint8_t>& Pixels, std::vector<std::uint8_t>* Channel1,
                   std::vector<std::uint8_t>* Channel2, std::vector<std::uint8_t>* Channel3)
{
    assert(Pixels.size() % 3 == 0);

    auto end = Pixels.cend();
    for (auto it = Pixels.cbegin(); it != end;) {
        Channel1->push_back(*it++);
        Channel2->push_back(*it++);
        Channel3->push_back(*it++);
    }
}

}