/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_H_HEADER_GUARD
#	error "Must be included from bx/bx.h!"
#endif // BX_H_HEADER_GUARD

namespace bx
{
	// Reference(S):
	// - https://web.archive.org/web/20181115035420/http://cnicholson.net/2011/01/stupid-c-tricks-a-better-sizeof_array/
	//
	template<typename Ty, size_t NumT>
	char (&CountOfRequireArrayArgumentT(const Ty (&)[NumT]) )[NumT];

	template<bool B>
	struct isEnabled
	{
		// Template for avoiding MSVC: C4127: conditional expression is constant
		static constexpr bool value = B;
	};

	inline constexpr bool ignoreC4127(bool _x)
	{
		return _x;
	}

	template<typename Ty>
	inline Ty* addressOf(Ty& _a)
	{
		return reinterpret_cast<Ty*>(
				&const_cast<char&>(
					reinterpret_cast<const volatile char&>(_a)
				)
			);
	}

	template<typename Ty>
	inline const Ty* addressOf(const Ty& _a)
	{
		return reinterpret_cast<const Ty*>(
				&const_cast<char&>(
					reinterpret_cast<const volatile char&>(_a)
				)
			);
	}

	template<typename Ty>
	inline Ty* addressOf(void* _ptr, ptrdiff_t _offsetInBytes)
	{
		return (Ty*)( (uint8_t*)_ptr + _offsetInBytes);
	}

	template<typename Ty>
	inline const Ty* addressOf(const void* _ptr, ptrdiff_t _offsetInBytes)
	{
		return (const Ty*)( (const uint8_t*)_ptr + _offsetInBytes);
	}

	template<typename Ty>
	inline Ty loadAligned(const void* _ptr)
	{
		static_assert(isTriviallyCopyable<Ty>(), "Ty must be trivially copyable type.");

		return *(const Ty*)_ptr;
	}

	template<typename Ty>
	inline Ty loadUnaligned(const void* _ptr)
	{
		static_assert(isTriviallyCopyable<Ty>(), "Ty must be trivially copyable type.");

#if BX_COMPILER_GCC || BX_COMPILER_CLANG
		typedef Ty BX_ATTRIBUTE(aligned(1) ) UnalignedTy;
		return *(UnalignedTy*)_ptr;
#else
		Ty value;
		memCopy(&value, _ptr, sizeof(Ty) );

		return value;
#endif // BX_COMPILER_*
	}

	template<>
	inline uint32_t loadUnaligned(const void* _ptr)
	{
		const uint8_t* data = (const uint8_t*)_ptr;

		return 0
			| uint32_t(data[3])<<24
			| uint32_t(data[2])<<16
			| uint32_t(data[1])<<8
			| uint32_t(data[0])
			;
	}

	template<>
	inline uint64_t loadUnaligned(const void* _ptr)
	{
		const uint8_t* data = (const uint8_t*)_ptr;

		return 0
			| uint64_t(data[7])<<56
			| uint64_t(data[6])<<48
			| uint64_t(data[5])<<40
			| uint64_t(data[4])<<32
			| uint64_t(data[3])<<24
			| uint64_t(data[2])<<16
			| uint64_t(data[1])<<8
			| uint64_t(data[0])
			;
	}

	template<typename Ty>
	inline void storeAligned(void* _ptr, const Ty& _value)
	{
		static_assert(isTriviallyCopyable<Ty>(), "Ty must be trivially copyable type.");

		*(Ty*)_ptr = _value;
	}

	template<typename Ty>
	inline void storeUnaligned(void* _ptr, const Ty& _value)
	{
		static_assert(isTriviallyCopyable<Ty>(), "Ty must be trivially copyable type.");

#if BX_COMPILER_GCC || BX_COMPILER_CLANG
		typedef Ty BX_ATTRIBUTE(aligned(1) ) UnalignedTy;
		*(UnalignedTy*)_ptr = _value;
#else
		memCopy(_ptr, &_value, sizeof(Ty) );
#endif // BX_COMPILER_*
	}

	template<typename Ty>
	inline void swap(Ty& _a, Ty& _b)
	{
		Ty tmp = move(_a); _a = move(_b); _b = move(tmp);
	}

	template<typename Ty>
	struct LimitsT<Ty, true>
	{
		static constexpr Ty max = ( ( (Ty(1) << ( (sizeof(Ty) * 8) - 2) ) - Ty(1) ) << 1) | Ty(1);
		static constexpr Ty min = -max - Ty(1);
	};

