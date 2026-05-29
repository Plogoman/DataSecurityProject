# DES Encryption/Decryption CLI Tool

A command-line tool implementing the **Data Encryption Standard (DES)** from scratch in **C++20**. This project performs single-block DES encryption and decryption, accepting 64-bit (16 hex character) plaintext or ciphertext blocks and a 64-bit (16 hex character) key. Every stage of the DES algorithm — initial permutation, key schedule, Feistel rounds, S-box substitution, and final permutation — is implemented directly in C++ with no external cryptographic libraries.

---

## Features

- Full DES encryption and decryption of a single 64-bit block
- 16-round Feistel network with complete key schedule generation
- All standard DES components implemented from specification: IP/FP, E-expansion, S-boxes, P-permutation, PC-1/PC-2 permutations
- Hex string input and output for both the data block and the key
- Short-flag aliases (`-e`, `-d`) in addition to word commands (`encrypt`, `decrypt`)
- Case-insensitive hex input
- Clear error messages for invalid arguments or malformed input
- Zero external cryptographic dependencies — pure C++20

---

## Build Instructions

### Prerequisites

- CMake 3.16 or later
- A C++20-compatible compiler (GCC 10+, Clang 12+, or MSVC 19.29+)

### Steps

```bash
# Clone the repository
git clone <repository-url>
cd DataSecurityProject

# Create and enter the build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .
```

The compiled binary will be located at `build/DataSecurityProject` (or `build\DataSecurityProject.exe` on Windows).

---

## Usage

```
./DataSecurityProject encrypt <16-hex-input-block> <16-hex-key>
./DataSecurityProject decrypt <16-hex-cipher-block> <16-hex-key>
```

Short-flag aliases are also supported:

```
./DataSecurityProject -e <16-hex-input-block> <16-hex-key>
./DataSecurityProject -d <16-hex-cipher-block> <16-hex-key>
```

- `<16-hex-input-block>` / `<16-hex-cipher-block>` — exactly 16 hexadecimal characters representing the 64-bit data block
- `<16-hex-key>` — exactly 16 hexadecimal characters representing the 64-bit DES key

Both arguments are case-insensitive (`ABCDEF` and `abcdef` are equivalent).

---

## Encryption Example

Encrypt the plaintext block `0123456789ABCDEF` using the key `133457799BBCDFF1`:

```bash
./DataSecurityProject encrypt 0123456789ABCDEF 133457799BBCDFF1
```

**Output:**

```
Ciphertext: 85E813540F0AB405
```

---

## Decryption Example

Decrypt the ciphertext block `85E813540F0AB405` using the same key `133457799BBCDFF1`:

```bash
./DataSecurityProject decrypt 85E813540F0AB405 133457799BBCDFF1
```

**Output:**

```
Plaintext: 0123456789ABCDEF
```

---

## Input Format

All inputs are provided as **hexadecimal strings**:

| Field | Length | Represents |
|-------|--------|------------|
| Data block | 16 hex characters | 64-bit (8-byte) plaintext or ciphertext |
| Key | 16 hex characters | 64-bit (8-byte) DES key |

Each pair of hex characters encodes one byte. For example, the string `0123456789ABCDEF` encodes the 8 bytes `01 23 45 67 89 AB CD EF`.

> **Note on key parity bits:** The DES standard designates every 8th bit of the 64-bit key as a parity bit, yielding an effective key strength of 56 bits. This implementation accepts the full 64-bit key as-is; parity bits are included in the input but are discarded by the PC-1 permutation during key schedule generation, exactly as the standard specifies.

---

## Single-Block DES — Relationship to DES ECB

This tool performs **single-block DES**, which is exactly equivalent to **DES in ECB (Electronic Codebook) mode with no padding** applied to a single 64-bit block. ECB mode encrypts each block independently and identically; because only one block is processed here, no mode-of-operation logic is needed and the output is identical to what a standards-compliant DES-ECB implementation would produce for the same block and key.

