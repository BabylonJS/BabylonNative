/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx#license-bsd-2-clause
 */

#include "test.h"

#include <bx/scanner.h>

namespace bx
{
	uint32_t countLines(const StringView& _str)
	{
		Scanner scanner(_str);
		scanner.acceptAll();

		return scanner.getLine() - 1;
	}

	void printLines(const StringView& _str)
	{
		Scanner scanner(_str);

		while (!scanner.isDone() )
		{
			StringView sv = scanner.acceptUntil(Scanner::Class::EndOfLine);
			BX_TRACE("%d: '%S'", scanner.getLine(), &sv);

			scanner.acceptUntil(Scanner::Class::NewLine);
		}
	}

} // namespace bx

TEST_CASE("Scanner", "[scanner]")
{
	const bx::StringLiteral input =
		"test (foo) bar [baz]xyz\n"
		"    // test\n"
		"tset (foo) bar [baz]xyz\n"
		"    \n"
		"void main()\n"
		"{\n"
		"\treturn;\n"
		"}\n"
		;
	const uint32_t numLines = bx::countLines(input);

	bx::printLines(input);

	bx::Scanner scanner(input);

	REQUIRE(!scanner.isDone() );

	bx::StringView start = scanner.accept();

	REQUIRE("t" == start);
	REQUIRE("est " == scanner.acceptUntil("(") );
	REQUIRE("(" == scanner.accept('(') );

	REQUIRE(!scanner.isDone() );
	REQUIRE("foo" == scanner.accept(bx::Scanner::Class::Identifier) );
	REQUIRE(")" == scanner.accept(')') );

	REQUIRE(!scanner.isDone() );
	REQUIRE(" " == scanner.accept(' ') );

	REQUIRE(!scanner.isDone() );
	REQUIRE("b"  == scanner.accept('r', 'b', 'a') );
	REQUIRE("a"  == scanner.accept('b', 'a', 'r') );
	REQUIRE("r"  == scanner.accept('a', 'r', 'b') );

	REQUIRE(!scanner.isDone() );
	REQUIRE(" " == scanner.accept(' ') );

	REQUIRE(!scanner.isDone() );
	REQUIRE("[baz]" == scanner.acceptWhile("abz[]") );

	REQUIRE(!scanner.isDone() );
	REQUIRE("xyz" == scanner.accept("xyz") );

	REQUIRE(1 == scanner.getLine() );
	REQUIRE("\n    " == scanner.acceptUntil(bx::Scanner::Class::Space) );
	REQUIRE(2 == scanner.getLine() );

	scanner.acceptAll();
	REQUIRE(scanner.accept().isEmpty() );

	REQUIRE(numLines + 1 == scanner.getLine() );

	REQUIRE(scanner.isDone() );

	REQUIRE(scanner.seek(start) );
	REQUIRE(1 == scanner.getLine() );
	REQUIRE(!scanner.isDone() );

	REQUIRE(scanner.seek(INT32_MAX) );
	REQUIRE(numLines + 1 == scanner.getLine() );
	REQUIRE(scanner.isDone() );

	REQUIRE(scanner.seek(INT32_MIN) );
	REQUIRE(1 == scanner.getLine() );
	REQUIRE(!scanner.isDone() );
}

TEST_CASE("Scanner.class accept", "[scanner]")
{
	{
		bx::Scanner sc(bx::StringView("abc def") );
		REQUIRE("abc" == sc.accept(bx::Scanner::Class::Identifier) );
		REQUIRE(" def" == sc.acceptAll() );
	}

	{
		bx::Scanner sc(bx::StringView("   abc def") );
		REQUIRE("   " == sc.accept(bx::Scanner::Class::Space) );
		REQUIRE(!sc.isDone() );
		REQUIRE("abc def" == sc.acceptAll() );
	}

	{
		bx::Scanner sc(bx::StringView("abc") );
		REQUIRE(sc.accept(bx::Scanner::Class::Space).isEmpty() );
		REQUIRE("abc" == sc.acceptAll() );
	}

	{
		bx::Scanner sc(bx::StringView("abc   def") );
		REQUIRE("abc" == sc.accept(bx::Scanner::Class::NonSpace) );
		REQUIRE("   def" == sc.acceptAll() );
	}

	{
		bx::Scanner sc(bx::StringView("abc\ndef") );
		REQUIRE("abc" == sc.accept(bx::Scanner::Class::EndOfLine) );
		REQUIRE("\ndef" == sc.acceptAll() );
	}

	{
		bx::Scanner a(bx::StringView("   abc") );
		bx::Scanner b(bx::StringView("   abc") );
		REQUIRE("   " == a.accept(bx::Scanner::Class::Space) );
		REQUIRE("   " == b.acceptUntil(bx::Scanner::Class::Space) );
	}
}