	template<typename Ty>
	struct LimitsT<Ty, false>
	{
		static constexpr Ty min = 0;
		static constexpr Ty max = Ty(-1);
	};

	template<>
	struct LimitsT<float, true>
	{
		static constexpr float min = -kFloatLargest;
		static constexpr float max =  kFloatLargest;
	};

	template<>
	struct LimitsT<double, true>
	{
		static constexpr double min = -kDoubleLargest;
		static constexpr double max =  kDoubleLargest;
	};

	template<typename Ty>
	inline constexpr Ty max()
	{
		return LimitsT<Ty>::max;
	}

	template<typename Ty>
	inline constexpr Ty min()
	{
		return LimitsT<Ty>::min;
	}

	template<typename Ty>
	inline constexpr Ty min(const Ty& _a, const TypeIdentityType<Ty>& _b)
	{
		return _a < _b ? _a : _b;
	}

	template<typename Ty>
	inline constexpr Ty max(const Ty& _a, const TypeIdentityType<Ty>& _b)
	{
		return _a > _b ? _a : _b;
	}

	template<typename Ty, typename... Args>
	inline constexpr Ty min(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args)
	{
		return min(min(_a, _b), _args...);
	}

	template<typename Ty, typename... Args>
	inline constexpr Ty max(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args)
	{
		return max(max(_a, _b), _args...);
	}

	template<typename Ty, typename... Args>
	inline constexpr Ty mid(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args)
	{
		return max(min(_a, _b), min(max(_a, _b), _args...) );
	}

	template<typename Ty>
	inline constexpr Ty clamp(const Ty& _a, const TypeIdentityType<Ty>& _min, const TypeIdentityType<Ty>& _max)
	{
		return max(min(_a, _max), _min);
	}

	template<typename Ty>
	inline constexpr bool isPowerOf2(Ty _a)
	{
		return _a && !(_a & (_a - 1) );
	}

	constexpr bool isConstantEvaluated()
	{
		return __builtin_is_constant_evaluated();
	}

	BX_PRAGMA_DIAGNOSTIC_PUSH();
	BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wpsabi");

	template <typename Ty, typename FromT>
	inline constexpr Ty bitCast(const FromT& _from)
	{
		static_assert(sizeof(Ty) == sizeof(FromT)
			, "bx::bitCast failed! Ty and FromT must be the same size."
			);
		static_assert(isTriviallyCopyable<FromT>()
			, "bx::bitCast failed! FromT must be trivially copyable."
			);
		static_assert(isTriviallyCopyable<Ty>()
			, "bx::bitCast failed! Ty must be trivially copyable."
			);
		static_assert(isTriviallyConstructible<Ty>()
			, "bx::bitCast failed! Ty must be trivially constructible."
			);

		return __builtin_bit_cast(Ty, _from);
	}

	BX_PRAGMA_DIAGNOSTIC_POP();

	template<typename Ty, typename FromT>
	requires (isInteger<   Ty>() || isFloatingPoint<   Ty>() )
		  && (isInteger<FromT>() || isFloatingPoint<FromT>() )
	inline constexpr Ty saturateCast(FromT _from)
	{
		if constexpr (isSame<RemoveCvType<Ty>, RemoveCvType<FromT> >() )
		{
			return _from;
		}

		constexpr Ty mx = max<Ty>();

		if constexpr (isSigned<Ty>() && isSigned<FromT>() )
		{
			if constexpr (sizeof(Ty) < sizeof(FromT) )
			{
				constexpr FromT mn = min<Ty>();

				if (_from < mn)
				{
					return mn;
				}
				else if (_from > mx)
				{
					return mx;
				}
			}
		}
		else if constexpr (isSigned<FromT>() )
		{
			if (_from < FromT(0) )
			{
				return Ty(0);
			}
			else if (asUnsigned<FromT>(_from) > mx)
			{
				return mx;
			}
		}
		else if (_from > asUnsigned<Ty>(max<Ty>() ) )
		{
			return mx;
		}

		return static_cast<Ty>(_from);
	}

