/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_STRING_H_HEADER_GUARD
#	error "Must be included from bx/string.h!"
#endif // BX_STRING_H_HEADER_GUARD

#if BX_CRT_MSVC && !defined(va_copy)
#	define va_copy(_a, _b) (_a) = (_b)
#endif // BX_CRT_MSVC && !defined(va_copy)

namespace bx
{
	template <typename Ty>
	inline void stringPrintfVargs(Ty& _out, const char* _format, va_list _argList)
	{
		char temp[2048];

		char* out = temp;
		int32_t len = vsnprintf(out, sizeof(temp), _format, _argList);
		if (int32_t(sizeof(temp) ) < len)
		{
			out = (char*)BX_STACK_ALLOC(len);
			len = vsnprintf(out, len, _format, _argList);
		}
		_out.append(out, out+len);
	}

	template <typename Ty>
	inline void stringPrintf(Ty& _out, const char* _format, ...)
	{
		va_list argList;
		va_start(argList, _format);
		stringPrintfVargs(_out, _format, argList);
		va_end(argList);
	}

	inline constexpr StringLiteral::StringLiteral()
		: m_ptr("")
		, m_len(0)
	{
	}

	template<int32_t SizeT>
	inline constexpr StringLiteral::StringLiteral(const char (&_str)[SizeT])
		: m_ptr(_str)
		, m_len(SizeT - 1)
	{
		BX_ASSERT('\0' == m_ptr[SizeT - 1], "Must be 0 terminated.");
	}

	inline constexpr int32_t StringLiteral::getLength() const
	{
		return m_len;
	}

	inline constexpr const char* StringLiteral::getCPtr() const
	{
		return m_ptr;
	}

	inline constexpr void StringLiteral::clear()
	{
		m_ptr = "";
		m_len = 0;
	}

	inline constexpr bool StringLiteral::isEmpty() const
	{
		return 0 == m_len;
	}

	inline constexpr StringView::StringView()
	{
		m_ptr = "";
		m_len = 0;
	}

	inline constexpr StringView::StringView(const StringLiteral& _str)
		: m_ptr(_str.getCPtr() )
		, m_len(_str.getLength() )
		, m_0terminated(true)
	{
	}

	inline constexpr StringView::StringView(const StringView& _rhs)
	{
		set(_rhs);
	}

	inline constexpr StringView::StringView(const StringView& _rhs, int32_t _start, int32_t _len)
	{
		set(_rhs, _start, _len);
	}

	inline constexpr StringView& StringView::operator=(const char* _rhs)
	{
		set(_rhs);
		return *this;
	}

	inline constexpr StringView& StringView::operator=(const StringView& _rhs)
	{
		set(_rhs);
		return *this;
	}

	inline constexpr StringView::StringView(const char* _ptr)
	{
		set(_ptr, INT32_MAX);
	}

	inline constexpr StringView::StringView(const char* _ptr, int32_t _len)
	{
		set(_ptr, _len);
	}

	inline constexpr StringView::StringView(const char* _ptr, const char* _term)
	{
		set(_ptr, _term);
	}

	inline constexpr void StringView::set(const char* _ptr)
	{
		set(_ptr, INT32_MAX);
	}

	inline constexpr void StringView::set(const char* _ptr, int32_t _len)
	{
		clear();

		if (NULL != _ptr)
		{
			m_len = INT32_MAX == _len ? strLen(_ptr) : _len;
			m_ptr = _ptr;
			m_0terminated = INT32_MAX == _len;
		}
	}

	inline constexpr void StringView::set(const char* _ptr, const char* _term)
	{
		set(_ptr, int32_t(_term-_ptr) );
	}

	inline constexpr void StringView::set(const StringView& _str)
	{
		set(_str, 0, INT32_MAX);
	}

	inline constexpr void StringView::set(const StringView& _str, int32_t _start, int32_t _len)
	{
		const int32_t start = min(_start, _str.m_len);
		const int32_t len   = clamp(_str.m_len - start, 0, min(_len, _str.m_len) );
		set(_str.m_ptr + start, len);
	}

	inline constexpr void StringView::clear()
	{
		m_ptr = "";
		m_len = 0;
		m_0terminated = true;
	}

	inline constexpr const char* StringView::getPtr() const
	{
		return m_ptr;
	}

	inline constexpr const char* StringView::getTerm() const
	{
		return m_ptr + m_len;
	}

	inline constexpr bool StringView::isEmpty() const
	{
		return 0 == m_len;
	}

	inline constexpr int32_t StringView::getLength() const
	{
		return m_len;
	}

	inline constexpr bool StringView::is0Terminated() const
	{
		return m_0terminated;
	}

