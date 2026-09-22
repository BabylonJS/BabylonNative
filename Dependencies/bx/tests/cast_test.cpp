/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/bx.h>

TEST_CASE("Bit cast", "[cast]")
{
	STATIC_REQUIRE(0x4172f58bc0000000ull == bx::bitCast<uint64_t>(19880124.0) );
	STATIC_REQUIRE(0x3fe9000000000000ull == bx::bitCast<uint64_t>(0.781250) );
	STATIC_REQUIRE(19880124.0            == bx::bitCast<double>(0x4172f58bc0000000ull) );
	STATIC_REQUIRE(0.781250              == bx::bitCast<double>(0x3fe9000000000000ull) );
}

TEST_CASE("Narrow cast", "[cast]")
{
	REQUIRE(127 == bx::narrowCast<int8_t>(int32_t(127) ) );
	REQUIRE_ASSERTS(bx::narrowCast<int8_t>(int32_t(128) ) );
	REQUIRE_ASSERTS(bx::narrowCast<int8_t>(uint32_t(128) ) );
	REQUIRE(128 == bx::narrowCast<uint8_t>(int32_t(128) ) );
}

template<typename Ty, typename FromT>
inline constexpr bool saturateCastTest(Ty _expectedMin, Ty _expectedMax)
{
	return true
		&& _expectedMin == bx::saturateCast<Ty>(static_cast<FromT>(_expectedMin) )
		&& _expectedMax == bx::saturateCast<Ty>(static_cast<FromT>(_expectedMax) )
		;
}

