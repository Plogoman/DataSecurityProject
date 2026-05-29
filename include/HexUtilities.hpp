#pragma once

#include <cstdint>
#include <string>

class HexUtils {
public:
    static bool isValidHex64(const std::string& value);
    static std::uint64_t hexToUint64(const std::string& value);
    static std::string uint64ToHex(std::uint64_t value);
    static std::string normalizeHex(std::string value);
};

