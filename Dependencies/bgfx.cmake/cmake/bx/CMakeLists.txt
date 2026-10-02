# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include(bx.cmake)

if(BGFX_BUILD_TOOLS_BIN2C)
	include(bin2c.cmake)
endif()

if(BGFX_BUILD_TESTS)
	file(
		GLOB
		BX_TEST_SOURCES #
		${BX_DIR}/3rdparty/catch/catch_amalgamated.cpp
		${BX_DIR}/tests/*_test.cpp #
		${BX_DIR}/tests/*.h #
		${BX_DIR}/tests/dbg.* #
	)
	add_executable(bx_test ${BX_TEST_SOURCES})
	target_compile_definitions(bx_test PRIVATE CATCH_AMALGAMATED_CUSTOM_MAIN)
	target_link_libraries(bx_test PRIVATE bx)
	add_test(NAME bx.test COMMAND bx_test)

	file(
		GLOB
		BX_BENCH_SOURCES #
		${BX_DIR}/tests/*_bench.cpp #
		${BX_DIR}/tests/*_bench.h #
		${BX_DIR}/tests/dbg.* #
	)
	add_executable(bx_bench ${BX_BENCH_SOURCES})
	target_link_libraries(bx_bench PRIVATE bx)
	add_test(NAME bx.bench COMMAND bx_bench)
endif()
