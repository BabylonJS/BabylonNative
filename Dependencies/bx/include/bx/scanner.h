/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SCANNER_H_HEADER_GUARD
#define BX_SCANNER_H_HEADER_GUARD

#include <bx/string.h>

namespace bx
{
	/// Forward-only cursor over a string view, used to tokenize text input.
	///
	/// Scanner keeps a cursor inside immutable input string, and tracks line number as cursor
	/// moves. `accept*` functions advance cursor when they match, `peek*` functions perform the
	/// same test without moving cursor.
	///
	/// All returned string views point into input string, and no copies are made. This includes
	/// empty string views returned when nothing matched, which point at cursor rather than being
	/// default constructed. They can be used with `seek` and `between`.
	///
	/// @attention Scanner doesn't own input string. Input string must outlive Scanner.
	///
	class Scanner
	{
	public:
		/// Character class matched by `accept`, `peek`, and `acceptUntil`.
		///
		enum class Class : uint8_t
		{
			Space,      //!< Run of whitespace characters.
			NonSpace,   //!< Run of non-whitespace characters.
			Identifier, //!< Run of alphanumeric characters and `_`.
			EndOfLine,  //!< Everything up to, but not including, line terminator.
			NewLine,    //!< Everything up to, and including, line terminator.
		};

		/// Position inside input string returned by `getCursor`.
		///
		enum class Cursor : uint8_t
		{
			Current,   //!< Current cursor position.
			LineStart, //!< First character of line cursor is on.
			LineEnd,   //!< Position of `\n` that terminates line cursor is on, or end of input.
		};

		/// Default constructor is not available. Scanner must be constructed with input string.
		///
		Scanner() = delete;

		/// Constructor.
		///
		/// @param[in] _input Input string to scan. It's not copied, and it must outlive Scanner.
		///
		Scanner(const StringView& _input);

		/// Rewind cursor to the beginning of input string, and reset line counter.
		///
		void reset();

		/// Accept everything from cursor to the end of input string.
		///
		/// @returns Accepted string view, or empty string view if input is exhausted.
		///
		StringView acceptAll();

		/// Accept single character.
		///
		/// @returns Accepted character, or empty string view if input is exhausted.
		///
		StringView accept();

		/// Accept single character if it matches `_ch`.
		///
		/// @param[in] _ch Character to match.
		///
		/// @returns Accepted character, or empty string view if it doesn't match.
		///
		StringView accept(char _ch);

		/// Accept single character if it matches any of the characters passed in.
		///
		/// @param[in] _ch0 First character to match.
		/// @param[in] _ch1 Second character to match.
		///
		/// @returns Accepted character, or empty string view if it doesn't match.
		///
		StringView accept(char _ch0, char _ch1);

		/// Accept single character if it matches any of the characters passed in.
		///
		/// @param[in] _ch0 First character to match.
		/// @param[in] _ch1 Second character to match.
		/// @param[in] _args Additional characters to match.
		///
		/// @returns Accepted character, or empty string view if it doesn't match.
		///
		template<typename ...Args>
		StringView accept(char _ch0, char _ch1, Args... _args);

		/// Accept `_str` if input at cursor starts with it.
		///
		/// @param[in] _str String to match.
		///
		/// @returns Accepted string view, or empty string view if it doesn't match.
		///
		StringView accept(const StringView& _str);

		/// Accept leading run of characters belonging to `_class`.
		///
		/// @param[in] _class Character class to match.
		///
		/// @returns Accepted string view, or empty string view if it doesn't match.
		///
		StringView accept(Class _class);

		/// Accept single character if `_fn` returns true for it.
		///
		/// @param[in] _fn Character test function.
		///
		/// @returns Accepted character, or empty string view if it doesn't match.
		///
		StringView accept(CharTestFn _fn);

		/// Accept characters as long as they are contained in `_any`.
		///
		/// @param[in] _any Set of characters to match.
		///
		/// @returns Accepted string view, or empty string view if it doesn't match.
		///
		StringView acceptWhile(const StringView& _any);

		/// Accept characters as long as `_fn` returns true for them.
		///
		/// @param[in] _fn Character test function.
		///
		/// @returns Accepted string view, or empty string view if it doesn't match.
		///
		StringView acceptWhile(CharTestFn _fn);

		/// Accept characters until `_find` is found. Cursor stops at `_find`, and `_find` is not
		/// accepted.
		///
		/// @param[in] _find String to search for.
		///
		/// @returns Accepted string view, or empty string view at cursor if `_find` is not found,
		///   or it's already at cursor.
		///
		/// @attention Returned string view is empty in both cases, and cursor doesn't move. If
		///   these two cases have to be told apart, test for `_find` separately with `peek`.
		///
		StringView acceptUntil(const StringView& _find);

		/// Accept leading run of characters belonging to `_class`.
		///
		/// @param[in] _class Character class to match.
		///
		/// @returns Accepted string view, or empty string view if it doesn't match.
		///
		/// @remarks Same as `accept(Class)`. It's provided for readability when used with
		///   `Class::EndOfLine` and `Class::NewLine`.
		///
		StringView acceptUntil(Class _class);

