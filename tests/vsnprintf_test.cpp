/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/string.h>
#include <bx/readerwriter.h>

#include <limits>
#include <inttypes.h>

TEST_CASE("No output buffer provided.", "[string][printf]")
{
	REQUIRE(4 == bx::snprintf(NULL,  0, "test") );
	REQUIRE(4 == bx::snprintf(NULL, 10, "test") );
	REQUIRE(1 == bx::snprintf(NULL,  0, "%d", 1) );
	REQUIRE(1 == bx::snprintf(NULL, 10, "%d", 1) );
}

TEST_CASE("Truncated output buffer.", "[string][printf]")
{
	REQUIRE(4 == bx::snprintf(NULL, 0, "abvg") );

	char buffer15[15]; // fit
	REQUIRE(4    == bx::snprintf(buffer15, BX_COUNTOF(buffer15), "abvg") );
	REQUIRE('\0' == buffer15[4]);
	REQUIRE(0    == bx::strCmp(buffer15, "abvg") );

	char buffer1[1]; // truncate
	REQUIRE(4    == bx::snprintf(buffer1, BX_COUNTOF(buffer1), "abvg") );
	REQUIRE('\0' == buffer1[BX_COUNTOF(buffer1)-1]);

	buffer1[0] = '\xfb'; // null destination
	REQUIRE(4      == bx::snprintf(NULL, BX_COUNTOF(buffer1), "abvg") );
	REQUIRE('\xfb' == buffer1[0]);

	buffer1[0] = '\xbf'; // one byte destination
	REQUIRE(4    == bx::snprintf(buffer1, 1, "abvg") );
	REQUIRE('\0' == buffer1[0]);

	char buffer7[7]; // truncate
	REQUIRE(10   == bx::snprintf(NULL, 0, "Ten chars!") );
	REQUIRE(10   == bx::snprintf(buffer7, BX_COUNTOF(buffer7), "Ten chars!") );
	REQUIRE('\0' == buffer7[BX_COUNTOF(buffer7)-1]);
	REQUIRE(0    == bx::strCmp(buffer7, "Ten ch") );

	REQUIRE(7    == bx::snprintf(NULL, 0, "Seven67") );
	REQUIRE(7    == bx::snprintf(buffer7, BX_COUNTOF(buffer7), "Seven67") );
	REQUIRE('\0' == buffer7[BX_COUNTOF(buffer7)-1]);
	REQUIRE(0    == bx::strCmp(buffer7, "Seven6") );

	REQUIRE(11   == bx::snprintf(NULL, 0, "SevenEleven") );
	REQUIRE(11   == bx::snprintf(buffer7, BX_COUNTOF(buffer7), "SevenEleven") );
	REQUIRE('\0' == buffer7[BX_COUNTOF(buffer7)-1]);
	REQUIRE(0    == bx::strCmp(buffer7, "SevenE") );
}

template<bool StdCompliantT>
static bool test(const char* _expected, const char* _format, va_list _argList)
{
	const int32_t expectedLen = bx::strLen(_expected);
	int32_t max = expectedLen + 1024;
	char* bxTemp = (char*)BX_STACK_ALLOC(max);

	va_list argList;
	va_copy(argList, _argList);
	const int32_t bxLen = bx::vsnprintf(bxTemp, max, _format, argList);

	bool result = true
		&& bxLen == expectedLen
		&&     0 == bx::strCmp(_expected, bxTemp)
		;

	char*  crtTemp = NULL;
	int32_t crtLen = 0;

	if (!result
	||  StdCompliantT)
	{
		BX_ASSERT(bx::strFind(_format, "%S").isEmpty()
			, "String format test is using '%%S' bx::StringView specific format specifier which is not standard compliant. "
			  "Use `testNotStdCompliant` string testing method."
			);

		crtTemp = (char*)BX_STACK_ALLOC(max);

		va_copy(argList, _argList);
		crtLen = ::vsnprintf(crtTemp, max, _format, argList);

		result &= true
			&& crtLen == bxLen
			&&      0 == bx::strCmp(bx::StringView(bxTemp, bxLen), bx::StringView(crtTemp, crtLen) )
			;
	}

	if (!result)
	{
		printf("---\n");
		printf("printf format '%s'\n", _format);
		printf("    bx result (%4d) '%s'\n", bxLen, bxTemp);
		printf("     expected (%4d) '%s'\n", expectedLen, _expected);
		printf("CRT vsnprintf (%4d) '%s'\n", crtLen, crtTemp);
	}

	return result;
}

