/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/string.h>
#include <bx/url.h>

struct UrlTest
{
	bool result;
	const char* url;
	const char* tokens[bx::UrlView::Count];
};

static const UrlTest s_urlTest[] =
{
	{ true
	, "scheme://username:password@host.rs:80/this/is/path/index.php?query=\"value\"#fragment",
	{ "scheme", "username", "password", "host.rs", "80", "/this/is/path/index.php", "query=\"value\"", "fragment" }
	},
	{ true
	, "scheme://host.rs/",
	{ "scheme", "", "", "host.rs", "", "/", "", "" },
	},
	{ true
	, "scheme://host.rs:1389/",
	{ "scheme", "", "", "host.rs", "1389", "/", "", "" },
	},
	{ true
	, "host.rs/abvgd.html",
	{ "", "", "", "host.rs", "", "/abvgd.html", "", "" },
	},
	{ true
	, "https://192.168.0.1:8080/",
	{ "https", "", "", "192.168.0.1", "8080", "/", "", "" },
	},

	{ true
	, "file:///d:/tmp/archive.tar.gz",
	{ "file", "", "", "", "", "/d:/tmp/archive.tar.gz", "", "" },
	},

	{ true
	, "scheme://host.rs/path#fragment",
	{ "scheme", "", "", "host.rs", "", "/path", "", "fragment" },
	},
	{ true
	, "scheme://host.rs/path?query#fragment",
	{ "scheme", "", "", "host.rs", "", "/path", "query", "fragment" },
	},
	{ true
	, "scheme://host.rs/path?",
	{ "scheme", "", "", "host.rs", "", "/path", "", "" },
	},
	{ true
	, "scheme://username:password@host.rs/",
	{ "scheme", "username", "password", "host.rs", "", "/", "", "" },
	},
	{ true
	, "scheme://host.rs",
	{ "scheme", "", "", "host.rs", "", "", "", "" },
	},
	{ false // Fragment must not precede query.
	, "scheme://host.rs/path#fragment?query",
	{ "", "", "", "", "", "", "", "" },
	},
	{ false // Neither scheme nor path.
	, "host.rs",
	{ "", "", "", "", "", "", "", "" },
	},
	{ false // Scheme must be alpha.
	, "1scheme://host.rs/",
	{ "", "", "", "", "", "", "", "" },
	},
};

TEST_CASE("tokenizeUrl", "[url][string]")
{
	bx::UrlView url;

	for (uint32_t ii = 0; ii < BX_COUNTOF(s_urlTest); ++ii)
	{
		const UrlTest& urlTest = s_urlTest[ii];

		bool result = url.parse(urlTest.url);
		REQUIRE(urlTest.result == result);

		if (result)
		{
			for (uint32_t token = 0; token < bx::UrlView::Count; ++token)
			{
//				char tmp[1024];
//				strCopy(tmp, BX_COUNTOF(tmp), url.get(bx::UrlView::Enum(token)) );
//				printf("`%s`, expected: `%s`\n", tmp, urlTest.tokens[token]);

				REQUIRE(0 == bx::strCmp(urlTest.tokens[token], url.get(bx::UrlView::Enum(token)) ) );
			}
		}
	}
}

struct UrlEncodeTest
{
	const char* input;
	bx::UrlEncoding::Enum encoding;
	const char* expected;
};

static const UrlEncodeTest s_urlEncodeTest[] =
{
	{ "",              bx::UrlEncoding::Component, ""                    },
	{ "abcABC123",     bx::UrlEncoding::Component, "abcABC123"           },
	{ "-_.~",          bx::UrlEncoding::Component, "-_.~"                },
	{ "a b",           bx::UrlEncoding::Component, "a%20b"               },
	{ "100%",          bx::UrlEncoding::Component, "100%25"              },
	{ "a+b=c&d",       bx::UrlEncoding::Component, "a%2Bb%3Dc%26d"       },
	{ "\xc3\xa9",      bx::UrlEncoding::Component, "%C3%A9"              },
	{ "/tmp/a b.txt",  bx::UrlEncoding::Component, "%2Ftmp%2Fa%20b.txt"  },
	{ "/tmp/a b.txt",  bx::UrlEncoding::Path,      "/tmp/a%20b.txt"      },
	{ "\xc3\xa9",      bx::UrlEncoding::Path,      "%C3%A9"              },
};

TEST_CASE("urlEncode", "[url][string]")
{
	for (uint32_t ii = 0; ii < BX_COUNTOF(s_urlEncodeTest); ++ii)
	{
		const UrlEncodeTest& test = s_urlEncodeTest[ii];

		char tmp[128];
		const int32_t len = bx::urlEncode(tmp, BX_COUNTOF(tmp), test.input, test.encoding);

		REQUIRE(0 == bx::strCmp(test.expected, tmp) );
		REQUIRE(len == bx::strLen(test.expected) );
	}
}

TEST_CASE("urlEncode truncate", "[url][string]")
{
	{
		char tmp[4];
		REQUIRE(3 == bx::urlEncode(tmp, BX_COUNTOF(tmp), "abcdef") );
		REQUIRE(0 == bx::strCmp("abc", tmp) );
	}

	{
		char tmp[4];
		REQUIRE(3 == bx::urlEncode(tmp, BX_COUNTOF(tmp), "  ") );
		REQUIRE(0 == bx::strCmp("%20", tmp) );
	}

	{
		char tmp[3];
		REQUIRE(0 == bx::urlEncode(tmp, BX_COUNTOF(tmp), " ") );
		REQUIRE(0 == bx::strCmp("", tmp) );
	}

	{
		char tmp[1];
		REQUIRE(0 == bx::urlEncode(tmp, BX_COUNTOF(tmp), "abc") );
		REQUIRE(0 == bx::strCmp("", tmp) );
	}
}

TEST_CASE("urlEncode StringView", "[url][string]")
{
	const bx::StringView str("abc def", 3);

	char tmp[128];
	REQUIRE(3 == bx::urlEncode(tmp, BX_COUNTOF(tmp), str) );
	REQUIRE(0 == bx::strCmp("abc", tmp) );
}
