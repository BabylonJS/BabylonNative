/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include <bx/scanner.h>

namespace bx
{
	bool Scanner::seek(const StringView& _to)
	{
		if (contain(m_input, _to) )
		{
			moveTo(_to);
			return true;
		}

		BX_TRACE("StringView '%S' is not a view into input string!", &_to);
		return false;
	}

	uint32_t Scanner::getColumn() const
	{
		return uint32_t(between(getCursor(Cursor::LineStart) ).getLength() ) + 1;
	}

	StringView Scanner::getCursor(Cursor _which) const
	{
		const char* ptr = m_tail.getPtr();

		if (Cursor::LineStart == _which)
		{
			while (ptr > m_input.getPtr()
			&&     '\n' != ptr[-1])
			{
				--ptr;
			}
		}
		else if (Cursor::LineEnd == _which)
		{
			const char* term = m_input.getTerm();
			while (ptr < term
			&&     '\n' != *ptr)
			{
				++ptr;
			}
		}

		return StringView(ptr, ptr);
	}

	StringView Scanner::moveTo(const StringView& _to)
	{
		if (BX_LIKELY(!_to.isEmpty() ) )
		{
			if (BX_LIKELY(overlap(m_tail, _to) ) )
			{
				const StringView result(m_tail.getPtr(), _to.getTerm() );

				m_tail.set(_to.getTerm(), m_tail.getTerm() );
				m_line += countLines(result);

				return result;
			}

			const StringView result(_to.getPtr(), m_tail.getPtr() );

			m_tail.set(_to.getPtr(), m_tail.getTerm() );
			m_line -= countLines(result);

			return result;
		}

		if (_to.getPtr() != m_tail.getPtr()
		&&  contain(m_input, _to) )
		{
			moveBy(int32_t(_to.getPtr() - m_tail.getPtr() ) );
		}

		return getCursor();
	}

	StringView Scanner::strFunc(Class _class) const
	{
		switch (_class)
		{
		case Class::Space:
			return strLTrimSpace(m_tail);

		case Class::NonSpace:
			return strLTrimNonSpace(m_tail);

		case Class::Identifier:
			{
				const StringView word = strIdentifier(m_tail);
				return word.isEmpty()
					? m_tail
					: StringView(word.getTerm(), m_tail.getTerm() )
					;
			}

		case Class::EndOfLine:
			return strFindEol(m_tail);

		case Class::NewLine:
			return strFindNl(m_tail);

		default:
			BX_ASSERT(false, "Bug, _class can't be %d!", _class);
			BX_UNREACHABLE;
		}
	}

	StringView Scanner::moveBy(int32_t _bytes)
	{
		const int32_t len = m_tail.getLength();
		_bytes = clamp(_bytes, len - m_input.getLength(), len);

		if (BX_LIKELY(0 < _bytes) )
		{
			const StringView result(m_tail.getPtr(), m_tail.getPtr() + _bytes);

			m_tail.set(result.getTerm(), m_tail.getTerm() );
			m_line += countLines(result);

			return result;
		}

		const StringView result(m_tail.getPtr() + _bytes, m_tail.getPtr() );

		m_tail.set(result.getPtr(), m_tail.getTerm() );
		m_line -= countLines(result);

		return result;
	}

	uint32_t Scanner::countLines(const StringView& _str)
	{
		uint32_t line = 0;

		StringView str = strFindEol(_str);

		while (!str.isEmpty() )
		{
			++line;
			str = strFindEol(strFindNl(str) );
		}

		return line;
	}

	StringView LineReader::next()
	{
		if (!m_scanner.isDone() )
		{
			++m_line;

			const StringView cursor = m_scanner.getCursor();
			m_scanner.acceptUntil(Scanner::Class::EndOfLine);

			const StringView line = m_scanner.between(cursor);
			m_scanner.accept(Scanner::Class::NewLine);

			// A line terminator is `\n` or `\r\n`, and `Class::EndOfLine` already
			// excludes it. Only malformed input (`\r` at the end of input, or
			// `\r\r\n`) can leave a trailing `\r` behind.
			return strRTrim(line, "\r");
		}

		return m_scanner.getCursor();
	}

} // namespace bx
