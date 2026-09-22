/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/ringbuffer.h>

TEST_CASE("RingBufferControl", "")
{
	constexpr uint32_t kMax = 16;

	bx::RingBufferControl control(kMax);

	REQUIRE(kMax   == control.getSize() );
	REQUIRE(0      == control.getNumUsed() );
	REQUIRE(0      == control.getNumReserved() );
	REQUIRE(kMax-1 == control.getNumEmpty() );
	REQUIRE(control.isEmpty() );

	REQUIRE(1 == control.reserve(1)  );

	REQUIRE(kMax   == control.getSize() );
	REQUIRE(0      == control.getNumUsed() );
	REQUIRE(1      == control.getNumReserved() );
	REQUIRE(kMax-2 == control.getNumEmpty() );
	REQUIRE(!control.isEmpty() );

	REQUIRE(0      == control.reserve(16, true) );
	REQUIRE(kMax-2 == control.reserve(16) );

	REQUIRE(kMax   == control.getSize() );
	REQUIRE(0      == control.getNumUsed() );
	REQUIRE(kMax-1 == control.getNumReserved() );
	REQUIRE(0      == control.getNumEmpty() );
	REQUIRE(!control.isEmpty() );

	REQUIRE(15 == control.commit(15)  );

	REQUIRE(kMax   == control.getSize() );
	REQUIRE(kMax-1 == control.getNumUsed() );
	REQUIRE(0      == control.getNumReserved() );
	REQUIRE(0      == control.getNumEmpty() );
	REQUIRE(!control.isEmpty() );

	REQUIRE(15 == control.consume(15) );

	REQUIRE(kMax   == control.getSize() );
	REQUIRE(0      == control.getNumUsed() );
	REQUIRE(0      == control.getNumReserved() );
	REQUIRE(kMax-1 == control.getNumEmpty() );
	REQUIRE(control.isEmpty() );
}

TEST_CASE("RingBufferControl resize", "")
{
	bx::RingBufferControl control(10);

	uint32_t reserved;
	uint32_t commited;
	uint32_t consumed;

	reserved = control.reserve(8);
	REQUIRE(reserved == 8);
	REQUIRE(control.m_current == 0);
	REQUIRE(control.m_write   == 8);
	REQUIRE(control.m_read    == 0);

	commited = control.commit(4);
	REQUIRE(commited == 4);
	REQUIRE(control.m_current == 4);
	REQUIRE(control.m_write   == 8);
	REQUIRE(control.m_read    == 0);

	consumed = control.consume(2);
	REQUIRE(consumed == 2);
	REQUIRE(control.m_current == 4);
	REQUIRE(control.m_write   == 8);
	REQUIRE(control.m_read    == 2);

	REQUIRE(10 == control.getSize() );

	control.resize(10);
	REQUIRE(20 == control.getSize() );

	control.reserve(8);
	REQUIRE(control.m_current == 4);
	REQUIRE(control.m_write   == 16);
	REQUIRE(control.m_read    == 2);

	control.commit(4);
	REQUIRE(control.m_current == 8);
	REQUIRE(control.m_write   == 16);
	REQUIRE(control.m_read    == 2);

	control.consume(2);
	REQUIRE(control.m_current == 8);
	REQUIRE(control.m_write   == 16);
	REQUIRE(control.m_read    == 4);

	reserved = control.reserve(4);
	REQUIRE(reserved == 4);
	commited = control.commit(4);
	REQUIRE(commited == 4);
	consumed = control.consume(6);
	REQUIRE(consumed == 6);

	REQUIRE(control.m_current == 12);
	REQUIRE(control.m_write   == 0);
	REQUIRE(control.m_read    == 10);

	REQUIRE(2  == control.getNumUsed() );
	REQUIRE(8  == control.getNumReserved() );
	REQUIRE(9  == control.getNumEmpty() );

	control.resize(-10);
	REQUIRE(11 == control.getSize() );
	REQUIRE(2  == control.getNumUsed() );
	REQUIRE(8  == control.getNumReserved() );
	REQUIRE(0  == control.getNumEmpty() );

	REQUIRE(control.m_current == 3);
	REQUIRE(control.m_write   == 0);
	REQUIRE(control.m_read    == 1);
}