// Test against CRT's vsnprintf implementation.
static bool test(const char* _expected, const char* _format, ...)
{
	va_list argList;
	va_start(argList, _format);
	const bool result = test<false>(_expected, _format, argList);
	va_end(argList);

	return result;
}

// Skip test against CRT's vsnprintf implementation.
static bool testNotStdCompliant(const char* _expected, const char* _format, ...)
{
	va_list argList;
	va_start(argList, _format);
	const bool result = test<false>(_expected, _format, argList);
	va_end(argList);

	return result;
}

TEST_CASE("Format %f", "[string][printf]")
{
	constexpr double kDoubleNan = bx::bitsToDouble(bx::kDoubleExponentMask | bx::kDoubleMantissaMask);

	REQUIRE(test("1.337",    "%0.3f", 1.337) );
	REQUIRE(test("  13.370", "%8.3f", 13.37) );
	REQUIRE(test("  13.370", "%*.*f", 8, 3, 13.37) );
	REQUIRE(test("13.370  ", "%-8.3f", 13.37) );
	REQUIRE(test("13.370  ", "%*.*f", -8, 3, 13.37) );
	REQUIRE(test(" -13.370", "% 8.3f", -13.37) );
	REQUIRE(test("          13.370", "% 16.3f",  13.37) );
	REQUIRE(test("         -13.370", "% 16.3f", -13.37) );

	REQUIRE(test("nan     ", "%-8f",  kDoubleNan) );
	REQUIRE(test("     nan", "%8f",   kDoubleNan) );
	REQUIRE(test("-NAN    ", "%-8F", -kDoubleNan) );

#if !defined(__FAST_MATH__) || !__FAST_MATH__
	REQUIRE(test("     inf", "%8f",   bx::kDoubleInfinity) );
	REQUIRE(test("inf     ", "%-8f",  bx::kDoubleInfinity) );
	REQUIRE(test("    -INF", "%8F",  -bx::kDoubleInfinity) );
#endif // !defined(__FAST_MATH__) || !__FAST_MATH__

	REQUIRE(test(" 1.0",     "%4.1f",    1.0) );
	REQUIRE(test(" 1.500",   "%6.3f",    1.5) );
	REQUIRE(test("0001.500", "%08.3f",   1.5) );
	REQUIRE(test("+001.500", "%+08.3f",  1.5) );
	REQUIRE(test("-001.500", "%+08.3f", -1.5) );
	REQUIRE(test("0.0039",   "%.4f",     0.00390625) );

	REQUIRE(test("0.003906",     "%f",   0.00390625) );
	REQUIRE(test("-1.234567e-9", "%f",  -1.234567e-9) );

	REQUIRE(test("-1e-9",            "%.0f",  -1.234567e-9) );
	REQUIRE(test("-1.2e-9",          "%.1f",  -1.234567e-9) );
	REQUIRE(test("-1.23e-9",         "%.2f",  -1.234567e-9) );
	REQUIRE(test("-1.234e-9",        "%.3f",  -1.234567e-9) );
	REQUIRE(test("-1.2345e-9",       "%.4f",  -1.234567e-9) );
	REQUIRE(test("-1.23456e-9",      "%.5f",  -1.234567e-9) );
	REQUIRE(test("-1.234567e-9",     "%.6f",  -1.234567e-9) );
	REQUIRE(test("-1.2345670e-9",    "%.7f",  -1.234567e-9) );
	REQUIRE(test("-1.23456700e-9",   "%.8f",  -1.234567e-9) );
	REQUIRE(test("-1.234567000e-9",  "%.9f",  -1.234567e-9) );
	REQUIRE(test("-1.2345670000e-9", "%.10f", -1.234567e-9) );

	REQUIRE(test("3.141592",           "%f",    3.1415926535897932) );
	REQUIRE(test("3.141592",           "%F",    3.1415926535897932) );
	REQUIRE(test("3",                  "%.0f",  3.1415926535897932) );
	REQUIRE(test("3.1",                "%.1f",  3.1415926535897932) );
	REQUIRE(test("3.14",               "%.2f",  3.1415926535897932) );
	REQUIRE(test("3.141",              "%.3f",  3.1415926535897932) );
	REQUIRE(test("3.1415",             "%.4f",  3.1415926535897932) );
	REQUIRE(test("3.14159",            "%.5f",  3.1415926535897932) );
	REQUIRE(test("3.141592",           "%.6f",  3.1415926535897932) );
	REQUIRE(test("3.1415926",          "%.7f",  3.1415926535897932) );
	REQUIRE(test("3.14159265",         "%.8f",  3.1415926535897932) );
	REQUIRE(test("3.141592653",        "%.9f",  3.1415926535897932) );
	REQUIRE(test("3.1415926535",       "%.10f", 3.1415926535897932) );
	REQUIRE(test("3.14159265358",      "%.11f", 3.1415926535897932) );
	REQUIRE(test("3.141592653589",     "%.12f", 3.1415926535897932) );
	REQUIRE(test("3.1415926535897",    "%.13f", 3.1415926535897932) );
	REQUIRE(test("3.14159265358979",   "%.14f", 3.1415926535897932) );
	REQUIRE(test("3.141592653589793",  "%.15f", 3.1415926535897932) );
	REQUIRE(test("3.1415926535897930", "%.16f", 3.1415926535897932) );
	REQUIRE(test("3.1415926535897930", "%.16F", 3.1415926535897932) );

	REQUIRE(test("-3.141592e-9",           "%f",    -3.1415926535897932e-9) );
	REQUIRE(test("-3.141592E-9",           "%F",    -3.1415926535897932e-9) );
	REQUIRE(test("-3e-9",                  "%.0f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.1e-9",                "%.1f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.14e-9",               "%.2f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.141e-9",              "%.3f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.1415e-9",             "%.4f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.14159e-9",            "%.5f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.141592e-9",           "%.6f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.1415926e-9",          "%.7f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.14159265e-9",         "%.8f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.141592653e-9",        "%.9f",  -3.1415926535897932e-9) );
	REQUIRE(test("-3.1415926535e-9",       "%.10f", -3.1415926535897932e-9) );
	REQUIRE(test("-3.14159265358e-9",      "%.11f", -3.1415926535897932e-9) );
	REQUIRE(test("-3.141592653589e-9",     "%.12f", -3.1415926535897932e-9) );
	REQUIRE(test("-3.1415926535897e-9",    "%.13f", -3.1415926535897932e-9) );
	REQUIRE(test("-3.14159265358979e-9",   "%.14f", -3.1415926535897932e-9) );
	REQUIRE(test("-3.141592653589793e-9",  "%.15f", -3.1415926535897932e-9) );
	REQUIRE(test("-3.1415926535897930e-9", "%.16f", -3.1415926535897932e-9) );
	REQUIRE(test("-3.1415926535897930E-9", "%.16F", -3.1415926535897932e-9) );

	REQUIRE(test("1e-12", "%f", 1e-12));

	REQUIRE(test("0.00390625",          "%.8f",  0.00390625) );
	REQUIRE(test("-0.00390625",         "%.8f", -0.00390625) );
	REQUIRE(test("1.50000000000000000", "%.17f", 1.5) );

	REQUIRE(test("-3.000000",      "%f",      -3.0) );
	REQUIRE(test("-0.000000",      "%f",      -0.) );
	REQUIRE(test("4.1",            "%.1f",    4.1) );
	REQUIRE(test("0.00",           "%.2f",    1e-4) );
	REQUIRE(test("-5.20",          "%+4.2f", -5.2) );
	REQUIRE(test("0.0       ",     "%-10.1f", 0.) );
	REQUIRE(test("-8.8888888800",  "%.10f",  -8.88888888) );
	REQUIRE(test("880.0888888800", "%.10f",   880.08888888) );
	REQUIRE(test("100056789.0",    "%.1f",    100056789.0) );
	REQUIRE(test(" 0",             "% .0f",   0.1) );
}