	inline BX_CONSTEXPR_FUNC bool isEqual(const StringView& _lhs, const StringView& _rhs, bool _caseSensitive)
	{
		const int32_t len = _lhs.getLength();

		if (len != _rhs.getLength() )
		{
			return false;
		}

		if (0 == len)
		{
			return true;
		}

		const char* lhs = _lhs.getPtr();
		const char* rhs = _rhs.getPtr();

		if constexpr (!isConstantEvaluated() )
		{
			// note: comparison of addresses of literals has unspecified value
			if (lhs == rhs)
			{
				return true;
			}
		}

		if (_caseSensitive)
		{
			for (int32_t ii = 0; ii < len; ++ii)
			{
				if (lhs[ii] != rhs[ii])
				{
					return false;
				}
			}

			return true;
		}

		for (int32_t ii = 0; ii < len; ++ii)
		{
			const char lch = lhs[ii];
			const char rch = rhs[ii];
			const char ll  = 'A' <= lch && lch <= 'Z' ? char(lch + 0x20) : lch;
			const char rl  = 'A' <= rch && rch <= 'Z' ? char(rch + 0x20) : rch;

			if (ll != rl)
			{
				return false;
			}
		}

		return true;
	}

	inline constexpr bool operator==(const StringView& _lhs, const StringView& _rhs)
	{
		return isEqual(_lhs, _rhs);
	}

	inline constexpr bool operator!=(const StringView& _lhs, const StringView& _rhs)
	{
		return !(_lhs == _rhs);
	}

	inline constexpr bool overlap(const StringView& _a, const StringView& _b)
	{
		return _a.getTerm() > _b.getPtr()
			&& _b.getTerm() > _a.getPtr()
			;
	}

	inline constexpr bool contain(const StringView& _a, const StringView& _b)
	{
		return _a.getPtr()  <= _b.getPtr()
			&& _a.getTerm() >= _b.getTerm()
			;
	}

	template<uint16_t MaxCapacityT>
	inline constexpr FixedStringT<MaxCapacityT>::FixedStringT()
		: m_pod{ .storage = { '\0' }, .len = 0 }
	{
	}

	template<uint16_t MaxCapacityT>
	inline constexpr FixedStringT<MaxCapacityT>::FixedStringT(const char* _str)
		: FixedStringT<MaxCapacityT>()
	{
		set(_str);
	}

	template<uint16_t MaxCapacityT>
	inline constexpr FixedStringT<MaxCapacityT>::FixedStringT(const StringView& _str)
		: FixedStringT<MaxCapacityT>()
	{
		set(_str);
	}

	template<uint16_t MaxCapacityT>
	inline constexpr FixedStringT<MaxCapacityT>::~FixedStringT()
	{
	}

	template<uint16_t MaxCapacityT>
	inline constexpr void FixedStringT<MaxCapacityT>::set(const char* _str)
	{
		set(StringView(_str) );
	}

	template<uint16_t MaxCapacityT>
	inline constexpr void FixedStringT<MaxCapacityT>::set(const StringView& _str)
	{
		int32_t copied = strCopy(m_pod.storage, MaxCapacityT, _str);
		m_pod.len = copied;
	}

	template<uint16_t MaxCapacityT>
	inline constexpr void FixedStringT<MaxCapacityT>::append(const StringView& _str)
	{
		m_pod.len += strCopy(&m_pod.storage[m_pod.len], MaxCapacityT-m_pod.len, _str);
	}

	template<uint16_t MaxCapacityT>
	inline constexpr void FixedStringT<MaxCapacityT>::clear()
	{
		m_pod.len = 0;
		m_pod.storage[0] = '\0';
	}

	template<uint16_t MaxCapacityT>
	inline constexpr bool FixedStringT<MaxCapacityT>::isEmpty() const
	{
		return 0 == m_pod.len;
	}

	template<uint16_t MaxCapacityT>
	inline constexpr int32_t FixedStringT<MaxCapacityT>::getLength() const
	{
		return m_pod.len;
	}

	template<uint16_t MaxCapacityT>
	inline constexpr const char* FixedStringT<MaxCapacityT>::getCPtr() const
	{
		return m_pod.storage;
	}

	template<uint16_t MaxCapacityT>
	inline constexpr FixedStringT<MaxCapacityT>::operator StringView() const
	{
		return StringView(m_pod.storage, m_pod.len);
	}

	template<uint16_t MaxCapacityT>
	inline typename FixedStringT<MaxCapacityT>::Pod& FixedStringT<MaxCapacityT>::asPod()
	{
		return m_pod;
	}

	template<AllocatorI** AllocatorT>
	inline StringT<AllocatorT>::StringT()
		: m_ptr("")
		, m_len(0)
		, m_capacity(0)
	{
	}

	template<AllocatorI** AllocatorT>
	inline StringT<AllocatorT>::StringT(const StringT<AllocatorT>& _rhs)
		: m_ptr("")
		, m_len(0)
		, m_capacity(0)
	{
		set(_rhs);
	}

