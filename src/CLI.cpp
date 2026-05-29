#include "CLI.hpp"

#include "DES.hpp"
#include "HexUtilities.hpp"

#include <exception>
#include <iostream>
#include <string>

int CLI::run(int argc, char* argv[]) const {
    if (argc != 4) {
        printUsage();
        return 1;
    }

    const std::string mode = argv[1];
    const std::string inputText = argv[2];
    const std::string keyText = argv[3];

    if (!HexUtils::isValidHex64(inputText)) {
        std::cerr << "Error: input block must be exactly 16 hexadecimal characters.\n";
        return 1;
    }

    if (!HexUtils::isValidHex64(keyText)) {
        std::cerr << "Error: key must be exactly 16 hexadecimal characters.\n";
        return 1;
    }

    try {
        const std::uint64_t inputBlock = HexUtils::hexToUint64(inputText);
        const std::uint64_t key = HexUtils::hexToUint64(keyText);

        DES des(key);

        if (mode == "encrypt" || mode == "-e" || mode == "--encrypt") {
            const std::uint64_t cipherBlock = des.encryptBlock(inputBlock);
            std::cout << HexUtils::uint64ToHex(cipherBlock) << '\n';
            return 0;
        }

        if (mode == "decrypt" || mode == "-d" || mode == "--decrypt") {
            const std::uint64_t plainBlock = des.decryptBlock(inputBlock);
            std::cout << HexUtils::uint64ToHex(plainBlock) << '\n';
            return 0;
        }

        std::cerr << "Error: unknown mode '" << mode << "'.\n";
        printUsage();
        return 1;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

void CLI::printUsage() {
    std::cout
        << "DES Encryption/Decryption CLI Tool\n\n"
        << "Usage:\n"
        << "  DataSecurityProject encrypt <16-hex-input-block> <16-hex-key>\n"
        << "  DataSecurityProject decrypt <16-hex-cipher-block> <16-hex-key>\n\n"
        << "Aliases:\n"
        << "  -e, --encrypt\n"
        << "  -d, --decrypt\n\n"
        << "Examples:\n"
        << "  DataSecurityProject encrypt 0123456789ABCDEF 133457799BBCDFF1\n"
        << "  DataSecurityProject decrypt 85E813540F0AB405 133457799BBCDFF1\n";
}