TEST_CASE("Saturate cast", "[cast]")
{
	STATIC_REQUIRE(-128 == bx::saturateCast<int8_t >( int32_t(       -255) ) );
	STATIC_REQUIRE( 127 == bx::saturateCast<int8_t >( int32_t(        255) ) );
	STATIC_REQUIRE( 127 == bx::saturateCast<int8_t >( int32_t(        127) ) );
	STATIC_REQUIRE( 127 == bx::saturateCast<int8_t >( int32_t(        128) ) );
	STATIC_REQUIRE( 127 == bx::saturateCast<int8_t >(uint32_t(        128) ) );
	STATIC_REQUIRE(-128 == bx::saturateCast<int8_t >(   float(-1389.9831f) ) );
	STATIC_REQUIRE( 127 == bx::saturateCast<int8_t >(   float( 1389.9831f) ) );
	STATIC_REQUIRE(   0 == bx::saturateCast<uint8_t>( int32_t(       -128) ) );
	STATIC_REQUIRE( 128 == bx::saturateCast<uint8_t>( int32_t(        128) ) );
	STATIC_REQUIRE( -13 == bx::saturateCast<int8_t >(   float(    -13.89f) ) );
	STATIC_REQUIRE(  13 == bx::saturateCast<int8_t >(   float(     13.89f) ) );
	STATIC_REQUIRE(   0 == bx::saturateCast<uint8_t>(   float(    -13.89f) ) );
	STATIC_REQUIRE(  13 == bx::saturateCast<uint8_t>(   float(     13.89f) ) );

	STATIC_REQUIRE(saturateCastTest<int8_t,   int8_t>(-128, 127) );
	STATIC_REQUIRE(saturateCastTest<int8_t,  uint8_t>(   0, 127) );
	STATIC_REQUIRE(saturateCastTest<int8_t,  int16_t>(-128, 127) );
	STATIC_REQUIRE(saturateCastTest<int8_t, uint16_t>(   0, 127) );
	STATIC_REQUIRE(saturateCastTest<int8_t,  int32_t>(-128, 127) );
	STATIC_REQUIRE(saturateCastTest<int8_t, uint32_t>(   0, 127) );
	STATIC_REQUIRE(saturateCastTest<int8_t,  int64_t>(-128, 127) );
	STATIC_REQUIRE(saturateCastTest<int8_t, uint64_t>(   0, 127) );

	STATIC_REQUIRE(saturateCastTest<uint8_t,   int8_t>(  0, 127) );
	STATIC_REQUIRE(saturateCastTest<uint8_t,  uint8_t>(  0, 255) );
	STATIC_REQUIRE(saturateCastTest<uint8_t,  int16_t>(  0, 255) );
	STATIC_REQUIRE(saturateCastTest<uint8_t, uint16_t>(  0, 255) );
	STATIC_REQUIRE(saturateCastTest<uint8_t,  int32_t>(  0, 255) );
	STATIC_REQUIRE(saturateCastTest<uint8_t, uint32_t>(  0, 255) );
	STATIC_REQUIRE(saturateCastTest<uint8_t,  int64_t>(  0, 255) );
	STATIC_REQUIRE(saturateCastTest<uint8_t, uint64_t>(  0, 255) );

	STATIC_REQUIRE(saturateCastTest<int16_t,   int8_t>(   -128,   127) );
	STATIC_REQUIRE(saturateCastTest<int16_t,  uint8_t>(      0,   255) );
	STATIC_REQUIRE(saturateCastTest<int16_t,  int16_t>( -32768, 32767) );
	STATIC_REQUIRE(saturateCastTest<int16_t, uint16_t>(      0, 32767) );
	STATIC_REQUIRE(saturateCastTest<int16_t,  int32_t>( -32768, 32767) );
	STATIC_REQUIRE(saturateCastTest<int16_t, uint32_t>(      0, 32767) );
	STATIC_REQUIRE(saturateCastTest<int16_t,  int64_t>( -32768, 32767) );
	STATIC_REQUIRE(saturateCastTest<int16_t, uint64_t>(      0, 32767) );

	STATIC_REQUIRE(saturateCastTest<uint16_t,   int8_t>(     0,   127) );
	STATIC_REQUIRE(saturateCastTest<uint16_t,  uint8_t>(     0,   255) );
	STATIC_REQUIRE(saturateCastTest<uint16_t,  int16_t>(     0, 32767) );
	STATIC_REQUIRE(saturateCastTest<uint16_t, uint16_t>(     0, 65535) );
	STATIC_REQUIRE(saturateCastTest<uint16_t,  int32_t>(     0, 65535) );
	STATIC_REQUIRE(saturateCastTest<uint16_t, uint32_t>(     0, 65535) );
	STATIC_REQUIRE(saturateCastTest<uint16_t,  int64_t>(     0, 65535) );
	STATIC_REQUIRE(saturateCastTest<uint16_t, uint64_t>(     0, 65535) );

	STATIC_REQUIRE(saturateCastTest<int32_t,   int8_t>(        -128,        127) );
	STATIC_REQUIRE(saturateCastTest<int32_t,  uint8_t>(           0,        255) );
	STATIC_REQUIRE(saturateCastTest<int32_t,  int16_t>(      -32768,      32767) );
	STATIC_REQUIRE(saturateCastTest<int32_t, uint16_t>(           0,      65535) );
	STATIC_REQUIRE(saturateCastTest<int32_t,  int32_t>( -2147483648, 2147483647) );
	STATIC_REQUIRE(saturateCastTest<int32_t, uint32_t>(           0, 2147483647) );
	STATIC_REQUIRE(saturateCastTest<int32_t,  int64_t>( -2147483648, 2147483647) );
	STATIC_REQUIRE(saturateCastTest<int32_t, uint64_t>(           0, 2147483647) );

	STATIC_REQUIRE(saturateCastTest<uint32_t,   int8_t>(          0,        127) );
	STATIC_REQUIRE(saturateCastTest<uint32_t,  uint8_t>(          0,        255) );
	STATIC_REQUIRE(saturateCastTest<uint32_t,  int16_t>(          0,      32767) );
	STATIC_REQUIRE(saturateCastTest<uint32_t, uint16_t>(          0,      65535) );
	STATIC_REQUIRE(saturateCastTest<uint32_t,  int32_t>(          0, 2147483647) );
	STATIC_REQUIRE(saturateCastTest<uint32_t, uint32_t>(          0, 4294967295) );
	STATIC_REQUIRE(saturateCastTest<uint32_t,  int64_t>(          0, 4294967295) );
	STATIC_REQUIRE(saturateCastTest<uint32_t, uint64_t>(          0, 4294967295) );

	STATIC_REQUIRE(saturateCastTest<int64_t,   int8_t>(                   -128,                   127) );
	STATIC_REQUIRE(saturateCastTest<int64_t,  uint8_t>(                      0,                   255) );
	STATIC_REQUIRE(saturateCastTest<int64_t,  int16_t>(                 -32768,                 32767) );
	STATIC_REQUIRE(saturateCastTest<int64_t, uint16_t>(                      0,                 65535) );
	STATIC_REQUIRE(saturateCastTest<int64_t,  int32_t>(            -2147483648,            2147483647) );
	STATIC_REQUIRE(saturateCastTest<int64_t, uint32_t>(                      0,            4294967295) );
	STATIC_REQUIRE(saturateCastTest<int64_t,  int64_t>( -9223372036854775808ll, 9223372036854775807ll) );
	STATIC_REQUIRE(saturateCastTest<int64_t, uint64_t>(                      0, 9223372036854775807ll) );

	STATIC_REQUIRE(saturateCastTest<uint64_t,   int8_t>(                     0,              127) );
	STATIC_REQUIRE(saturateCastTest<uint64_t,  uint8_t>(                     0,                     255) );
	STATIC_REQUIRE(saturateCastTest<uint64_t,  int16_t>(                     0,                   32767) );
	STATIC_REQUIRE(saturateCastTest<uint64_t, uint16_t>(                     0,                   65535) );
	STATIC_REQUIRE(saturateCastTest<uint64_t,  int32_t>(                     0,              2147483647) );
	STATIC_REQUIRE(saturateCastTest<uint64_t, uint32_t>(                     0,              4294967295) );
	STATIC_REQUIRE(saturateCastTest<uint64_t,  int64_t>(                     0,  9223372036854775807ull) );
	STATIC_REQUIRE(saturateCastTest<uint64_t, uint64_t>(                     0, 18446744073709551615ull) );
}
