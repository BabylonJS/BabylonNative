/*-
* Copyright 2012-2018 Matthew Endsley
* All rights reserved
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted providing that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include "test.h"

#include <tinystl/unordered_map.h>
#include <tinystl/string.h>
#include <utility>

template<typename K, typename V>
static void comparemaps(const tinystl::unordered_map<K, V>& m, const tinystl::unordered_map<K, V>& expected) {
	CHECK( m.size() == expected.size() );

	typedef typename tinystl::unordered_map<K, V>::const_iterator iterator;
	for (iterator it = expected.begin(), end = expected.end(); it != end; ++it) {
		iterator found = m.find((*it).first);
		CHECK( found != m.end() );
		CHECK( (*found).second == (*it).second );
	}
}

TEST_CASE("uomap_constructor", "[tinystl]") {
	typedef tinystl::unordered_map<int, int> unordered_map;
	using tinystl::make_pair;

	unordered_map baseline;
	comparemaps(baseline, baseline); // test with empty maps
	baseline.insert(make_pair(5, 1));
	baseline.insert(make_pair(6, 2));
	CHECK( 2 == baseline.size() );
	CHECK( baseline.find(5) != baseline.end() );
	CHECK( baseline[5] == 1 );
	CHECK( baseline.find(6) != baseline.end() );
	CHECK( baseline[6] == 2 );
	comparemaps(baseline, baseline);

	{
		unordered_map m;

		CHECK( m.empty() );
		CHECK( m.size() == 0 );
	}

	{
		unordered_map m = baseline;

		comparemaps(m, baseline);
	}

	{
		unordered_map other = baseline;
		unordered_map m = std::move(other);

		comparemaps(m, baseline);
		CHECK( other.empty() );
	}
}

TEST_CASE("uomap_assign", "[tinystl]") {
	typedef tinystl::unordered_map<int, int> unordered_map;
	using tinystl::make_pair;

	unordered_map baseline;
	baseline.insert(make_pair(5, 1));
	baseline.insert(make_pair(6, 2));
	CHECK( 2 == baseline.size() );
	CHECK( baseline.find(5) != baseline.end() );
	CHECK( baseline[5] == 1 );
	CHECK( baseline.find(6) != baseline.end() );
	CHECK( baseline[6] == 2 );
	comparemaps(baseline, baseline);

	{
		unordered_map m;
		m = baseline;

		comparemaps(m, baseline);
	}

	{
		unordered_map m;
		for (int ii = 0; ii != 10; ++ii)
			m.insert(make_pair(ii, 10*ii));

		m = baseline;

		comparemaps(m, baseline);
	}

	{
		unordered_map other = baseline;
		unordered_map m;
		m = std::move(other);

		comparemaps(m, baseline);
		CHECK( other.empty() );
	}

	{
		unordered_map other = baseline;
		unordered_map m;
		for (int ii = 0; ii != 10; ++ii)
			m.insert(make_pair(ii, 10*ii));

		m = std::move(other);

		comparemaps(m, baseline);
		CHECK( other.empty() );
	}
}

TEST_CASE("uomap_insert", "[tinystl]") {
	using tinystl::string;
	using tinystl::pair;
	typedef tinystl::unordered_map<string, string> unordered_map;
	typedef pair<unordered_map::iterator, bool> inspair;

	{
		unordered_map m;
		m.insert(make_pair(string("hello"), string("world")));
		CHECK( m.find("hello") != m.end() );
	}

	{
		const pair<string, string> p("hello", "world");
		unordered_map m;
		inspair p1 = m.insert(p);
		CHECK( p1.second );
		CHECK( (*p1.first).first == tinystl::string("hello") );
		CHECK( (*p1.first).second == tinystl::string("world") );

		inspair p2 = m.insert(p);
		CHECK( !p2.second );
		CHECK( p2.first == p1.first );
	}

	{
		unordered_map m;
		m.emplace(pair<string, string>("hello", "world"));

		CHECK( m.find("hello") != m.end() );
	}

	{
		unordered_map m;
		inspair p1 = m.emplace(pair<string, string>("hello", "world"));
		CHECK( p1.second );
		CHECK( (*p1.first).first == tinystl::string("hello") );
		CHECK( (*p1.first).second == tinystl::string("world") );

		inspair p2 = m.emplace(pair<string, string>("hello", "world"));
		CHECK( !p2.second );
		CHECK( p2.first == p1.first );
	}

	{
		unordered_map m;
		pair<string, string> p("hello", "world");
		m.emplace(std::move(p));

		CHECK( m.find("hello") != m.end() );
		CHECK( p.first.size() == 0 );
		CHECK( p.second.size() == 0 );
	}
}

TEST_CASE("uomap_iterate", "[tinystl]") {
	typedef tinystl::unordered_map<size_t, size_t> unordered_map;
	{
		unordered_map m;
		for (size_t i = 0; i < 1000; ++i) {
			CHECK( m.size() == i );
			size_t count = 0;
			for (auto it = m.begin(); it != m.end(); ++it) {
				count++;
			}
			CHECK( count == i );

			m.insert(tinystl::make_pair(17 * i, 101 * i));
		}
	}
}
