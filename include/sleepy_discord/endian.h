#pragma once
#include <utility>
#include <cstdint>
#include <cstring>
#include <array>
#if __cplusplus >= 202002L
#include <bit>
#define SLEEPY_ENDIAN_IF if constexpr
#else
#define SLEEPY_ENDIAN_IF if
#endif

namespace SleepyDiscord {

  enum class Endian
  {
    Little = 0,
    Big = 1,
    Unknown, // Could be PowerPC, which uses per-page endianness
  };

  constexpr Endian getSystemEndian() {
#ifdef __cpp_lib_endian
    return std::endian::native == std::endian::big ? Endian::Big :
      std::endian::native == std::endian::little ? Endian::Little :
      Endian::Unknown; // could be PowerPC
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return Endian::Little;
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    return Endian::Big;
#else
    return Endian::Unknown;
#endif
  }

  template<typename Byte = uint8_t>
  const std::array<Byte, 2> getByteOrder2() {
    const short bytes{ 0x0100 };
    const void* address{ static_cast<const void*>(&bytes) };
    const Byte* byteOrder{ static_cast<const Byte*>(address) };
    return std::array<Byte, 2>{byteOrder[0], byteOrder[1]};
  }

  template<typename Byte = uint8_t>
  const std::array<Byte, 8> getByteOrder8() {
    const long long bytes{ 0x0706050403020100 };
    const void* address{ static_cast<const void*>(&bytes) };
    const char* byteOrder{ static_cast<const char*>(address) };
    std::array<Byte, 8> output{};
    std::memcpy(output.data(), byteOrder, output.size());
    return output;
  }

  template<Endian endianness, typename Integer>
  struct EndianTyped {
    using IntegerType = Integer;
    Integer raw;
  };

  // you don't need to know the endianness, bitshifting works across endians

  template<typename Byte = uint8_t>
  uint16_t net2System16(const std::array<Byte, 2> value) {
    static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes large");
    constexpr Endian systemEndian = getSystemEndian();
    SLEEPY_ENDIAN_IF (Endian::Big == systemEndian) {
      uint16_t output;
      std::memcpy(&output, value.data(), value.size());
      return output;
    }

    return (
      ((static_cast<short>(value[1]) << 0) & static_cast<short>(0x00FF)) | \
      ((static_cast<short>(value[0]) << 8) & static_cast<short>(0xFF00))
    );
  }

  template<typename Byte = uint8_t>
  uint64_t net2System64(const std::array<Byte, 8> value) {
    static_assert(sizeof(uint64_t) == 8, "uint16_t must be 8 bytes large");
    constexpr Endian systemEndian = getSystemEndian();
    SLEEPY_ENDIAN_IF(Endian::Big == systemEndian) {
      uint64_t output;
      std::memcpy(&output, value.data(), value.size());
      return output;
    }

    return(
      ((static_cast<uint64_t>(value[7]) <<  0) & 0x00000000000000FFLL) | \
      ((static_cast<uint64_t>(value[6]) <<  8) & 0x000000000000FF00LL) | \
      ((static_cast<uint64_t>(value[5]) << 16) & 0x0000000000FF0000LL) | \
      ((static_cast<uint64_t>(value[4]) << 24) & 0x00000000FF000000LL) | \
      ((static_cast<uint64_t>(value[3]) << 32) & 0x000000FF00000000LL) | \
      ((static_cast<uint64_t>(value[2]) << 40) & 0x0000FF0000000000LL) | \
      ((static_cast<uint64_t>(value[1]) << 48) & 0x00FF000000000000LL) | \
      ((static_cast<uint64_t>(value[0]) << 56) & 0xFF00000000000000LL)
    );
  }

  template<typename Byte = uint8_t>
  std::array<Byte, 2> system2net16(const uint16_t value) {
    static_assert(sizeof(uint16_t) == 2, "uint16_t must be 8 bytes large");
    std::array<Byte, 2> output{};

    constexpr Endian systemEndian = getSystemEndian();
    SLEEPY_ENDIAN_IF (systemEndian == Endian::Big) {
      std::memcpy(output.data(), &value, output.size());
      return output;
    }

    output[1] = static_cast<Byte>((value << 0) & static_cast<short>(0x00FF));
    output[0] = static_cast<Byte>((value << 8) & static_cast<short>(0xFF00));
    return output;
  }

  template<typename Byte = uint8_t>
  std::array<Byte, 8> system2net64(const uint64_t value) {
    static_assert(sizeof(uint64_t) == 8, "uint16_t must be 8 bytes large");
    std::array<Byte, 8> output{};

    constexpr Endian systemEndian = getSystemEndian();
    SLEEPY_ENDIAN_IF (systemEndian == Endian::Big) {
      std::memcpy(output.data(), &value, output.size());
      return output;
    }

    output[7] = static_cast<Byte>((value <<  0) & 0x00000000000000FFLL);
    output[6] = static_cast<Byte>((value <<  8) & 0x000000000000FF00LL);
    output[5] = static_cast<Byte>((value << 16) & 0x0000000000FF0000LL);
    output[4] = static_cast<Byte>((value << 24) & 0x00000000FF000000LL);
    output[3] = static_cast<Byte>((value << 32) & 0x000000FF00000000LL);
    output[2] = static_cast<Byte>((value << 40) & 0x0000FF0000000000LL);
    output[1] = static_cast<Byte>((value << 48) & 0x00FF000000000000LL);
    output[0] = static_cast<Byte>((value << 56) & 0xFF00000000000000LL);
    return output;
  }

}