	template<AllocatorI** AllocatorT>
	inline StringT<AllocatorT>::StringT(const StringView& _rhs)
		: m_ptr("")
		, m_len(0)
		, m_capacity(0)
	{
		set(_rhs);
	}

	template<AllocatorI** AllocatorT>
	inline StringT<AllocatorT>::~StringT()
	{
		clear();
	}

	template<AllocatorI** AllocatorT>
	inline StringT<AllocatorT>& StringT<AllocatorT>::operator=(const StringT<AllocatorT>& _rhs)
	{
		set(_rhs);
		return *this;
	}

	template<AllocatorI** AllocatorT>
	inline void StringT<AllocatorT>::set(const StringView& _str)
	{
		clear();
		append(_str);
	}

	template<AllocatorI** AllocatorT>
	inline void StringT<AllocatorT>::append(const StringView& _str)
	{
		if (0 != _str.getLength() )
		{
			const int32_t old = m_len;
			const int32_t len = m_len + _str.getLength();

			char* ptr = const_cast<char*>(m_ptr);

			if (len+1 > m_capacity)
			{
				const int32_t capacity = alignUp(len+1, 256);
				ptr = (char*)realloc(*AllocatorT, 0 != m_capacity ? ptr : NULL, capacity);

				*const_cast<char**>(&m_ptr) = ptr;
				m_capacity = capacity;
			}

			m_len = len;
			strCopy(ptr + old, len-old+1, _str);
		}
	}

	template<AllocatorI** AllocatorT>
	inline void StringT<AllocatorT>::append(const char* _ptr, const char* _term)
	{
		append(StringView(_ptr, _term) );
	}

	template<AllocatorI** AllocatorT>
	inline void StringT<AllocatorT>::clear()
	{
		if (0 != m_capacity)
		{
			free(*AllocatorT, const_cast<char*>(m_ptr) );

			m_ptr = "";
			m_len = 0;
			m_capacity = 0;
		}
	}

	template<AllocatorI** AllocatorT>
	inline bool StringT<AllocatorT>::isEmpty() const
	{
		return 0 == m_len;
	}

	template<AllocatorI** AllocatorT>
	inline int32_t StringT<AllocatorT>::getLength() const
	{
		return m_len;
	}

	template<AllocatorI** AllocatorT>
	inline const char* StringT<AllocatorT>::getCPtr() const
	{
		return m_ptr;
	}

	template<AllocatorI** AllocatorT>
	inline StringT<AllocatorT>::operator StringView() const
	{
		return StringView(m_ptr, m_len);
	}

	inline BX_CONSTEXPR_FUNC bool isInRange(char _ch, char _from, char _to)
	{
		return unsigned(_ch - _from) <= unsigned(_to-_from);
	}

	inline BX_CONSTEXPR_FUNC bool isSpace(char _ch)
	{
		return ' '  == _ch // Space.
			|| '\t' == _ch // Horizontal tab.
			|| '\n' == _ch // Line feed / new line.
			|| '\r' == _ch // Carriage return.
			|| '\v' == _ch // Vertical tab.
			|| '\f' == _ch // Form feed / new page.
			;
	}

	inline BX_CONSTEXPR_FUNC bool isSpaceHoriz(char _ch)
	{
		return false
			|| ' ' == _ch
			|| '\t' == _ch
			|| '\r' == _ch
			;
	}

	inline BX_CONSTEXPR_FUNC bool isUpper(char _ch)
	{
		return isInRange(_ch, 'A', 'Z');
	}

	inline BX_CONSTEXPR_FUNC char toUpper(char _ch)
	{
		return _ch - (isLower(_ch) ? 0x20 : 0);
	}

	inline BX_CONSTEXPR_FUNC bool isLower(char _ch)
	{
		return isInRange(_ch, 'a', 'z');
	}

	inline BX_CONSTEXPR_FUNC char toLower(char _ch)
	{
		return _ch + (isUpper(_ch) ? 0x20 : 0);
	}

	inline BX_CONSTEXPR_FUNC bool isAlpha(char _ch)
	{
		return isLower(_ch) || isUpper(_ch);
	}

	inline BX_CONSTEXPR_FUNC bool isNumeric(char _ch)
	{
		return isInRange(_ch, '0', '9');
	}

	inline BX_CONSTEXPR_FUNC bool isAlphaNum(char _ch)
	{
		return false
			|| isAlpha(_ch)
			|| isNumeric(_ch)
			;
	}

	inline BX_CONSTEXPR_FUNC bool isHexNum(char _ch)
	{
		return false
			|| isInRange(toLower(_ch), 'a', 'f')
			|| isNumeric(_ch)
			;
	}

