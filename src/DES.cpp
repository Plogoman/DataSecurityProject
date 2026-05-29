
#include "DES.hpp"
#include "DESConstants.hpp"

#include <algorithm>

DES::DES(Block key) {
    generateRoundKeys(key);
}

DES::Block DES::encryptBlock(Block plainBlock) const {
    std::uint64_t permutedBlock = permute(
        plainBlock,
        DESConstants::INITIAL_PERMUTATION.data(),
        64,
        64
    );

    std::uint32_t left = static_cast<std::uint32_t>(permutedBlock >> 32);
    std::uint32_t right = static_cast<std::uint32_t>(permutedBlock & 0xFFFFFFFF);

    for (int round = 0; round < 16; ++round) {
        std::uint32_t previousRight = right;
        right = left ^ feistel(right, roundKeys[round]);
        left = previousRight;
    }

    std::uint64_t combined = (static_cast<std::uint64_t>(right) << 32) | left;

    return permute(
        combined,
        DESConstants::FINAL_PERMUTATION.data(),
        64,
        64
    );
}

DES::Block DES::decryptBlock(Block cipherBlock) const {
    std::uint64_t permutedBlock = permute(
        cipherBlock,
        DESConstants::INITIAL_PERMUTATION.data(),
        64,
        64
    );

    std::uint32_t left = static_cast<std::uint32_t>(permutedBlock >> 32);
    std::uint32_t right = static_cast<std::uint32_t>(permutedBlock & 0xFFFFFFFF);

    for (int round = 15; round >= 0; --round) {
        std::uint32_t previousRight = right;
        right = left ^ feistel(right, roundKeys[round]);
        left = previousRight;
    }

    std::uint64_t combined = (static_cast<std::uint64_t>(right) << 32) | left;

    return permute(
        combined,
        DESConstants::FINAL_PERMUTATION.data(),
        64,
        64
    );
}

void DES::generateRoundKeys(Block key) {
    std::uint64_t permutedKey = permute(
        key,
        DESConstants::PC1.data(),
        56,
        64
    );

    std::uint32_t left = static_cast<std::uint32_t>((permutedKey >> 28) & 0x0FFFFFFF);
    std::uint32_t right = static_cast<std::uint32_t>(permutedKey & 0x0FFFFFFF);

    for (int round = 0; round < 16; ++round) {
        left = leftRotate28(left, DESConstants::SHIFTS[round]);
        right = leftRotate28(right, DESConstants::SHIFTS[round]);

        std::uint64_t combined = (static_cast<std::uint64_t>(left) << 28) | right;

        roundKeys[round] = permute(
            combined,
            DESConstants::PC2.data(),
            48,
            56
        );
    }
}

std::uint64_t DES::permute(std::uint64_t input, const int* table, int tableSize, int inputSize) {
    std::uint64_t output = 0;

    for (int i = 0; i < tableSize; ++i) {
        output <<= 1;
        int sourcePosition = inputSize - table[i];
        output |= (input >> sourcePosition) & 1ULL;
    }

    return output;
}

std::uint32_t DES::feistel(std::uint32_t rightHalf, std::uint64_t roundKey) {
    std::uint64_t expanded = permute(
        static_cast<std::uint64_t>(rightHalf),
        DESConstants::EXPANSION_TABLE.data(),
        48,
        32
    );

    expanded ^= roundKey;

    std::uint32_t substituted = sBoxSubstitution(expanded);

    return static_cast<std::uint32_t>(
        permute(
            substituted,
            DESConstants::P_BOX.data(),
            32,
            32
        )
    );
}

std::uint32_t DES::sBoxSubstitution(std::uint64_t input) {
    std::uint32_t output = 0;

    for (int box = 0; box < 8; ++box) {
        std::uint8_t sixBits = static_cast<std::uint8_t>(
            (input >> (42 - box * 6)) & 0x3F
        );

        int row = ((sixBits & 0x20) >> 4) | (sixBits & 0x01);
        int column = (sixBits >> 1) & 0x0F;

        output <<= 4;
        output |= DESConstants::S_BOXES[box][row][column] & 0x0F;
    }

    return output;
}

std::uint32_t DES::leftRotate28(std::uint32_t value, int shifts) {
    value &= 0x0FFFFFFF;
    return ((value << shifts) | (value >> (28 - shifts))) & 0x0FFFFFFF;
}