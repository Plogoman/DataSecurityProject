#include "HexUtilities.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

bool HexUtils::isValidHex64(const std::string& value) {
    std::string normalized = normalizeHex(value);

    if (normalized.size() != 16) {
        return false;
    }

    return std::all_of(
        normalized.begin(),
        normalized.end(),
        [](unsigned char character) {
            return std::isxdigit(character);
        }
    );
}

std::uint64_t HexUtils::hexToUint64(const std::string& value) {
    std::string normalized = normalizeHex(value);

    if (!isValidHex64(normalized)) {
        throw std::invalid_argument("Value must be exactly 16 hexadecimal characters.");
    }

    std::uint64_t result = 0;
    std::stringstream stream;
    stream << std::hex << normalized;
    stream >> result;

    return result;
}

std::string HexUtils::uint64ToHex(std::uint64_t value) {
    std::ostringstream stream;
    stream << std::uppercase
           << std::hex
           << std::setw(16)
           << std::setfill('0')
           << value;

    return stream.str();
}

std::string HexUtils::normalizeHex(std::string value) {
    if (value.starts_with("0x") || value.starts_with("0X")) {
        value = value.substr(2);
    }

    value.erase(
        std::remove_if(
            value.begin(),
            value.end(),
            [](unsigned char character) {
                return std::isspace(character);
            }
        ),
        value.end()
    );

    return value;
}