TEST_CASE("Format %d, %i, %o, %u, %x", "[string][printf]")
{
	REQUIRE(test("1337", "%d", 1337) );
	REQUIRE(test("-1337", "% d", -1337) );
	REQUIRE(test("1337                ", "%-20d",  1337) );
	REQUIRE(test("-1337               ", "%-20d", -1337) );
	REQUIRE(test("               -1337", "% 20d", -1337) );

	REQUIRE(test("1337", "%i", 1337) );
	REQUIRE(test("1337                ", "%-20i",  1337) );
	REQUIRE(test("-1337               ", "%-20i", -1337) );

	REQUIRE(test("1337", "%o", 01337) );
	REQUIRE(test("2471", "%o", 1337) );
	REQUIRE(test("1337                ", "%-20o",  01337) );
	REQUIRE(test("37777776441         ", "%-20o", -01337) );
	REQUIRE(test("                2471", "%20o",    1337) );
	REQUIRE(test("00000000000000002471", "%020o",   1337) );

	REQUIRE(test("1337", "%u", 1337) );
	REQUIRE(test("1337                ", "%-20u",  1337) );
	REQUIRE(test("4294965959          ", "%-20u", -1337) );

	REQUIRE(test("1337", "%x", 0x1337) );
	REQUIRE(test("1234abcd            ", "%-20x",  0x1234abcd) );
	REQUIRE(test("1234ABCD            ", "%-20X",  0x1234abcd) );
	REQUIRE(test("edcb5433            ", "%-20x", -0x1234abcd) );
	REQUIRE(test("EDCB5433            ", "%-20X", -0x1234abcd) );
	REQUIRE(test("            1234abcd", "% 20x",  0x1234abcd) );
	REQUIRE(test("            1234ABCD", "% 20X",  0x1234abcd) );
	REQUIRE(test("            edcb5433", "% 20x", -0x1234abcd) );
	REQUIRE(test("            EDCB5433", "% 20X", -0x1234abcd) );
	REQUIRE(test("0000000000001234abcd", "%020x",  0x1234abcd) );
	REQUIRE(test("0000000000001234ABCD", "%020X",  0x1234abcd) );
	REQUIRE(test("000000000000edcb5433", "%020x", -0x1234abcd) );
	REQUIRE(test("000000000000EDCB5433", "%020X", -0x1234abcd) );

	REQUIRE(test("0xffffffff", "0x%01x", -1) );
	REQUIRE(test("0xffffffff", "0x%08x", -1) );

	REQUIRE(test("  -1", "% 4i", -1) );
	REQUIRE(test("   0", "% 4i",  0) );
	REQUIRE(test("   1", "% 4i",  1) );
	REQUIRE(test("   1", "% 4o",  1) );
	REQUIRE(test("  +1", "%+4i",  1) );
	REQUIRE(test("  +1", "%+4o",  1) );
	REQUIRE(test("  +0", "%+4i",  0) );
	REQUIRE(test("  -1", "%+4i", -1) );
	REQUIRE(test("0001", "%04i",  1) );
	REQUIRE(test("0001", "%04o",  1) );
	REQUIRE(test("0000", "%04i",  0) );
	REQUIRE(test("0000", "%04o",  0) );
	REQUIRE(test("-001", "%04i", -1) );
	REQUIRE(test("+001", "%+04i", 1) );

	if constexpr (sizeof(intmax_t) == 4)
	{
		REQUIRE(test("2147483647", "%jd", INTMAX_MAX) );
	}
	else
	{
		REQUIRE(test("9223372036854775807", "%jd", INTMAX_MAX) );
	}

	REQUIRE(test("18446744073709551615", "%" PRIu64, UINT64_MAX) );
	REQUIRE(test("ffffffffffffffff", "%016" PRIx64, UINT64_MAX) );

	REQUIRE(test("33 555",  "%hi %ld", (short)33, 555l) );
	REQUIRE(test("20 0020", "%u %04u", 20u, 20u) );

	REQUIRE(test("012 0x1e 0X3C", "%#o %#x %#X", 10u, 30u, 60u) );
	REQUIRE(test(" 12 1e 3C ",    "%3o %2x %-3X", 10u, 30u, 60u) );
	REQUIRE(test("12 1e 3C",      "%o %x %X", 10u, 30u, 60u) );
}

