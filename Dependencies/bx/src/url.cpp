/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bnet#license-bsd-2-clause
 */

#include <bx/scanner.h>
#include <bx/url.h>

namespace bx
{
	static bool isNotSlash(char _ch)
	{
		return '/' != _ch;
	}

	static bool isNotColon(char _ch)
	{
		return ':' != _ch;
	}

	static bool isNotQuery(char _ch)
	{
		return '?' != _ch;
	}

	static bool isNotFragment(char _ch)
	{
		return '#' != _ch;
	}

	static bool isNotQueryOrFragment(char _ch)
	{
		return isNotQuery(_ch) && isNotFragment(_ch);
	}

	UrlView::UrlView()
	{
	}

	void UrlView::clear()
	{
		for (uint32_t ii = 0; ii < Count; ++ii)
		{
			m_tokens[ii].clear();
		}
	}

	bool UrlView::parse(const StringView& _url)
	{
		clear();

		Scanner scanner(_url);

		const StringView scheme = scanner.acceptUntil("://");

		const bool hasScheme = !scanner.accept("://").isEmpty();

		if (hasScheme)
		{
			if (!isAlpha(scheme) )
			{
				return false;
			}

			m_tokens[Scheme].set(scheme);
		}

		const StringView authority = scanner.acceptWhile(isNotSlash);

		const bool hasPath = !scanner.peek('/').isEmpty();

		if (!hasScheme
		&&  !hasPath)
		{
			return false;
		}

		if (hasPath)
		{
			m_tokens[Path].set(scanner.acceptWhile(isNotQueryOrFragment) );

			if (!scanner.accept('?').isEmpty() )
			{
				m_tokens[Query].set(scanner.acceptWhile(isNotFragment) );
			}

			if (!scanner.accept('#').isEmpty() )
			{
				m_tokens[Fragment].set(scanner.acceptWhile(isNotQuery) );
			}

			// Anything left over is a query following a fragment.
			if (!scanner.isDone() )
			{
				return false;
			}
		}

		Scanner authorityScanner(authority);

		const StringView userInfo = authorityScanner.acceptUntil("@");

		if (!authorityScanner.accept('@').isEmpty() )
		{
			Scanner userInfoScanner(userInfo);

			m_tokens[UserName].set(userInfoScanner.acceptWhile(isNotColon) );

			if (!userInfoScanner.accept(':').isEmpty() )
			{
				m_tokens[Password].set(userInfoScanner.acceptAll() );
			}
		}

		m_tokens[Host].set(authorityScanner.acceptWhile(isNotColon) );

		if (!authorityScanner.accept(':').isEmpty() )
		{
			m_tokens[Port].set(authorityScanner.acceptAll() );
		}

		return true;
	}

	const StringView& UrlView::get(Enum _token) const
	{
		return m_tokens[_token];
	}

	static char toHex(uint8_t _nible)
	{
		return "0123456789ABCDEF"[_nible&0xf];
	}

	// https://secure.wikimedia.org/wikipedia/en/wiki/URL_encoding
	int32_t urlEncode(char* _out, int32_t _max, const StringView& _str, UrlEncoding::Enum _encoding)
	{
		if (0 >= _max)
		{
			return 0;
		}

		const char* str = _str.getPtr();

		const int32_t max = _max-1; // need space for zero terminator
		int32_t len = 0;

		for (int32_t ii = 0, num = _str.getLength(); ii < num; ++ii)
		{
			const char ch = str[ii];

			if (isAlphaNum(ch)
			||  '-' == ch
			||  '_' == ch
			||  '.' == ch
			||  '~' == ch
			|| ('/' == ch && UrlEncoding::Path == _encoding) )
			{
				if (max < len+1)
				{
					break;
				}

				_out[len++] = ch;
			}
			else
			{
				// Escape sequence must not be split by truncation.
				if (max < len+3)
				{
					break;
				}

				_out[len++] = '%';
				_out[len++] = toHex(uint8_t(ch)>>4);
				_out[len++] = toHex(uint8_t(ch) );
			}
		}

		_out[len] = '\0';

		return len;
	}

} // namespace bx