TEST_CASE("Scanner.peek", "[scanner]")
{
	bx::Scanner sc(bx::StringView("abc def") );

	REQUIRE("a"   == sc.peek() );
	REQUIRE("a"   == sc.peek('a') );
	REQUIRE(sc.peek('x').isEmpty() );
	REQUIRE("abc" == sc.peek(bx::StringView("abc") ) );
	REQUIRE(sc.peek(bx::StringView("abx") ).isEmpty() );
	REQUIRE("abc" == sc.peek(bx::Scanner::Class::Identifier) );

	REQUIRE("a"  == sc.accept() );
	REQUIRE("bc" == sc.peek(bx::Scanner::Class::Identifier) );
	REQUIRE("bc" == sc.accept(bx::Scanner::Class::Identifier) );

	sc.acceptAll();
	REQUIRE(sc.isDone() );
	REQUIRE(sc.peek().isEmpty() );
	REQUIRE(sc.peek(bx::Scanner::Class::Identifier).isEmpty() );
}

TEST_CASE("Scanner.getCursor between", "[scanner]")
{
	bx::Scanner sc(bx::StringView("abc def") );

	const bx::StringView mark = sc.getCursor();
	REQUIRE(mark.isEmpty() );

	sc.accept(bx::Scanner::Class::Identifier);
	REQUIRE("abc" == sc.between(mark) );

	const bx::StringView here = sc.getCursor();
	sc.acceptAll();
	REQUIRE(" def" == sc.between(here) );
	REQUIRE(sc.seek(INT32_MIN) );
	REQUIRE("abc" == sc.between(here) );
}

TEST_CASE("Scanner.getCursor lines", "[scanner]")
{
	bx::Scanner sc(bx::StringView("abc\ndefgh\nij") );

	sc.accept(bx::Scanner::Class::Identifier);
	sc.accept('\n');
	sc.accept('d');
	sc.accept('e');

	REQUIRE("de"  == sc.between(sc.getCursor(bx::Scanner::Cursor::LineStart) ) );
	REQUIRE("fgh" == sc.between(sc.getCursor(bx::Scanner::Cursor::LineEnd) ) );
}

TEST_CASE("Scanner.getColumn", "[scanner]")
{
	bx::Scanner sc(bx::StringView("abc\ndef") );

	REQUIRE(1 == sc.getColumn() );
	sc.accept();
	REQUIRE(2 == sc.getColumn() );
	sc.accept(bx::Scanner::Class::Identifier);
	REQUIRE(4 == sc.getColumn() );

	sc.accept('\n');
	REQUIRE(2 == sc.getLine() );
	REQUIRE(1 == sc.getColumn() );
	sc.accept();
	REQUIRE(2 == sc.getColumn() );
}

TEST_CASE("Scanner.predicate accept", "[scanner]")
{
	bx::Scanner sc(bx::StringView("123abc  xyz") );

	REQUIRE("1" == sc.accept(bx::isNumeric) );
	REQUIRE(sc.accept(bx::isAlpha).isEmpty() );
	REQUIRE("2" == sc.accept(bx::isNumeric) );

	REQUIRE("3"   == sc.acceptWhile(bx::isNumeric) );
	REQUIRE("abc" == sc.acceptWhile(bx::isAlpha) );

	REQUIRE(sc.acceptWhile(bx::isNumeric).isEmpty() );
	REQUIRE(!sc.isDone() );

	REQUIRE("  "  == sc.acceptWhile(bx::isSpace) );
	REQUIRE("xyz" == sc.acceptWhile(bx::isAlphaNum) );
	REQUIRE(sc.isDone() );
}