TEST_CASE("Format modifiers", "[string][printf]")
{
	REQUIRE(test("|  1.000000|", "|%10f|",      1.0f) );
	REQUIRE(test("|1.000000  |", "|%-10f|",     1.0f) );
	REQUIRE(test("|001.000000|", "|%010f|",     1.0f) );
	REQUIRE(test("|0000000001|", "|%010.0f|",   1.0f) );
	REQUIRE(test("|000000001.|", "|%#010.0f|",  1.0f) );
	REQUIRE(test("|         1|", "|%10.0f|",    1.0f) );
	REQUIRE(test("|        1.|", "|%#10.0f|",   1.0f) );
	REQUIRE(test("|       +1.|", "|%#+10.0f|",  1.0f) );
	REQUIRE(test("|1         |", "|%-10.0f|",   1.0f) );
	REQUIRE(test("|1.        |", "|%#-10.0f|",  1.0f) );
	REQUIRE(test("|+1.       |", "|%+#-10.0f|", 1.0f) );

	REQUIRE(test("|     00013:    -00089|", "|%10.5d:%10.5d|",   13, -89) );
	REQUIRE(test("|    -00013:    +00089|", "|%10.5d:%+10.5d|", -13,  89) );
	REQUIRE(test("|    -00013:    -00089|", "|%10.5d:%10.5d|",  -13, -89) );
}