		/// Peek at single character without moving cursor.
		///
		/// @returns Next character, or empty string view if input is exhausted.
		///
		StringView peek() const;

		/// Peek at single character without moving cursor, if it matches `_ch`.
		///
		/// @param[in] _ch Character to match.
		///
		/// @returns Next character, or empty string view if it doesn't match.
		///
		StringView peek(char _ch) const;

		/// Peek at `_str` without moving cursor, if input at cursor starts with it.
		///
		/// @param[in] _str String to match.
		///
		/// @returns Matched string view, or empty string view if it doesn't match.
		///
		StringView peek(const StringView& _str) const;

		/// Peek at leading run of characters belonging to `_class`, without moving cursor.
		///
		/// @param[in] _class Character class to match.
		///
		/// @returns Matched string view. It's zero length if it doesn't match.
		///
		StringView peek(Class _class) const;

		/// Move cursor to `_to`. Line number is adjusted accordingly, and cursor can move backward.
		///
		/// @param[in] _to Position to move cursor to. It must be a string view into input string.
		///
		/// @returns True if cursor moved, or false if `_to` is not a string view into input string.
		///
		bool seek(const StringView& _to);

		/// Move cursor by `_bytes`. Negative value moves cursor backward. Line number is adjusted
		/// accordingly, and value is clamped to input string bounds.
		///
		/// @param[in] _bytes Number of bytes to move cursor by.
		///
		/// @returns Always true.
		///
		bool seek(int32_t _bytes);

		/// Returns one-based line number cursor is on.
		///
		/// @returns Line number.
		///
		uint32_t getLine() const;

		/// Returns one-based column number cursor is on.
		///
		/// @returns Column number.
		///
		uint32_t getColumn() const;

		/// Returns cursor position as zero length string view.
		///
		/// @param[in] _which Position to return.
		///
		/// @returns Zero length string view at requested position.
		///
		/// @remarks Returned string view keeps pointer into input string even though it's empty,
		///   and it can be passed to `seek` to return to this position later, or to `between` to
		///   capture text that can legitimately be empty.
		///
		StringView getCursor(Cursor _which = Cursor::Current) const;

		/// Returns text between `_from` and current cursor position.
		///
		/// @param[in] _from Position to measure from, usually obtained by `getCursor`.
		///
		/// @returns String view between `_from` and cursor. Order of arguments doesn't matter,
		///   `_from` can be either before or after cursor.
		///
		StringView between(const StringView& _from) const;

		/// Returns true if input string is exhausted.
		///
		/// @returns True if cursor reached end of input string, otherwise returns false.
		///
		bool isDone() const;

	private:
		/// Move cursor to `_to`, and update line number.
		///
		/// @param[in] _to Position to move cursor to. Empty string view is treated as cursor
		///   position obtained by `getCursor` rather than as span, and cursor moves to it.
		///
		/// @returns String view between old and new cursor position.
		///
		StringView moveTo(const StringView& _to);

		/// Move cursor by `_bytes` clamped to input string bounds, and update line number.
		///
		/// @param[in] _bytes Number of bytes to move cursor by.
		///
		/// @returns String view between old and new cursor position.
		///
		StringView moveBy(int32_t _bytes);

		/// Returns end of leading run of characters belonging to `_class`.
		///
		/// @param[in] _class Character class to match.
		///
		/// @returns String view starting past the run, and ending at end of input string.
		///
		StringView strFunc(Class _class) const;

		/// Count number of line terminators in `_str`.
		///
		/// @param[in] _str String to count line terminators in.
		///
		/// @returns Number of lines.
		///
		static uint32_t countLines(const StringView& _str);

		const StringView m_input;
		StringView m_tail;
		uint32_t   m_line;
	};

	/// Splits string into lines.
	///
	/// Line terminator is `\n` or `\r\n`, and it's not part of returned line. Trailing `\r` left
	/// by malformed input is trimmed.
	///
	/// All returned string views point into input string, and no copies are made.
	///
	/// @attention LineReader doesn't own input string. Input string must outlive LineReader.
	///
	class LineReader
	{
	public:
		/// Constructor.
		///
		/// @param[in] _str Input string to read lines from. It's not copied, and it must outlive
		///   LineReader.
		///
		LineReader(const StringView& _str);

		/// Rewind to the beginning of input string, and reset line counter.
		///
		void reset();

		/// Read next line.
		///
		/// @returns Next line without line terminator. Line can be empty, and returned string view
		///   keeps pointer into input string. If input is exhausted, zero length string view at
		///   end of input string is returned.
		///
		StringView next();

		/// Returns one-based line number of line returned by the last `next` call.
		///
		/// @returns Line number, or 0 if `next` was not called yet.
		///
		uint32_t getLine() const;

		/// Returns true if input string is exhausted.
		///
		/// @returns True if all lines are read, otherwise returns false.
		///
		bool isDone() const;

	private:
		Scanner  m_scanner;
		uint32_t m_line;
	};

} // namespace bx

#include "inline/scanner.inl"

#endif // BX_SCANNER_H_HEADER_GUARD
