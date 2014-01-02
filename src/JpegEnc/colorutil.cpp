#include "stdafx.h"
#include "colorutil.h"

namespace Util
{

std::vector<uint8_t> ConvertRGBToYCbCr(const std::vector<std::uint8_t>& Pixels)
{
    assert(Pixels.size() % 3 == 0);

    std::vector<uint8_t> result(Pixels.size());

    auto end = Pixels.cend();
    for (auto it = Pixels.cbegin(); it != end;) {
        std::uint8_t r = *it++;
        std::uint8_t g = *it++;
        std::uint8_t b = *it++;

        auto y  = static_cast<std::uint8_t>(  0 +    0.299f * r +    0.587f * g +    0.114f * b);
        auto cb = static_cast<std::uint8_t>(128 - 0.168736f * r - 0.331264f * g +      0.5f * b);
        auto cr = static_cast<std::uint8_t>(128 +      0.5f * r - 0.418688f * g - 0.081312f * b);

        result.push_back(y);
        result.push_back(cb);
        result.push_back(cr);
    }

    return result;
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