TEST_CASE("Format %p", "[string][printf]")
{
	REQUIRE(test("0xbadc0de", "%p", (void*)0xbadc0de) );
	REQUIRE(test("0xbadc0de           ", "%-20p", (void*)0xbadc0de) );
}

TEST_CASE("Format %s", "[string][printf]")
{
	REQUIRE(test("(null)", "%s", NULL) );
}

TEST_CASE("Format %td", "[string][printf]")
{
	ptrdiff_t size = ptrdiff_t(-1);

	REQUIRE(test("-1", "%td", size) );

	if constexpr (4 == sizeof(ptrdiff_t) )
	{
		REQUIRE(test("-1073741824", "%td", ptrdiff_t(3221225472) ) );
	}
	else
	{
		REQUIRE(test("3221225472", "%td", ptrdiff_t(3221225472) ) );
	}
}

TEST_CASE("Format %n", "[string][printf]")
{
	char temp[64];

	int32_t p0, p1, p2;
	bx::snprintf(temp, sizeof(temp), "%n", &p0);
	REQUIRE(0 == p0);

	bx::snprintf(temp, sizeof(temp), "01%n23%n45%n", &p0, &p1, &p2);
	REQUIRE(2 == p0);
	REQUIRE(4 == p1);
	REQUIRE(6 == p2);
}

TEST_CASE("Format %g", "[string][printf]")
{
	REQUIRE(test("   0.01",  "%7.2g", .01) );
	REQUIRE(test(" 0.0123",  "%7.4G", .0123) );
//	REQUIRE(test("1.23e+05", "%.3g",  123000.25) );
//	REQUIRE(test("1e+05",    "%.0g",  123000.25) );
	REQUIRE(test("1",       "%.0g", 1.2) );
}

TEST_CASE("Format %c, %s, %S", "[string][printf]")
{
	REQUIRE(test("hello               ", "%-20s", "hello") );
	REQUIRE(test("     hello", "%10s", "hello") );
	REQUIRE(test("hello, world!", "%s, %s!", "hello", "world") );

	bx::StringView str("0hello1world2");
	bx::StringView hello(str, 1, 5);
	bx::StringView world(str, 7, 5);
	REQUIRE(test("hello, world!", "%.*s, %.*s!"
		, hello.getLength(), hello.getPtr()
		, world.getLength(), world.getPtr()
		) );

	REQUIRE(testNotStdCompliant("hello, world!", "%S, %S!"
		, &hello
		, &world
		) );
}

TEST_CASE("WriterI", "[string][printf]")
{
	char tmp[64];
	bx::StaticMemoryBlock mb(tmp, sizeof(tmp));
	bx::MemoryWriter writer(&mb);

	bx::Error err;
	int32_t len = bx::write(&writer, &err, "%d", 1389);
	REQUIRE(err.isOk());
	REQUIRE(len == 4);

	bx::StringView str(tmp, len);
	REQUIRE(0 == bx::strCmp(str, "1389") );
}