TEST_CASE("Scanner.reset", "[scanner]")
{
	bx::Scanner sc(bx::StringView("abc\ndef\n") );

	sc.acceptAll();
	REQUIRE(sc.isDone() );
	REQUIRE(3 == sc.getLine() );

	sc.reset();
	REQUIRE(!sc.isDone() );
	REQUIRE(1 == sc.getLine() );
	REQUIRE(1 == sc.getColumn() );
	REQUIRE("abc" == sc.accept(bx::Scanner::Class::Identifier) );

	// Reset from a partially consumed state.
	sc.reset();
	REQUIRE("abc\ndef\n" == sc.acceptAll() );
}

TEST_CASE("Scanner.EndOfLine mixed", "[scanner]")
{
	// End of line must be the *first* terminator, even when a later line
	// uses \r\n and an earlier one uses a lone \n.
	bx::Scanner sc(bx::StringView("a\nb\r\n") );

	REQUIRE("a" == sc.acceptUntil(bx::Scanner::Class::EndOfLine) );
	REQUIRE("\n" == sc.accept(bx::Scanner::Class::NewLine) );
	REQUIRE("b" == sc.acceptUntil(bx::Scanner::Class::EndOfLine) );
	REQUIRE("\r\n" == sc.accept(bx::Scanner::Class::NewLine) );
	REQUIRE(sc.isDone() );

	REQUIRE(2 == bx::countLines(bx::StringView("a\nb\r\n") ) );
	REQUIRE(4 == bx::countLines(bx::StringView("a\nb\r\nc\rd\ne\r\n") ) );
}

TEST_CASE("LineReader", "[scanner][string]")
{
	{
		bx::LineReader lr(bx::StringView("") );
		REQUIRE(lr.isDone() );
		REQUIRE(lr.next().isEmpty() );
		REQUIRE(0 == lr.getLine() );
	}

	{
		bx::LineReader lr(bx::StringView("abc") );
		REQUIRE(!lr.isDone() );
		REQUIRE("abc" == lr.next() );
		REQUIRE(1 == lr.getLine() );
		REQUIRE(lr.isDone() );
	}

	{
		// \n, \r\n and empty lines interleaved.
		bx::LineReader lr(bx::StringView("a\r\nb\n\nc\r\n\r\nd") );

		REQUIRE("a" == lr.next() );
		REQUIRE("b" == lr.next() );
		REQUIRE(lr.next().isEmpty() );
		REQUIRE("c" == lr.next() );
		REQUIRE(lr.next().isEmpty() );
		REQUIRE("d" == lr.next() );
		REQUIRE(6 == lr.getLine() );
		REQUIRE(lr.isDone() );

		// next() past the end is a no-op.
		REQUIRE(lr.next().isEmpty() );
		REQUIRE(6 == lr.getLine() );
	}

	{
		// Trailing \r without \n is trimmed, interior \r is not a line break.
		bx::LineReader lr(bx::StringView("a\rb\nc\r") );

		REQUIRE("a\rb" == lr.next() );
		REQUIRE("c" == lr.next() );
		REQUIRE(lr.isDone() );
	}

	{
		// Only \r and \n are trimmed, other trailing whitespace is kept.
		bx::LineReader lr(bx::StringView("a  \r\n\tb\t\n") );

		REQUIRE("a  " == lr.next() );
		REQUIRE("\tb\t" == lr.next() );
		REQUIRE(lr.isDone() );
	}

	{
		bx::LineReader lr(bx::StringView("a\nb\n") );

		REQUIRE("a" == lr.next() );
		REQUIRE("b" == lr.next() );
		REQUIRE(lr.isDone() );
		REQUIRE(2 == lr.getLine() );

		lr.reset();

		REQUIRE(!lr.isDone() );
		REQUIRE(0 == lr.getLine() );
		REQUIRE("a" == lr.next() );
		REQUIRE("b" == lr.next() );
		REQUIRE(2 == lr.getLine() );
	}

	{
		// Returned views must point into the input, not at a temporary.
		const bx::StringView input("abc\ndef\n");

		bx::LineReader lr(input);

		const bx::StringView first = lr.next();
		REQUIRE(first.getPtr() == input.getPtr() );

		const bx::StringView second = lr.next();
		REQUIRE(second.getPtr() == input.getPtr() + 4);
	}
}
