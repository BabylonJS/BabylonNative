/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/os.h>
#include <bx/semaphore.h>
#include <bx/timer.h>

TEST_CASE("getProcessMemoryUsed", "[os]")
{
	if (BX_ENABLED(BX_PLATFORM_EMSCRIPTEN) )
	{
		SKIP("Not supported by wasm.");
	}

	REQUIRE(0 != bx::getProcessMemoryUsed() );
}

#if BX_CONFIG_SUPPORTS_THREADING

TEST_CASE("semaphore_timeout", "[os]")
{
	bx::Semaphore sem;

	int64_t start     = bx::getHPCounter();
	bool    ok        = sem.wait(900);
	int64_t elapsed   = bx::getHPCounter() - start;
	int64_t frequency = bx::getHPFrequency();
	double  ms        = double(elapsed) / double(frequency) * 1000;
	bx::printf("%f\n", ms);
	REQUIRE(!ok);
}

#endif // BX_CONFIG_SUPPORTS_THREADING

TEST_CASE("memoryPageSize", "[os]")
{
	const size_t pageSize = bx::memoryPageSize();
	REQUIRE(0 != pageSize);
	// Page sizes are always powers of two on supported platforms.
	REQUIRE(0 == (pageSize & (pageSize - 1) ) );

	// Large/huge page queries must not return a value smaller than the base page size.
	const size_t large = bx::memoryPageSize(bx::Memory::PageLarge);
	const size_t huge  = bx::memoryPageSize(bx::Memory::PageHuge);
	REQUIRE( (0 == large || large >= pageSize) );
	REQUIRE( (0 == huge  || huge  >= pageSize) );
}

TEST_CASE("memoryMap-readWrite-roundtrip", "[os]")
{
	const size_t size = 4 * bx::memoryPageSize();

	bx::Error err;
	void* addr = bx::memoryMap(NULL, size, 0, bx::Memory::ReadWrite, &err);
	REQUIRE(err.isOk() );
	REQUIRE(nullptr != addr);

	uint8_t* bytes = (uint8_t*)addr;
	for (size_t ii = 0; ii < size; ++ii)
	{
		bytes[ii] = uint8_t(ii & 0xff);
	}
	for (size_t ii = 0; ii < size; ++ii)
	{
		REQUIRE(uint8_t(ii & 0xff) == bytes[ii]);
	}

	bx::memoryUnmap(addr, size, &err);
	REQUIRE(err.isOk() );
}

TEST_CASE("memoryMap-reserve-then-commit", "[os]")
{
	const size_t pageSize = bx::memoryPageSize();
	const size_t size     = 8 * pageSize;

	bx::Error err;
	void* addr = bx::memoryMap(NULL, size, 0, bx::Memory::Reserve | bx::Memory::ProtectNone, &err);
	REQUIRE(err.isOk() );
	REQUIRE(nullptr != addr);

	// Commit one page inside the reservation and write to it.
	uint8_t* bytes = (uint8_t*)addr;
	bx::memoryMap(bytes + pageSize, pageSize, 0, bx::Memory::Commit | bx::Memory::ProtectReadWrite, &err);
	REQUIRE(err.isOk() );

	for (size_t ii = 0; ii < pageSize; ++ii)
	{
		bytes[pageSize + ii] = uint8_t( (ii * 7) & 0xff);
	}
	for (size_t ii = 0; ii < pageSize; ++ii)
	{
		REQUIRE(uint8_t( (ii * 7) & 0xff) == bytes[pageSize + ii]);
	}

	// Decommit the page; reservation stays, but physical backing is dropped.
	bx::memoryMap(bytes + pageSize, pageSize, 0, bx::Memory::Decommit | bx::Memory::ProtectNone, &err);
	REQUIRE(err.isOk() );

	// Release the whole reservation.
	bx::memoryUnmap(addr, size, &err);
	REQUIRE(err.isOk() );
}

TEST_CASE("memoryMap-protection-change", "[os]")
{
	const size_t size = bx::memoryPageSize();

	bx::Error err;
	void* addr = bx::memoryMap(NULL, size, 0, bx::Memory::ReadWrite, &err);
	REQUIRE(err.isOk() );
	REQUIRE(nullptr != addr);

	uint8_t* bytes = (uint8_t*)addr;
	bytes[0] = 0xab;
	REQUIRE(0xab == bytes[0]);

	// Flip to read-only, then back to read/write. Neither call should fail.
	bx::memoryMap(addr, size, 0, bx::Memory::ProtectRead, &err);
	REQUIRE(err.isOk() );
	REQUIRE(0xab == bytes[0]);

	bx::memoryMap(addr, size, 0, bx::Memory::ProtectReadWrite, &err);
	REQUIRE(err.isOk() );
	bytes[0] = 0xcd;
	REQUIRE(0xcd == bytes[0]);

	bx::memoryUnmap(addr, size, &err);
	REQUIRE(err.isOk() );
}