TEST_CASE("Invalid", "[string][printf]")
{
	char temp[64];
	REQUIRE(0 == bx::snprintf(temp, sizeof(temp), "%", 1) );
	REQUIRE(0 == bx::snprintf(temp, sizeof(temp), "%-", 1) );
	REQUIRE(0 == bx::snprintf(temp, sizeof(temp), "%-0", 1) );
	REQUIRE(0 == bx::snprintf(temp, sizeof(temp), "%-03", 1) );
	REQUIRE(0 == bx::snprintf(temp, sizeof(temp), "%-03.", 1) );
	REQUIRE(0 == bx::snprintf(temp, sizeof(temp), "%-03.0", 1) );
	REQUIRE(0 == bx::snprintf(temp, sizeof(temp), "%-03.0t", 1) );
}

TEST_CASE("Format %s width does not truncate", "[string][printf]")
{
	SECTION("Width smaller than string length must NOT truncate")
	{
		REQUIRE(test("hello",      "%1s",  "hello") );
		REQUIRE(test("hello",      "%5s",  "hello") );
		REQUIRE(test(" hello",     "%6s",  "hello") );
		REQUIRE(test("  hello",    "%7s",  "hello") );
	}

	SECTION("Left-aligned, width smaller than string length")
	{
		REQUIRE(test("hello",      "%-1s", "hello") );
		REQUIRE(test("hello ",     "%-6s", "hello") );
		REQUIRE(test("hello  ",    "%-7s", "hello") );
	}
}

TEST_CASE("Format %s precision truncates", "[string][printf]")
{
	SECTION("Precision DOES truncate strings")
	{
		REQUIRE(test("",       "%.0s", "hello") );
		REQUIRE(test("h",      "%.1s", "hello") );
		REQUIRE(test("he",     "%.2s", "hello") );
		REQUIRE(test("hel",    "%.3s", "hello") );
		REQUIRE(test("hell",   "%.4s", "hello") );
		REQUIRE(test("hello",  "%.5s", "hello") );
		REQUIRE(test("hello",  "%.10s", "hello") );
	}
}

TEST_CASE("Format %s width and precision combined", "[string][printf]")
{
	SECTION("Width pads, precision truncates")
	{
		REQUIRE(test("         h", "%10.1s", "hello") );
		REQUIRE(test("        he", "%10.2s", "hello") );
		REQUIRE(test("       hel", "%10.3s", "hello") );
		REQUIRE(test("     hello", "%10.5s", "hello") );
	}

	SECTION("Left-aligned with width and precision")
	{
		REQUIRE(test("h         ", "%-10.1s", "hello") );
		REQUIRE(test("he        ", "%-10.2s", "hello") );
		REQUIRE(test("hel       ", "%-10.3s", "hello") );
		REQUIRE(test("hello     ", "%-10.5s", "hello") );
	}

	SECTION("Width smaller than precision and string")
	{
		REQUIRE(test("hel",   "%1.3s",  "hello") );
		REQUIRE(test("hel",   "%-1.3s", "hello") );
	}

	SECTION("Precision with * wildcard")
	{
		REQUIRE(test("hel",   "%.*s", 3, "hello") );
		REQUIRE(test("hello", "%.*s", 10, "hello") );
	}
}

TEST_CASE("Format %s empty string", "[string][printf]")
{
	REQUIRE(test("",           "%s",    "") );
	REQUIRE(test("          ", "%10s",  "") );
	REQUIRE(test("          ", "%-10s", "") );
	REQUIRE(test("",           "%.0s",  "hello") );
	REQUIRE(test("          ", "%10.0s", "hello") );
}

TEST_CASE("Format %d width does not truncate", "[string][printf]")
{
	SECTION("Width smaller than number of digits must NOT truncate")
	{
		REQUIRE(test("1337",   "%1d",  1337) );
		REQUIRE(test("1337",   "%4d",  1337) );
		REQUIRE(test(" 1337",  "%5d",  1337) );
		REQUIRE(test("  1337", "%6d",  1337) );

		REQUIRE(test("-1337",  "%1d",  -1337) );
		REQUIRE(test("-1337",  "%5d",  -1337) );
		REQUIRE(test(" -1337", "%6d",  -1337) );
	}
}