	template<typename Ty, typename FromT>
	inline constexpr bool narrowCastTest(Ty* _out, const FromT& _from)
	{
		if constexpr (isSame<Ty, FromT>() )
		{
			*_out = _from;
			return true;
		}

BX_PRAGMA_DIAGNOSTIC_PUSH()
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4702) // warning C4702: unreachable code
		*_out = static_cast<Ty>(_from);
		return static_cast<FromT>(*_out) == _from;
BX_PRAGMA_DIAGNOSTIC_POP()
	}

	template<typename Ty, typename FromT>
	inline Ty narrowCast(const FromT& _from, Location _location)
	{
		Ty to;
		const bool result = narrowCastTest(&to, _from);

		BX_ASSERT_LOC(_location, result
			, "bx::narrowCast failed! Value is truncated!"
			);
		BX_UNUSED(result);
		return to;
	}

	constexpr float  kFloatInfinity  = bitCast<float>(kFloatExponentMask);
	constexpr double kDoubleInfinity = bitCast<double>(kDoubleExponentMask);

	inline BX_CONSTEXPR_FUNC uint32_t gcd(uint32_t _a, uint32_t _b)
	{
		do
		{
			const uint32_t tmp = _a % _b;
			_a = _b;
			_b = tmp;
		}
		while (_b);

		return _a;
	}

	inline BX_CONSTEXPR_FUNC uint32_t lcm(uint32_t _a, uint32_t _b)
	{
		return _a * (_b / gcd(_a, _b) );
	}

	inline BX_CONSTEXPR_FUNC uint32_t strideAlign(uint32_t _offset, uint32_t _stride)
	{
		const uint32_t mod    = _offset % _stride;
		const uint32_t add    = _stride - mod;
		const uint32_t tmp    = (0 == mod) ? 0 : add;
		const uint32_t result = _offset + tmp;

		return result;
	}

	template<uint32_t Min>
	inline BX_CONSTEXPR_FUNC uint32_t strideAlign(uint32_t _offset, uint32_t _stride)
	{
		const uint32_t align  = lcm(Min, _stride);
		const uint32_t mod    = _offset % align;
		const uint32_t tmp0   = (0 == mod) ? 0 : align;
		const uint32_t tmp1   = _offset + tmp0;
		const uint32_t result = tmp1 - mod;

		return result;
	}

	template<typename Ty>
	inline BX_CONSTEXPR_FUNC bool isAligned(Ty _a, size_t _align)
	{
		const size_t mask = max<size_t>(1, _align) - 1;
		return 0 == (size_t(_a) & mask);
	}

	template<>
	inline BX_CONSTEXPR_FUNC bool isAligned(const void* _ptr, size_t _align)
	{
		const uintptr_t addr = bitCast<uintptr_t>(_ptr);
		return isAligned(addr, _align);
	}

	template<typename Ty>
	inline BX_CONSTEXPR_FUNC Ty alignDown(Ty _a, size_t _align)
	{
		const size_t mask = max<size_t>(1, _align) - 1;
		return Ty(size_t(_a) & ~mask);
	}

	template<typename Ty>
	inline BX_CONSTEXPR_FUNC Ty* alignDown(Ty* _ptr, size_t _align)
	{
		uintptr_t addr = bitCast<uintptr_t>(_ptr);
		addr = alignDown(addr, _align);
		return bitCast<Ty*>(addr);
	}

	template<typename Ty>
	inline BX_CONSTEXPR_FUNC const Ty* alignDown(const Ty* _ptr, size_t _align)
	{
		uintptr_t addr = bitCast<uintptr_t>(_ptr);
		addr = alignDown(addr, _align);
		return bitCast<const Ty*>(addr);
	}

	template<typename Ty>
	inline BX_CONSTEXPR_FUNC Ty alignUp(Ty _a, size_t _align)
	{
		const size_t mask = max<size_t>(1, _align) - 1;
		return Ty( (size_t(_a) + mask) & ~mask);
	}

	template<typename Ty>
	inline BX_CONSTEXPR_FUNC Ty* alignUp(Ty* _ptr, size_t _align)
	{
		uintptr_t addr = bitCast<uintptr_t>(_ptr);
		addr = alignUp(addr, _align);
		return bitCast<Ty*>(addr);
	}

	template<typename Ty>
	inline BX_CONSTEXPR_FUNC const Ty* alignUp(const Ty* _ptr, size_t _align)
	{
		uintptr_t addr = bitCast<uintptr_t>(_ptr);
		addr = alignUp(addr, _align);
		return bitCast<const Ty*>(addr);
	}

} // namespace bx
