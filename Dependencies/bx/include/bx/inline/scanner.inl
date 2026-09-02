/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SCANNER_H_HEADER_GUARD
#	error "Must be included from bx/scanner.h!"
#endif // BX_SCANNER_H_HEADER_GUARD

namespace bx
{
	inline Scanner::Scanner(const StringView& _input)
		: m_input(_input)
		, m_tail(_input)
		, m_line(1)
	{
	}

	inline void Scanner::reset()
	{
		m_tail = m_input;
		m_line = 1;
	}

	inline StringView Scanner::acceptAll()
	{
		StringView result = m_tail;
		moveBy(result.getLength() );

		return result;
	}

	inline StringView Scanner::accept()
	{
		return moveBy(1);
	}

	inline StringView Scanner::accept(char _ch)
	{
		if (!m_tail.isEmpty()
		&&  _ch == *m_tail.getPtr() )
		{
			return moveBy(1);
		}

		return getCursor();
	}

	inline StringView Scanner::accept(char _ch0, char _ch1)
	{
		if (!m_tail.isEmpty() )
		{
			const char ch = *m_tail.getPtr();

			if (ch == _ch0
			||  ch == _ch1)
			{
				return moveBy(1);
			}
		}

		return getCursor();
	}

	template<typename ...Args>
	inline StringView Scanner::accept(char _ch0, char _ch1, Args... _args)
	{
		StringView result = accept(_ch0, _ch1);

		if (!result.isEmpty() )
		{
			return result;
		}

		return accept(_args...);
	}

	inline StringView Scanner::accept(const StringView& _str)
	{
		if (hasPrefix(m_tail, _str) )
		{
			return moveTo({ m_tail.getPtr(), _str.getLength() });
		}

		return getCursor();
	}

	inline StringView Scanner::accept(Class _class)
	{
		return moveTo({ m_tail.getPtr(), strFunc(_class).getPtr() });
	}

	inline StringView Scanner::accept(CharTestFn _fn)
	{
		if (!m_tail.isEmpty()
		&&  _fn(*m_tail.getPtr() ) )
		{
			return moveBy(1);
		}

		return getCursor();
	}

	inline StringView Scanner::acceptWhile(const StringView& _any)
	{
		const StringView input = strLTrim(m_tail, _any);

		if (input.getPtr() != m_tail.getPtr() )
		{
			return moveTo({ m_tail.getPtr(), input.getPtr() });
		}

		return getCursor();
	}

	inline StringView Scanner::acceptWhile(CharTestFn _fn)
	{
		const char* ptr  = m_tail.getPtr();
		const char* term = m_tail.getTerm();

		while (ptr != term
		&&     _fn(*ptr) )
		{
			++ptr;
		}

		return moveTo({ m_tail.getPtr(), ptr });
	}

	inline StringView Scanner::acceptUntil(const StringView& _find)
	{
		StringView result = strFind(m_tail, _find);

		if (!result.isEmpty() )
		{
			return moveTo({ m_tail.getPtr(), result.getPtr() });
		}

		return getCursor();
	}

	inline StringView Scanner::acceptUntil(Class _class)
	{
		StringView result = strFunc(_class);
		return moveTo({ m_tail.getPtr(), result.getPtr() });
	}

	inline StringView Scanner::peek() const
	{
		return m_tail.isEmpty()
			? getCursor()
			: StringView(m_tail.getPtr(), m_tail.getPtr() + 1)
			;
	}

	inline StringView Scanner::peek(char _ch) const
	{
		if (!m_tail.isEmpty()
		&&  _ch == *m_tail.getPtr() )
		{
			return StringView(m_tail.getPtr(), m_tail.getPtr() + 1);
		}

		return getCursor();
	}

	inline StringView Scanner::peek(const StringView& _str) const
	{
		if (hasPrefix(m_tail, _str) )
		{
			return StringView(m_tail.getPtr(), _str.getLength() );
		}

		return getCursor();
	}

	inline StringView Scanner::peek(Class _class) const
	{
		return StringView(m_tail.getPtr(), strFunc(_class).getPtr() );
	}

	inline bool Scanner::seek(int32_t _bytes)
	{
		moveBy(_bytes);

		return true;
	}

	inline uint32_t Scanner::getLine() const
	{
		return m_line;
	}

	inline StringView Scanner::between(const StringView& _from) const
	{
		const char* a = _from.getPtr();
		const char* b = m_tail.getPtr();
		return a <= b ? StringView(a, b) : StringView(b, a);
	}

	inline bool Scanner::isDone() const
	{
		return m_tail.isEmpty();
	}

	inline LineReader::LineReader(const StringView& _str)
		: m_scanner(_str)
		, m_line(0)
	{
	}

	inline void LineReader::reset()
	{
		m_scanner.reset();
		m_line = 0;
	}

	inline uint32_t LineReader::getLine() const
	{
		return m_line;
	}

	inline bool LineReader::isDone() const
	{
		return m_scanner.isDone();
	}

} // namespace bx