TEST_CASE("Format %x width does not truncate", "[string][printf]")
{
	REQUIRE(test("abcd",   "%1x",  0xabcd) );
	REQUIRE(test("abcd",   "%4x",  0xabcd) );
	REQUIRE(test(" abcd",  "%5x",  0xabcd) );
	REQUIRE(test("ABCD",   "%1X",  0xabcd) );
}

TEST_CASE("Format %% percent literal", "[string][printf]")
{
	REQUIRE(test("%",     "%%") );
	REQUIRE(test("100%",  "100%%") );
	REQUIRE(test("%%",    "%%%%") );
	REQUIRE(test("a%b",   "a%%b") );
	REQUIRE(test("%42%",  "%%%d%%", 42) );
}

TEST_CASE("Format %d edge values", "[string][printf]")
{
	REQUIRE(test("0",           "%d",  0) );
	REQUIRE(test("2147483647",  "%d",  INT32_MAX) );
	REQUIRE(test("-2147483648", "%d",  INT32_MIN) );
	REQUIRE(test("+2147483647", "%+d", INT32_MAX) );

	REQUIRE(test("0",          "%u",  0u) );
	REQUIRE(test("4294967295", "%u",  UINT32_MAX) );

	REQUIRE(test("0", "%x", 0) );
	REQUIRE(test("ffffffff", "%x", UINT32_MAX) );
	REQUIRE(test("FFFFFFFF", "%X", UINT32_MAX) );

	REQUIRE(test("-100006789", "%d", -100006789) );
}

TEST_CASE("Format mixed specifiers", "[string][printf]")
{
	REQUIRE(test("hello 42 3.14 world", "%s %d %.2f %s", "hello", 42, 3.14, "world") );
	REQUIRE(test("a1b2c3", "a%db%dc%d", 1, 2, 3) );
	REQUIRE(test("0xff = 255", "0x%x = %u", 255, 255u) );
}

TEST_CASE("Format %c edge cases", "[string][printf]")
{
	REQUIRE(test("A",                    "%c",    'A') );
	REQUIRE(test(" A",                   "%2c",   'A') );
	REQUIRE(test("         A",           "%10c",  'A') );
	REQUIRE(test("A         ",           "%-10c", 'A') );
}

TEST_CASE("misc", "[string][printf]")
{
	REQUIRE(test("53000atest-20 bit", "%u%u%ctest%d %s", 5, 3000, 'a', -20, "bit") );
	REQUIRE(test("0.33",       "%.*f", 2, 0.33333333) );
	REQUIRE(test("1",          "%.*d", -1, 1) );
	REQUIRE(test(" ",          "% .0d", 0) );
	REQUIRE(test("     00004", "%10.5d", 4) );
	REQUIRE(test("hi x",       "%*sx", -3, "hi") );
	REQUIRE(test("0.33",       "%.*g", 2, 0.33333333) );
//	REQUIRE(test("3.33e-01",   "%.*e", 2, 0.33333333) );
	REQUIRE(test("",           "") );
}

