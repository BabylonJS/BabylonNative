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

#include <tinystl/unordered_set.h>
#include <tinystl/string.h>
#include <utility>

template<typename T>
static void comparesets(const tinystl::unordered_set<T>& s, const tinystl::unordered_set<T>& expected) {
	CHECK( s.size() == expected.size() );

	typedef typename tinystl::unordered_set<T>::const_iterator iterator;
	for (iterator it = expected.begin(), end = expected.end(); it != end; ++it) {
		CHECK( s.find(*it) != s.end() );
	}
}

TEST_CASE("uoset_constructor", "[tinystl]") {
	typedef tinystl::unordered_set<int> unordered_set;

	unordered_set baseline;
	comparesets(baseline, baseline); // test on empty
	baseline.insert(5);
	baseline.insert(6);
	CHECK( 2 == baseline.size() );
	CHECK( baseline.find(5) != baseline.end() );
	CHECK( baseline.find(6) != baseline.end() );
	comparesets(baseline, baseline);

	{
		unordered_set s;

		CHECK( s.empty() );
		CHECK( s.size() == 0 );
	}

	{
		unordered_set s = baseline;

		comparesets(s, baseline);
	}

	{
		unordered_set other = baseline;
		unordered_set s = std::move(other);

		comparesets(s, baseline);
		CHECK( other.empty() );
	}
}

TEST_CASE("uoset_assign", "[tinystl]") {
	typedef tinystl::unordered_set<int> unordered_set;

	unordered_set baseline;
	baseline.insert(5);
	baseline.insert(6);
	CHECK( 2 == baseline.size() );
	CHECK( baseline.find(5) != baseline.end() );
	CHECK( baseline.find(6) != baseline.end() );
	comparesets(baseline, baseline);

	{
		unordered_set s;
		s = baseline;

		comparesets(s, baseline);
	}

	{
		unordered_set s;
		for (int ii = 0; ii != 10; ++ii)
			s.insert(ii);

		s = baseline;

		comparesets(s, baseline);
	}

	{
		unordered_set other = baseline;
		unordered_set s;
		s = std::move(other);

		comparesets(s, baseline);
		CHECK( other.empty() );
	}

	{
		unordered_set other = baseline;
		unordered_set s;
		for (int ii = 0; ii != 10; ++ii)
			s.insert(ii);

		s = std::move(other);

		comparesets(s, baseline);
		CHECK( other.empty() );
	}
}

TEST_CASE("uoset_insert", "[tinystl]") {
	typedef tinystl::unordered_set<tinystl::string> unordered_set;
	typedef tinystl::pair<unordered_set::iterator, bool> pair;

	{
		unordered_set s;
		s.insert("hello");
		CHECK( s.find("hello") != s.end() );
	}

	{
		unordered_set s;
		pair p1 = s.insert("hello");
		CHECK( p1.second );
		CHECK( (*p1.first) == tinystl::string("hello") );

		pair p2 = s.insert("hello");
		CHECK( !p2.second );
		CHECK( p2.first == p1.first );
	}

	{
		unordered_set s;
		s.emplace("hello");

		CHECK( s.find("hello") != s.end() );
	}

	{
		unordered_set s;
		pair p1 = s.emplace("hello");
		CHECK( p1.second );
		CHECK( (*p1.first) == tinystl::string("hello") );

		pair p2 = s.emplace("hello");
		CHECK( !p2.second );
		CHECK( p2.first == p1.first );
	}

	{
		unordered_set s;
		tinystl::string key("hello");
		s.emplace(std::move(key));

		CHECK( s.find("hello") != s.end() );
		CHECK( key.size() == 0 );
	}
}

TEST_CASE("uoset_iterate", "[tinystl]") {
	typedef tinystl::unordered_set<int> unordered_set;
	{
		unordered_set s;
		for (size_t i = 0; i < 1000; ++i) {
			CHECK( s.size() == i );
			size_t count = 0;
			for (auto it = s.begin(); it != s.end(); ++it) {
				count++;
			}
			CHECK( count == i );

			s.insert(int(17 * i));
		}
	}
}
