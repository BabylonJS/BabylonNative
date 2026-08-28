/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/settings.h>
#include <bx/file.h>

namespace
{
	class CountingAllocator : public bx::AllocatorI
	{
	public:
		CountingAllocator()
			: m_live(0)
		{
		}

		virtual ~CountingAllocator()
		{
		}

		virtual void* realloc(void* _ptr, size_t _size, size_t _align, const char* _filePath, uint32_t _line) override
		{
			if (NULL == _ptr
			&&  0    != _size)
			{
				++m_live;
			}
			else if (NULL != _ptr
				 &&  0    == _size)
			{
				--m_live;
			}

			return m_allocator.realloc(_ptr, _size, _align, _filePath, _line);
		}

		int32_t m_live;

	private:
		bx::DefaultAllocator m_allocator;
	};

} // namespace

TEST_CASE("Settings", "")
{
	bx::FilePath filePath;
	filePath.set(bx::Dir::Temp);
	filePath.join("settings.ini");

	bx::DefaultAllocator allocator;

	bx::Settings settings(&allocator);

	settings.set("meh/podmac", "true");
	settings.set("test/foo/bar/abvgd", "1389");

	bx::FileWriter writer;
	if (bx::open(&writer, filePath, false, bx::ErrorIgnore{}) )
	{
		bx::write(&writer, settings, bx::ErrorIgnore{});
		bx::close(&writer);
	}

	REQUIRE(settings.get("meh").isEmpty() );
	REQUIRE(0 == bx::strCmp(settings.get("meh/podmac"), "true") );
	REQUIRE(0 == bx::strCmp(settings.get("test/foo/bar/abvgd"), "1389") );

	settings.remove("meh/podmac");
	REQUIRE(settings.get("meh/podmac").isEmpty() );

	settings.clear();

	bx::FileReader reader;
	if (bx::open(&reader, filePath, bx::ErrorIgnore{}) )
	{
		bx::read(&reader, settings, bx::ErrorIgnore{});
		bx::close(&reader);
	}

	REQUIRE(settings.get("meh").isEmpty() );
	REQUIRE(0 == bx::strCmp(settings.get("meh/podmac"), "true") );
	REQUIRE(0 == bx::strCmp(settings.get("test/foo/bar/abvgd"), "1389") );
}

TEST_CASE("Settings parse", "")
{
	bx::DefaultAllocator allocator;

	const bx::StringView ini(
		"; leading comment\n"
		"globalKey=globalValue\n"
		"\n"
		"[section]\n"
		"\tkey = value \n"
		"spaces = a b c\n"
		"; comment inside section\n"
		"empty=\n"
		"\n"
		"[ padded ]\n"
		"k=v\n"
		"lineWithoutEquals\n"
		"=valueWithoutName\n"
		"\n"
		"[dup]\n"
		"x=1\n"
		"x=2"
		);

	bx::Settings settings(&allocator, ini.getPtr(), ini.getLength() );

	REQUIRE(0 == bx::strCmp(settings.get("globalKey"), "globalValue") );

	REQUIRE(0 == bx::strCmp(settings.get("section/key"), "value") );
	REQUIRE(0 == bx::strCmp(settings.get("section/spaces"), "a b c") );

	REQUIRE(settings.get("section/empty").isEmpty() );

	REQUIRE(0 == bx::strCmp(settings.get("padded/k"), "v") );

	REQUIRE(settings.get("padded/lineWithoutEquals").isEmpty() );

	REQUIRE(0 == bx::strCmp(settings.get("dup/x"), "2") );

	REQUIRE(settings.get("section/; comment inside section").isEmpty() );

	REQUIRE(0 == bx::strCmp(settings.get("SECTION/KEY"), "value") );

	REQUIRE(settings.get("nosuch/key").isEmpty() );
	REQUIRE(settings.get("nosuch").isEmpty() );
}

TEST_CASE("Settings parse CRLF", "")
{
	bx::DefaultAllocator allocator;

	const bx::StringView ini(
		"[a]\r\n"
		"key = value\r\n"
		"\r\n"
		"[b]\r\n"
		"key=other\r\n"
		);

	bx::Settings settings(&allocator, ini.getPtr(), ini.getLength() );

	REQUIRE(0 == bx::strCmp(settings.get("a/key"), "value") );
	REQUIRE(0 == bx::strCmp(settings.get("b/key"), "other") );
}

TEST_CASE("Settings empty", "")
{
	bx::DefaultAllocator allocator;

	bx::Settings settings(&allocator);
	REQUIRE(settings.get("anything").isEmpty() );

	const bx::StringView comments("; just a comment\n\n\n   \n");
	settings.load(comments.getPtr(), comments.getLength() );
	REQUIRE(settings.get("anything").isEmpty() );

	settings.load(NULL, 0);
	REQUIRE(settings.get("anything").isEmpty() );
}

TEST_CASE("Settings roundtrip", "")
{
	bx::DefaultAllocator allocator;

	bx::Settings settings(&allocator);
	settings.set("global", "0");
	settings.set("a/one", "1");
	settings.set("a/two", "2");
	settings.set("b/nested/three", "3");

	char tmp[512];
	bx::StaticMemoryBlockWriter writer(tmp, sizeof(tmp) );
	const int32_t size = bx::write(&writer, settings, bx::ErrorIgnore{});
	REQUIRE(0 < size);

	bx::Settings reloaded(&allocator, tmp, uint32_t(size) );
	REQUIRE(0 == bx::strCmp(reloaded.get("global"), "0") );
	REQUIRE(0 == bx::strCmp(reloaded.get("a/one"), "1") );
	REQUIRE(0 == bx::strCmp(reloaded.get("a/two"), "2") );
	REQUIRE(0 == bx::strCmp(reloaded.get("b/nested/three"), "3") );

	reloaded.set("a/one", "one-updated");
	REQUIRE(0 == bx::strCmp(reloaded.get("a/one"), "one-updated") );

	reloaded.remove("a/one");
	reloaded.remove("a/two");
	REQUIRE(reloaded.get("a/one").isEmpty() );
	REQUIRE(reloaded.get("a/two").isEmpty() );
	REQUIRE(0 == bx::strCmp(reloaded.get("b/nested/three"), "3") );
}

TEST_CASE("Settings allocations balanced", "")
{
	CountingAllocator allocator;

	{
		const bx::StringView ini(
			"global=0\n"
			"[a]\n"
			"one=1\n"
			"two=2\n"
			"[b]\n"
			"three=3\n"
			);

		bx::Settings settings(&allocator, ini.getPtr(), ini.getLength() );

		REQUIRE(0 < allocator.m_live);

		settings.set("c/four", "4");
		settings.set("a/one", "overwritten");
		settings.remove("b/three");

		settings.load(ini.getPtr(), ini.getLength() );

		settings.clear();

		settings.set("d/five", "5");
	}

	REQUIRE(0 == allocator.m_live);
}