TEST_CASE("Integer precision", "[string][printf]")
{
	SECTION("Precision zero suppresses zero-value output")
	{
		REQUIRE(test("",       "%.0d", 0) );
		REQUIRE(test("",       "%.0i", 0) );
		REQUIRE(test("",       "%.0u", 0u) );
		REQUIRE(test("",       "%.0x", 0) );
		REQUIRE(test("",       "%.0o", 0) );
		REQUIRE(test("",       "%.d",  0) );
		REQUIRE(test("",       "%.i",  0) );
		REQUIRE(test("",       "%.u",  0u) );
		REQUIRE(test("",       "%.x",  0) );
		REQUIRE(test("",       "%.o",  0) );
	}

	SECTION("Precision pads with leading zeros")
	{
		REQUIRE(test("00042",  "%.5d", 42) );
		REQUIRE(test("-00042", "%.5d", -42) );
		REQUIRE(test("00042",  "%.5i", 42) );
		REQUIRE(test("-00042", "%.5i", -42) );
		REQUIRE(test("00042",  "%.5u", 42u) );
		REQUIRE(test("000ff",  "%.5x", 255) );
		REQUIRE(test("000FF",  "%.5X", 255) );
		REQUIRE(test("00777",  "%.5o", 511) );
	}

	SECTION("Precision with sign flags")
	{
		REQUIRE(test("+",      "%+.0d", 0) );
		REQUIRE(test("+",      "%+.0i", 0) );
		REQUIRE(test("+01",    "%+.2i", 1) );
	}

	SECTION("Width + precision combined")
	{
		REQUIRE(test("               01024", "%20.5d",  1024) );
		REQUIRE(test("              -01024", "%20.5d", -1024) );
		REQUIRE(test("               01024", "%20.5u", 1024u) );
		REQUIRE(test("                    ", "%20.d",  0) );
		REQUIRE(test("                    ", "%20.u",  0u) );
		REQUIRE(test("                    ", "%20.x",  0) );
		REQUIRE(test("00123               ", "%-20.5i", 123) );
	}

	SECTION("Precision overrides 0 flag")
	{
		REQUIRE(test("  042",  "%05.3d", 42) );
		REQUIRE(test(" -042",  "%05.3d", -42) );
		REQUIRE(test("  042",  "%05.3i", 42) );
		REQUIRE(test(" -042",  "%05.3i", -42) );
		REQUIRE(test("  00042", "%07.5u", 42u) );
	}
}

TEST_CASE("Space and plus flag", "[string][printf]")
{
	SECTION("Space flag without width")
	{
		REQUIRE(test(" 42",  "% d",  42) );
		REQUIRE(test("-42",  "% d", -42) );
		REQUIRE(test(" 0",   "% d",  0) );
		REQUIRE(test(" 0",   "% i",  0) );
	}

	SECTION("Plus flag without width")
	{
		REQUIRE(test("+42",  "%+d",  42) );
		REQUIRE(test("-42",  "%+d", -42) );
		REQUIRE(test("+0",   "%+i",  0) );
	}

	SECTION("Plus overrides space")
	{
		REQUIRE(test("+42",  "%+ d",  42) );
		REQUIRE(test("+42",  "% +d",  42) );
		REQUIRE(test("-42",  "%+ d", -42) );
		REQUIRE(test("+0",   "%+ d",  0) );
	}

	SECTION("Space flag on float")
	{
		REQUIRE(test(" 1.500000",  "% f",  1.5) );
		REQUIRE(test("-1.500000",  "% f", -1.5) );
	}
}

TEST_CASE("Zero-padding negative numbers", "[string][printf]")
{
	REQUIRE(test("-5",   "%01d",  -5) );
	REQUIRE(test("-5",   "%02d",  -5) );
	REQUIRE(test("-05",  "%03d",  -5) );
	REQUIRE(test("-005", "%04d",  -5) );
}

TEST_CASE("Star args", "[string][printf]")
{
	SECTION("Star width")
	{
		REQUIRE(test("         Z", "%*c", 10, 'Z') );
		REQUIRE(test("               Hello", "%*s", 20, "Hello") );
	}

	SECTION("Negative star width = left-justify")
	{
		REQUIRE(test("42        ", "%*d", -10, 42) );
		REQUIRE(test("-42       ", "%*d", -10, -42) );
		REQUIRE(test("x         ", "%*c", -10, 'x') );
		REQUIRE(test("hello     ", "%*s", -10, "hello") );
	}

	SECTION("Star precision")
	{
		REQUIRE(test("01",    "%.*i", 2, 1) );
		REQUIRE(test("h",     "%.*s", 1, "hello world") );
	}

	SECTION("Negative star precision = precision omitted")
	{
		REQUIRE(test("42",    "%.*d", -1, 42) );
		REQUIRE(test("0",     "%.*d", -1, 0) );
		REQUIRE(test("hello", "%.*s", -1, "hello") );
	}

	SECTION("Star width + star precision")
	{
		REQUIRE(test("        07", "%*.*i", 10, 2, 7) );
	}

	SECTION("Star width/precision with %%")
	{
		REQUIRE(test(" 1.23 %", "%*.*f %%", 5, 2, 1.23) );
	}
}
