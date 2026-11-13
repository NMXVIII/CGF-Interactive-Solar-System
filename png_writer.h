#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace png_detail {

inline uint32_t crc32(const uint8_t* data, size_t n, uint32_t crc = 0)
{
    static uint32_t table[256];
    static bool ready = false;
    if (!ready) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int k = 0; k < 8; ++k)
                c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            table[i] = c;
        }
        ready = true;
    }
    crc = ~crc;
    for (size_t i = 0; i < n; ++i)
        crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    return ~crc;
}

inline void put32(std::vector<uint8_t>& v, uint32_t x)
{
    v.push_back(x >> 24);
    v.push_back(x >> 16);
    v.push_back(x >> 8);
    v.push_back(x);
}

inline void writeChunk(std::ofstream& out, const char type[4], const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> head;
    put32(head, static_cast<uint32_t>(data.size()));
    out.write(reinterpret_cast<const char*>(head.data()), 4);

    std::vector<uint8_t> body(type, type + 4);
    body.insert(body.end(), data.begin(), data.end());
    out.write(reinterpret_cast<const char*>(body.data()), body.size());

    std::vector<uint8_t> tail;
    put32(tail, crc32(body.data(), body.size()));
    out.write(reinterpret_cast<const char*>(tail.data()), 4);
}

}

inline bool writePNG(const std::string& path, int w, int h, const std::vector<uint8_t>& rgb)
{
    using namespace png_detail;

    std::vector<uint8_t> raw;
    raw.reserve(static_cast<size_t>(h) * (1 + 3 * w));
    for (int y = 0; y < h; ++y) {
        raw.push_back(0);
        raw.insert(raw.end(), rgb.begin() + static_cast<size_t>(y) * 3 * w,
                   rgb.begin() + static_cast<size_t>(y + 1) * 3 * w);
    }

    std::vector<uint8_t> z = {0x78, 0x01};
    size_t pos = 0;
    while (pos < raw.size()) {
        size_t n = std::min<size_t>(65535, raw.size() - pos);
        bool last = pos + n == raw.size();
        z.push_back(last ? 1 : 0);
        z.push_back(n & 0xFF);
        z.push_back(n >> 8);
        z.push_back(~n & 0xFF);
        z.push_back((~n >> 8) & 0xFF);
        z.insert(z.end(), raw.begin() + pos, raw.begin() + pos + n);
        pos += n;
    }
    uint32_t a = 1, b = 0;
    for (uint8_t c : raw) {
        a = (a + c) % 65521;
        b = (b + a) % 65521;
    }
    put32(z, (b << 16) | a);

    std::ofstream out(path, std::ios::binary);
    if (!out)
        return false;
    const uint8_t sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    out.write(reinterpret_cast<const char*>(sig), 8);

    std::vector<uint8_t> ihdr;
    put32(ihdr, w);
    put32(ihdr, h);
    ihdr.insert(ihdr.end(), {8, 2, 0, 0, 0});
    writeChunk(out, "IHDR", ihdr);
    writeChunk(out, "IDAT", z);
    writeChunk(out, "IEND", {});
    return static_cast<bool>(out);
}