This means the test vectors below can be verified against any compliant DES implementation (OpenSSL, CyberChef, NIST test suites) using the ECB mode, no padding, single block setting.

---

## Test Vectors

The following vectors can be used to verify correctness. Each can be confirmed with the encrypt and decrypt commands above.

### Vector 1 — Standard textbook example

| Field | Value |
|-------|-------|
| Plaintext | `0123456789ABCDEF` |
| Key | `133457799BBCDFF1` |
| Ciphertext | `85E813540F0AB405` |

### Vector 2 — All-zero plaintext and key

| Field | Value |
|-------|-------|
| Plaintext | `0000000000000000` |
| Key | `0000000000000000` |
| Ciphertext | `8CA64DE9C1B123A7` |

### Vector 3 — All-ones plaintext and key

| Field | Value |
|-------|-------|
| Plaintext | `FFFFFFFFFFFFFFFF` |
| Key | `FFFFFFFFFFFFFFFF` |
| Ciphertext | `7359B2163E4EDC58` |

### Vector 4 — All-zero plaintext, all-ones key

| Field | Value |
|-------|-------|
| Plaintext | `0000000000000000` |
| Key | `FFFFFFFFFFFFFFFF` |
| Ciphertext | `CAAAAF4DEAF1DBAE` |

### Vector 5 — All-ones plaintext, all-zero key

| Field | Value |
|-------|-------|
| Plaintext | `FFFFFFFFFFFFFFFF` |
| Key | `0000000000000000` |
| Ciphertext | `355550B2150E2451` |

---

## Design Overview

The implementation follows the DES specification (FIPS PUB 46-3) step by step:

1. **Input parsing** — The hex string arguments are decoded into 64-bit integers representing the plaintext/ciphertext block and the key.

2. **Key schedule** — The 64-bit key is passed through PC-1 (Permuted Choice 1), reducing it to 56 bits split into two 28-bit halves (C and D). Over 16 rounds, each half is rotated left by a round-dependent shift amount (1 or 2 positions). PC-2 (Permuted Choice 2) selects 48 bits from each rotated pair to produce 16 round subkeys.

3. **Initial Permutation (IP)** — The 64-bit plaintext block is rearranged according to the fixed IP table.

4. **16 Feistel Rounds** — The 64-bit block is split into 32-bit left (L) and right (R) halves. Each round applies the Feistel function `f(R, K)` to R using the round subkey K, then XORs the result with L. L and R are then swapped (except after the final round).

5. **Feistel function `f`:**
   - **E-expansion** — R is expanded from 32 to 48 bits.
   - **Key mixing** — The 48-bit expansion is XORed with the 48-bit round subkey.
   - **S-box substitution** — The 48 bits are split into eight 6-bit groups, each passed through one of the eight S-boxes, producing eight 4-bit outputs (32 bits total).
   - **P-permutation** — The 32-bit S-box output is permuted by the fixed P table.

6. **Final Permutation (FP)** — The inverse of IP is applied to the 64-bit output of the last round, producing the ciphertext.

7. **Decryption** — Identical to encryption, except the 16 round subkeys are applied in reverse order (K16 down to K1).

8. **Output** — The resulting 64-bit value is formatted as a 16-character uppercase hex string and printed to standard output.

---

## Limitations

- **Single block only** — The tool processes exactly one 64-bit block per invocation. Multi-block messages, stream processing, and padding schemes are not implemented.
- **No mode of operation** — Only the raw DES block cipher is implemented. Modes such as CBC, CTR, or GCM are outside the scope of this project.
- **No key stretching or derivation** — Keys are used directly as supplied; there is no PBKDF, salting, or key wrapping.
- **Not constant-time** — The implementation makes no attempt to resist timing side-channel attacks. It must not be used where side-channel resistance is required.
- **64-bit key input includes parity bits** — Parity checking is not enforced; any 64-bit value is accepted as a key.
