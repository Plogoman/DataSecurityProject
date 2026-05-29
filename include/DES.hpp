#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <vector>

class DES {
public:
    using Block = std::uint64_t;

    explicit DES(Block key);

    Block encryptBlock(Block plainBlock) const;
    Block decryptBlock(Block cipherBlock) const;

private:
    std::array<std::uint64_t, 16> roundKeys{};

    void generateRoundKeys(Block key);

    static std::uint64_t permute(std::uint64_t input, const int* table, int tableSize, int inputSize);
    static std::uint32_t feistel(std::uint32_t rightHalf, std::uint64_t roundKey);
    static std::uint32_t sBoxSubstitution(std::uint64_t input);
    static std::uint32_t leftRotate28(std::uint32_t value, int shifts);
};