	inline BX_CONSTEXPR_FUNC bool isOctNum(char _ch)
	{
		return isInRange(_ch, '0', '7');
	}

	inline BX_CONSTEXPR_FUNC bool isBinNum(char _ch)
	{
		return isInRange(_ch, '0', '1');
	}

	inline BX_CONSTEXPR_FUNC bool isPrint(char _ch)
	{
		return isInRange(_ch, ' ', '~');
	}

	inline BX_CONSTEXPR_FUNC bool isIdentStart(char _ch)
	{
		return false
			|| isAlpha(_ch)
			|| '_' == _ch
			;
	}

	inline BX_CONSTEXPR_FUNC bool isIdentChar(char _ch)
	{
		return false
			|| isAlphaNum(_ch)
			|| '_' == _ch
			;
	}

	inline BX_CONSTEXPR_FUNC bool isPathSeparator(char _ch)
	{
		return false
			|| '/'  == _ch
			|| '\\' == _ch
			;
	}

	inline StringView strSubstr(const StringView& _str, int32_t _start, int32_t _len)
	{
		return StringView(_str, _start, _len);
	}

	inline constexpr int32_t strLen(const StringView& _str, int32_t _max)
	{
		return min(_str.getLength(), _max);
	}

	inline constexpr int32_t strLen(const char* _str, int32_t _max)
	{
		if (NULL == _str)
		{
			return 0;
		}

		const char* ptr = _str;
		for (; 0 < _max && *ptr != '\0'; ++ptr, --_max) {};
		return int32_t(ptr - _str);
	}

	inline bool hasPrefix(const StringView& _str, const StringView& _prefix)
	{
		const int32_t len = _prefix.getLength();
		return _str.getLength() >= len
			&& 0 == strCmp(_str, _prefix, len)
			;
	}

	inline bool hasSuffix(const StringView& _str, const StringView& _suffix)
	{
		const int32_t len = _suffix.getLength();
		return _str.getLength() >= len
			&& 0 == strCmp(StringView(_str.getTerm() - len, _str.getTerm() ), _suffix, len)
			;
	}

	inline StringView strTrimPrefix(const StringView& _str, const StringView& _prefix)
	{
		if (hasPrefix(_str, _prefix) )
		{
			return StringView(_str.getPtr() + _prefix.getLength(), _str.getTerm() );
		}

		return _str;
	}

	inline StringView strTrimSuffix(const StringView& _str, const StringView& _suffix)
	{
		if (hasSuffix(_str, _suffix) )
		{
			return StringView(_str.getPtr(), _str.getTerm() - _suffix.getLength() );
		}

		return _str;
	}

	inline bool fromString(int8_t* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	inline bool fromString(uint8_t* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	inline bool fromString(int16_t* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	inline bool fromString(uint16_t* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	inline bool fromString(int32_t* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	inline bool fromString(uint32_t* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	inline bool fromString(long* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	inline bool fromString(unsigned long* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	inline bool fromString(unsigned long long* _out, const StringView& _str)
	{
		long long tmp;
		fromString(&tmp, _str);

		return narrowCastTest(_out, tmp);
	}

	template<uint16_t MaxCapacityT>
	inline FixedStringT<MaxCapacityT> toHuman(uint64_t _value)
	{
		FixedStringT<MaxCapacityT> result;
		auto& pod = result.asPod();

		pod.len = formatHumanNumber(pod.storage, MaxCapacityT, double(_value), 0);

		return result;
	};

	template<uint16_t MaxCapacityT>
	inline FixedStringT<MaxCapacityT> toHuman(uint64_t _value, Units::Enum _units, uint8_t _numFrac)
	{
		FixedStringT<MaxCapacityT> result;
		auto& pod = result.asPod();

		switch (_units)
		{
		case Units::Kilo:
			pod.len = formatHumanNumber(pod.storage, MaxCapacityT, double(_value), _numFrac, 1000.0, "", " KMGTPEZY");
			break;

		case Units::KiloByte:
			pod.len = formatHumanNumber(pod.storage, MaxCapacityT, double(_value), _numFrac, 1000.0, "B", "BkMGTPEZY");
			break;

		case Units::KibiByte:
		default:
			pod.len = formatHumanNumber(pod.storage, MaxCapacityT, double(_value), _numFrac, 1024.0, "iB", "BKMGTPEZY");
			break;
		}

		return result;
	};

	template<uint16_t MaxCapacityT>
	inline FixedStringT<MaxCapacityT> toHuman(Ticks _value, uint8_t _numFrac)
	{
		FixedStringT<MaxCapacityT> result;
		auto& pod = result.asPod();

		const double value = toSeconds<double>(_value);
		pod.len = formatHumanNumber(pod.storage, MaxCapacityT, value, _numFrac, 1000.0, "s", "pnum ", 4);

		return result;
	};

} // namespace bx
