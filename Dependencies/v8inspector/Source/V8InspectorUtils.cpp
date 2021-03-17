// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// This code is based on the old node inspector implementation. See NOTICE.md for Node.js' project license details

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "V8InspectorUtils.h"
#include <V8Inc.h>

#include <array>
#include <random>
#include <stdexcept>
#include <codecvt>

namespace Babylon {
namespace utils {

// These are copied from react-native code.
const uint16_t kUtf8OneByteBoundary = 0x80;
const uint16_t kUtf8TwoBytesBoundary = 0x800;
const uint16_t kUtf16HighSubLowBoundary = 0xD800;
const uint16_t kUtf16HighSubHighBoundary = 0xDC00;
const uint16_t kUtf16LowSubHighBoundary = 0xE000;

size_t utf16toUTF8Length(const uint16_t* utf16String, size_t utf16StringLen) {
  if (!utf16String || utf16StringLen == 0) {
    return 0;
  }

  uint32_t utf8StringLen = 0;
  auto utf16StringEnd = utf16String + utf16StringLen;
  auto idx16 = utf16String;
  while (idx16 < utf16StringEnd) {
    auto ch = *idx16++;
    if (ch < kUtf8OneByteBoundary) {
      utf8StringLen++;
    }
    else if (ch < kUtf8TwoBytesBoundary) {
      utf8StringLen += 2;
    }
    else if (
      (ch >= kUtf16HighSubLowBoundary) && (ch < kUtf16HighSubHighBoundary) &&
      (idx16 < utf16StringEnd) &&
      (*idx16 >= kUtf16HighSubHighBoundary) && (*idx16 < kUtf16LowSubHighBoundary)) {
      utf8StringLen += 4;
      idx16++;
    }
    else {
      utf8StringLen += 3;
    }
  }

  return utf8StringLen;
}

std::string utf16toUTF8(const uint16_t* utf16String, size_t utf16StringLen) noexcept {
  if (!utf16String || utf16StringLen <= 0) {
    return "";
  }

  std::string utf8String(utf16toUTF8Length(utf16String, utf16StringLen), '\0');
  auto idx8 = utf8String.begin();
  auto idx16 = utf16String;
  auto utf16StringEnd = utf16String + utf16StringLen;
  while (idx16 < utf16StringEnd) {
    auto ch = *idx16++;
    if (ch < kUtf8OneByteBoundary) {
      *idx8++ = (ch & 0x7F);
    }
    else if (ch < kUtf8TwoBytesBoundary) {
#ifdef _MSC_VER
#pragma warning(suppress: 4244)
      *idx8++ = 0b11000000 | (ch >> 6);
#else
      *idx8++ = 0b11000000 | (ch >> 6);
#endif
      *idx8++ = 0b10000000 | (ch & 0x3F);
    }
    else if (
      (ch >= kUtf16HighSubLowBoundary) && (ch < kUtf16HighSubHighBoundary) &&
      (idx16 < utf16StringEnd) &&
      (*idx16 >= kUtf16HighSubHighBoundary) && (*idx16 < kUtf16LowSubHighBoundary)) {
      auto ch2 = *idx16++;
      uint8_t trunc_byte = (((ch >> 6) & 0x0F) + 1);
      *idx8++ = 0b11110000 | (trunc_byte >> 2);
      *idx8++ = 0b10000000 | ((trunc_byte & 0x03) << 4) | ((ch >> 2) & 0x0F);
      *idx8++ = 0b10000000 | ((ch & 0x03) << 4) | ((ch2 >> 6) & 0x0F);
      *idx8++ = 0b10000000 | (ch2 & 0x3F);
    }
    else {
      *idx8++ = 0b11100000 | (ch >> 12);
      *idx8++ = 0b10000000 | ((ch >> 6) & 0x3F);
      *idx8++ = 0b10000000 | (ch & 0x3F);
    }
  }

  return utf8String;
}

std::wstring Utf8ToUtf16(const char* utf8, size_t utf8Len)
{
  std::wstring utf16{};

  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(utf8, utf8 + (utf8Len - 1));
}

std::string StringViewToUtf8(const v8_inspector::StringView& view)
{
    if (view.is8Bit())
    {
        return std::string(
            reinterpret_cast<const char*>(view.characters8()), view.length());
    }

    return Babylon::utils::utf16toUTF8(view.characters16(), view.length());
}

std::unique_ptr<v8_inspector::StringBuffer> Utf8ToStringView(
    const std::string& message)
{
    std::wstring wstr =
        Babylon::utils::Utf8ToUtf16(message.data(), message.length() + 1);
    v8_inspector::StringView view(
        reinterpret_cast<const uint16_t*>(wstr.c_str()), wstr.length());
    return v8_inspector::StringBuffer::create(view);
}

char ToLower(char c) {
  return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

std::string ToLower(const std::string& in) {
  std::string out(in.size(), 0);
  for (size_t i = 0; i < in.size(); ++i)
    out[i] = ToLower(in[i]);
  return out;
}

bool StringEqualNoCase(const char* a, const char* b) {
  do {
    if (*a == '\0')
      return *b == '\0';
    if (*b == '\0')
      return *a == '\0';
  } while (ToLower(*a++) == ToLower(*b++));
  return false;
}

bool StringEqualNoCaseN(const char* a, const char* b, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (ToLower(a[i]) != ToLower(b[i]))
      return false;
    if (a[i] == '\0')
      return true;
  }
  return true;
}

size_t base64_encode(const char* src, size_t slen, char* dst, size_t dlen) {
  // We know how much we'll write, just make sure that there's space.
  // CHECK(dlen >= base64_encoded_size(slen) && "not enough space provided for base64 encode");

  dlen = base64_encoded_size(slen);

  unsigned a;
  unsigned b;
  unsigned c;
  unsigned i;
  unsigned k;
  unsigned n;

  static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

  i = 0;
  k = 0;
  n = static_cast<int>(slen) / 3 * 3;

  while (i < n) {
    a = src[i + 0] & 0xff;
    b = src[i + 1] & 0xff;
    c = src[i + 2] & 0xff;

    dst[k + 0] = table[a >> 2];
    dst[k + 1] = table[((a & 3) << 4) | (b >> 4)];
    dst[k + 2] = table[((b & 0x0f) << 2) | (c >> 6)];
    dst[k + 3] = table[c & 0x3f];

    i += 3;
    k += 4;
  }

  if (n != slen) {
    switch (slen - n) {
    case 1:
      a = src[i + 0] & 0xff;
      dst[k + 0] = table[a >> 2];
      dst[k + 1] = table[(a & 3) << 4];
      dst[k + 2] = '=';
      dst[k + 3] = '=';
      break;

    case 2:
      a = src[i + 0] & 0xff;
      b = src[i + 1] & 0xff;
      dst[k + 0] = table[a >> 2];
      dst[k + 1] = table[((a & 3) << 4) | (b >> 4)];
      dst[k + 2] = table[(b & 0x0f) << 2];
      dst[k + 3] = '=';
      break;
    }
  }

  return dlen;
}

std::string GenerateUniqueID()
{
    static std::random_device rd;
    static std::mt19937 mte(rd());

    std::uniform_int_distribution<uint16_t> dist;

    std::array<uint16_t, 8> buffer;
    std::generate(buffer.begin(), buffer.end(), [&]() { return dist(mte); });

    char uuid[256];
    snprintf(uuid, sizeof(uuid), "%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
        buffer[0], buffer[1], buffer[2],
        (buffer[3] & 0x0fff) | 0x4000,
        (buffer[4] & 0x3fff) | 0x8000,
        buffer[5], buffer[6], buffer[7]);
    return uuid;
}

v8::Local<v8::String> OneByteString(v8::Isolate* isolate, const char* data, int length)
{
    return v8::String::NewFromOneByte(
        isolate,
        reinterpret_cast<const uint8_t*>(data),
        v8::NewStringType::kNormal,
        length).ToLocalChecked();
}

}
}