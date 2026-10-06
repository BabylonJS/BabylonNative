/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{
	BX_ALIGN_DECL(4, struct)  simd32_f32_ref_t { float   f32; };
	BX_ALIGN_DECL(4, struct)  simd32_i32_ref_t { int32_t i32; };

	BX_SIMD_FORCE_INLINE simd32_t simd32_ld(const void* _ptr)
	{
		simd32_t result;
		memCopy(&result, _ptr, sizeof(simd32_t) );
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_ld(float _x)
	{
		const simd32_f32_ref_t result = { .f32 = _x };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_ld(int32_t _x)
	{
		return { .u32 = uint32_t(_x) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_ld(uint32_t _x)
	{
		return { .u32 = _x };
	}

	BX_SIMD_FORCE_INLINE void simd32_st(void* _ptr, simd32_t _a)
	{
		memCopy(_ptr, &_a, sizeof(simd32_t) );
	}

	BX_SIMD_FORCE_INLINE void simd32_x32_st1(void* _ptr, simd32_t _a)
	{
		simd32_t* result = reinterpret_cast<simd32_t*>(_ptr);
		*result = _a;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_zero()
	{
		return { .u32 = 0 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_splat(float _a)
	{
		const simd32_f32_ref_t result = { .f32 = _a };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_splat(uint32_t _a)
	{
		return { .u32 = _a };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_splat(int32_t _a)
	{
		return { .u32 = uint32_t(_a) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_splat(uint16_t _a)
	{
		const uint32_t val = uint32_t(_a);
		return { .u32 = (val << 16) | val };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_splat(int16_t _a)
	{
		return simd32_splat(uint16_t(_a) );
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_splat(uint8_t _a)
	{
		const uint32_t val = uint32_t(_a);
		return { .u32 = (val << 24) | (val << 16) | (val << 8) | val };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_splat(int8_t _a)
	{
		return simd32_splat(uint8_t(_a) );
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_add(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t result = { .f32 = a.f32 + b.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_sub(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t result = { .f32 = a.f32 - b.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_mul(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t result = { .f32 = a.f32 * b.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_div(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t result = { .f32 = a.f32 / b.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_madd(simd32_t _a, simd32_t _b, simd32_t _c)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t c = bitCast<simd32_f32_ref_t>(_c);
		const simd32_f32_ref_t result = { .f32 = a.f32 * b.f32 + c.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_msub(simd32_t _a, simd32_t _b, simd32_t _c)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t c = bitCast<simd32_f32_ref_t>(_c);
		const simd32_f32_ref_t result = { .f32 = a.f32 * b.f32 - c.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_nmsub(simd32_t _a, simd32_t _b, simd32_t _c)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t c = bitCast<simd32_f32_ref_t>(_c);
		const simd32_f32_ref_t result = { .f32 = c.f32 - a.f32 * b.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC int simd32_x32_signbitsmask(simd32_t _a)
	{
		return int(_a.u32 >> 31);
	}

	inline BX_CONSTEXPR_FUNC int simd32_x8_signbitsmask(simd32_t _a)
	{
		return  int( (_a.u32 >>  7) & 1)
		     | (int( (_a.u32 >> 15) & 1) << 1)
		     | (int( (_a.u32 >> 23) & 1) << 2)
		     | (int(  _a.u32 >> 31)      << 3)
		     ;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x8_shuffle(simd32_t _a, simd32_t _indices)
	{
		uint32_t out = 0;
		for (uint32_t ii = 0; ii < 4; ++ii)
		{
			const uint32_t idx = (_indices.u32 >> (ii*8) ) & 0xffu;
			const uint32_t byte = (idx & 0x80u)
				? 0u
				: (_a.u32 >> ( (idx & 0x03u) * 8) ) & 0xffu;
			out |= byte << (ii*8);
		}
		return { .u32 = out };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x8_shuffle(simd32_t _a, simd32_t _b, simd32_t _indices)
	{
		uint32_t out = 0;
		for (uint32_t ii = 0; ii < 4; ++ii)
		{
			const uint32_t idx = (_indices.u32 >> (ii*8) ) & 0xffu;
			uint32_t byte = 0;
			if (0 == (idx & 0x80u) )
			{
				const uint32_t sel = idx & 0x07u;
				const uint32_t src = sel < 4u ? _a.u32 : _b.u32;
				byte = (src >> ( (sel & 0x03u) * 8) ) & 0xffu;
			}
			out |= byte << (ii*8);
		}
		return { .u32 = out };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_neg(simd32_t _a)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t result = { .f32 = -a.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_abs(simd32_t _a)
	{
		return { .u32 = _a.u32 & (kFloatExponentMask | kFloatMantissaMask) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_min(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t result = { .f32 = a.f32 < b.f32 ? a.f32 : b.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_max(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t result = { .f32 = a.f32 > b.f32 ? a.f32 : b.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_clamp(simd32_t _a, simd32_t _min, simd32_t _max)
	{
		return simd32_f32_min(simd32_f32_max(_a, _min), _max);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_lerp(simd32_t _a, simd32_t _b, simd32_t _s)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		const simd32_f32_ref_t s = bitCast<simd32_f32_ref_t>(_s);
		const simd32_f32_ref_t result = { .f32 = a.f32 + (b.f32 - a.f32) * s.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_rcp(simd32_t _a)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t result = { .f32 = 1.0f / a.f32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_sqrt(simd32_t _a)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t result = { .f32 = sqrt(a.f32) };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_rsqrt(simd32_t _a)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t result = { .f32 = 1.0f / sqrt(a.f32) };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_round(simd32_t _a)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t result = { .f32 = round(a.f32) };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_ceil(simd32_t _a)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t result = { .f32 = ceil(a.f32) };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_floor(simd32_t _a)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t result = { .f32 = floor(a.f32) };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmpeq(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		return { .u32 = a.f32 == b.f32 ? UINT32_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmpneq(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		return { .u32 = a.f32 != b.f32 ? UINT32_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmplt(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		return { .u32 = a.f32 < b.f32 ? UINT32_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmple(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		return { .u32 = a.f32 <= b.f32 ? UINT32_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmpgt(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		return { .u32 = a.f32 > b.f32 ? UINT32_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmpge(simd32_t _a, simd32_t _b)
	{
		const simd32_f32_ref_t a = bitCast<simd32_f32_ref_t>(_a);
		const simd32_f32_ref_t b = bitCast<simd32_f32_ref_t>(_b);
		return { .u32 = a.f32 >= b.f32 ? UINT32_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_add(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 + _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_sub(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 - _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_mul(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 * _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_div(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 / _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_mod(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 % _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_min(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 < _b.u32 ? _a.u32 : _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_max(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 > _b.u32 ? _a.u32 : _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_clamp(simd32_t _a, simd32_t _min, simd32_t _max)
	{
		return simd32_u32_min(simd32_u32_max(_a, _min), _max);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_add(simd32_t _a, simd32_t _b)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t b = bitCast<simd32_i32_ref_t>(_b);
		const simd32_i32_ref_t result = { .i32 = a.i32 + b.i32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_sub(simd32_t _a, simd32_t _b)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t b = bitCast<simd32_i32_ref_t>(_b);
		const simd32_i32_ref_t result = { .i32 = a.i32 - b.i32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_neg(simd32_t _a)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t result = { .i32 = -a.i32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_abs(simd32_t _a)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t result = { .i32 = a.i32 < 0 ? -a.i32 : a.i32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_min(simd32_t _a, simd32_t _b)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t b = bitCast<simd32_i32_ref_t>(_b);
		const simd32_i32_ref_t result = { .i32 = a.i32 < b.i32 ? a.i32 : b.i32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_max(simd32_t _a, simd32_t _b)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t b = bitCast<simd32_i32_ref_t>(_b);
		const simd32_i32_ref_t result = { .i32 = a.i32 > b.i32 ? a.i32 : b.i32 };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_clamp(simd32_t _a, simd32_t _min, simd32_t _max)
	{
		return simd32_i32_min(simd32_i32_max(_a, _min), _max);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmpeq(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(-(_a.u32 == _b.u32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmpneq(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(-(_a.u32 != _b.u32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmplt(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(-(_a.u32 < _b.u32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmple(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(-(_a.u32 <= _b.u32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmpgt(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(-(_a.u32 > _b.u32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmpge(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(-(_a.u32 >= _b.u32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_cmpeq(simd32_t _a, simd32_t _b)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t b = bitCast<simd32_i32_ref_t>(_b);
		return { .u32 = uint32_t(-(a.i32 == b.i32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_cmplt(simd32_t _a, simd32_t _b)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t b = bitCast<simd32_i32_ref_t>(_b);
		return { .u32 = uint32_t(-(a.i32 < b.i32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_cmpgt(simd32_t _a, simd32_t _b)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t b = bitCast<simd32_i32_ref_t>(_b);
		return { .u32 = uint32_t(-(a.i32 > b.i32)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i16_add(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(uint16_t(int16_t(_a.u32) + int16_t(_b.u32)))
		             | (uint32_t(uint16_t(int16_t(_a.u32 >> 16) + int16_t(_b.u32 >> 16))) << 16) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i16_sub(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(uint16_t(int16_t(_a.u32) - int16_t(_b.u32)))
		             | (uint32_t(uint16_t(int16_t(_a.u32 >> 16) - int16_t(_b.u32 >> 16))) << 16) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i16_cmpeq(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(int16_t(_a.u32) == int16_t(_b.u32) ? uint16_t(0xffff) : uint16_t(0))
		             | (uint32_t(int16_t(_a.u32 >> 16) == int16_t(_b.u32 >> 16) ? uint16_t(0xffff) : uint16_t(0)) << 16) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x16_sll(simd32_t _a, int _count)
	{
		return { .u32 = uint32_t(uint16_t(uint16_t(_a.u32) << _count))
		             | (uint32_t(uint16_t(uint16_t(_a.u32 >> 16) << _count)) << 16) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x16_srl(simd32_t _a, int _count)
	{
		return { .u32 = uint32_t(uint16_t(uint16_t(_a.u32) >> _count))
		             | (uint32_t(uint16_t(uint16_t(_a.u32 >> 16) >> _count)) << 16) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i8_add(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(uint8_t(int8_t(_a.u32      ) + int8_t(_b.u32      ) ) )
		             | (uint32_t(uint8_t(int8_t(_a.u32 >>  8) + int8_t(_b.u32 >>  8) ) ) <<  8)
		             | (uint32_t(uint8_t(int8_t(_a.u32 >> 16) + int8_t(_b.u32 >> 16) ) ) << 16)
		             | (uint32_t(uint8_t(int8_t(_a.u32 >> 24) + int8_t(_b.u32 >> 24) ) ) << 24) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i8_sub(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(uint8_t(int8_t(_a.u32      ) - int8_t(_b.u32      ) ) )
		             | (uint32_t(uint8_t(int8_t(_a.u32 >>  8) - int8_t(_b.u32 >>  8) ) ) <<  8)
		             | (uint32_t(uint8_t(int8_t(_a.u32 >> 16) - int8_t(_b.u32 >> 16) ) ) << 16)
		             | (uint32_t(uint8_t(int8_t(_a.u32 >> 24) - int8_t(_b.u32 >> 24) ) ) << 24) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u8_satadd(simd32_t _a, simd32_t _b)
	{
		simd32_t result{ .u32 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const uint16_t sum = uint16_t(uint8_t(_a.u32 >> (ii * 8))) + uint8_t(_b.u32 >> (ii * 8));
			result.u32 |= uint32_t(sum > 255 ? 255 : uint8_t(sum)) << (ii * 8);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u8_satsub(simd32_t _a, simd32_t _b)
	{
		simd32_t result{ .u32 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const uint8_t a = uint8_t(_a.u32 >> (ii * 8));
			const uint8_t b = uint8_t(_b.u32 >> (ii * 8));
			result.u32 |= uint32_t(a > b ? uint8_t(a - b) : uint8_t(0)) << (ii * 8);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u16_satadd(simd32_t _a, simd32_t _b)
	{
		simd32_t result{ .u32 = 0 };
		for (int ii = 0; ii < 2; ++ii)
		{
			const uint32_t sum = uint32_t(uint16_t(_a.u32 >> (ii * 16))) + uint16_t(_b.u32 >> (ii * 16));
			result.u32 |= uint32_t(sum > 65535 ? 65535 : uint16_t(sum)) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u16_satsub(simd32_t _a, simd32_t _b)
	{
		simd32_t result{ .u32 = 0 };
		for (int ii = 0; ii < 2; ++ii)
		{
			const uint16_t a = uint16_t(_a.u32 >> (ii * 16));
			const uint16_t b = uint16_t(_b.u32 >> (ii * 16));
			result.u32 |= uint32_t(a > b ? uint16_t(a - b) : uint16_t(0)) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_and(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 & _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_andc(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 & ~_b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_or(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 | _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_orc(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 | ~_b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_xor(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 ^ _b.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_not(simd32_t _a)
	{
		return { .u32 = ~_a.u32 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_sll(simd32_t _a, int _count)
	{
		return { .u32 = _a.u32 << _count };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_srl(simd32_t _a, int _count)
	{
		return { .u32 = _a.u32 >> _count };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_sra(simd32_t _a, int _count)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t result = { .i32 = a.i32 >> _count };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_sll(simd32_t _a, simd32_t _count)
	{
		return { .u32 = _a.u32 << (_count.u32 & 31u) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_srl(simd32_t _a, simd32_t _count)
	{
		return { .u32 = _a.u32 >> (_count.u32 & 31u) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_sra(simd32_t _a, simd32_t _count)
	{
		const simd32_i32_ref_t a = bitCast<simd32_i32_ref_t>(_a);
		const simd32_i32_ref_t result = { .i32 = a.i32 >> int32_t(_count.u32 & 31u) };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_selb(simd32_t _mask, simd32_t _a, simd32_t _b)
	{
		return { .u32 = (_a.u32 & _mask.u32) | (_b.u32 & ~_mask.u32) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_sels(simd32_t _test, simd32_t _a, simd32_t _b)
	{
		const simd32_i32_ref_t test = bitCast<simd32_i32_ref_t>(_test);
		const uint32_t mask = (uint32_t)(test.i32 >> 31);
		return { .u32 = (_a.u32 & mask) | (_b.u32 & ~mask) };
	}

	inline BX_CONSTEXPR_FUNC bool simd32_test(simd32_t _test)
	{
		return 0 != (_test.u32 >> 31);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_xorl(simd32_t _a, simd32_t _b)
	{
		return { .u32 = uint32_t(!_a.u32 != !_b.u32) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_rol(simd32_t _a, int _count)
	{
		return { .u32 = (_a.u32 << _count) | (_a.u32 >> (32 - _count)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_ror(simd32_t _a, int _count)
	{
		return { .u32 = (_a.u32 >> _count) | (_a.u32 << (32 - _count)) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_setnz(simd32_t _a)
	{
		return { .u32 = uint32_t(-!!_a.u32) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_satadd(simd32_t _a, simd32_t _b)
	{
		const uint32_t add = _a.u32 + _b.u32;
		return { .u32 = add | -(add < _a.u32) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_satsub(simd32_t _a, simd32_t _b)
	{
		const uint32_t sub = _a.u32 - _b.u32;
		return { .u32 = sub & -(sub <= _a.u32) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_satmul(simd32_t _a, simd32_t _b)
	{
		const uint64_t mul = (uint64_t)_a.u32 * (uint64_t)_b.u32;
		const uint32_t hi  = uint32_t(mul >> 32);
		return { .u32 = uint32_t(mul) | uint32_t(-!!hi) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_incwrap(simd32_t _val, simd32_t _min, simd32_t _max)
	{
		simd32_t inc{ .u32 = _val.u32 + 1 };
		const uint32_t diff = _max.u32 - _val.u32;
		simd32_t mask{ .u32 = uint32_t(int32_t(diff | uint32_t(-int32_t(diff))) >> 31) };
		return simd32_selb(mask, inc, _min);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_decwrap(simd32_t _val, simd32_t _min, simd32_t _max)
	{
		simd32_t dec{ .u32 = _val.u32 - 1 };
		const uint32_t diff = _min.u32 - _val.u32;
		simd32_t mask{ .u32 = uint32_t(int32_t(diff | uint32_t(-int32_t(diff))) >> 31) };
		return simd32_selb(mask, dec, _max);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_cntbits(simd32_t _a)
	{
		simd32_t result;
#if BX_COMPILER_GCC || BX_COMPILER_CLANG
		result.u32 = __builtin_popcount(_a.u32);
#else
		uint32_t val = _a.u32;
		val = val - ((val >> 1) & 0x55555555u);
		val = (val & 0x33333333u) + ((val >> 2) & 0x33333333u);
		val = (val + (val >> 4)) & 0x0f0f0f0fu;
		result.u32 = (val * 0x01010101u) >> 24;
#endif // BX_COMPILER_*
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_cntlz(simd32_t _a)
	{
		simd32_t result;
#if BX_COMPILER_GCC || BX_COMPILER_CLANG
		result.u32 = 0 == _a.u32 ? 32 : __builtin_clz(_a.u32);
#else
		uint32_t val = _a.u32;
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		simd32_t tmp{ .u32 = ~val };
		result = simd32_x32_cntbits(tmp);
#endif // BX_COMPILER_*
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_cnttz(simd32_t _a)
	{
		simd32_t result;
#if BX_COMPILER_GCC || BX_COMPILER_CLANG
		result.u32 = 0 == _a.u32 ? 32 : __builtin_ctz(_a.u32);
#else
		simd32_t tmp{ .u32 = ~_a.u32 & (_a.u32 - 1) };
		result = simd32_x32_cntbits(tmp);
#endif // BX_COMPILER_*
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_ffs(simd32_t _a)
	{
		const simd32_t tz = simd32_x32_cnttz(_a);
		return { .u32 = 0 == _a.u32 ? 0 : tz.u32 + 1 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_part1by1(simd32_t _a)
	{
		uint32_t val = _a.u32 & 0xffff;
		val = (val ^ (val << 8)) & 0x00ff00ff;
		val = (val ^ (val << 4)) & 0x0f0f0f0f;
		val = (val ^ (val << 2)) & 0x33333333;
		val = (val ^ (val << 1)) & 0x55555555;
		return { .u32 = val };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_x32_part1by2(simd32_t _a)
	{
		uint32_t val = _a.u32 & 0x3ff;
		val = (val ^ (val << 16) ) & 0xff0000ff;
		val = (val ^ (val <<  8) ) & 0x0300f00f;
		val = (val ^ (val <<  4) ) & 0x030c30c3;
		val = (val ^ (val <<  2) ) & 0x09249249;
		return { .u32 = val };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_testpow2(simd32_t _a)
	{
		const uint32_t dec = _a.u32 - 1;
		return { .u32 = uint32_t(-(int32_t( (_a.u32 ^ dec) >> 1 == dec) ) ) };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_nextpow2(simd32_t _a)
	{
		uint32_t val = _a.u32 - 1;
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		return { .u32 = val + 1 };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_gcd(simd32_t _a, simd32_t _b)
	{
		uint32_t a = _a.u32, b = _b.u32;
		do { const uint32_t tmp = a % b; a = b; b = tmp; } while (b);
		return { .u32 = a };
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_u32_lcm(simd32_t _a, simd32_t _b)
	{
		return { .u32 = _a.u32 * (_b.u32 / simd32_u32_gcd(_a, _b).u32) };
	}

	BX_SIMD_FORCE_INLINE simd32_t simd32_ldu(const void* _ptr)
	{
		return simd32_ld(_ptr);
	}

	BX_SIMD_FORCE_INLINE void simd32_stu(void* _ptr, simd32_t _a)
	{
		simd32_st(_ptr, _a);
	}

	inline BX_CONSTEXPR_FUNC bool simd32_test_any(simd32_t _test)
	{
		return 0 != _test.u32;
	}

	inline BX_CONSTEXPR_FUNC bool simd32_test_all(simd32_t _test)
	{
		return 0 != _test.u32;
	}

	inline BX_CONSTEXPR_FUNC bool simd32_test_zero(simd32_t _a, simd32_t _b)
	{
		return 0 == (_a.u32 & _b.u32);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_ftoi_trunc(simd32_t _a)
	{
		const simd32_f32_ref_t a      = bitCast<simd32_f32_ref_t>(_a);
		const simd32_i32_ref_t result = { .i32 = int32_t(a.f32) };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_f32_ftoi_round(simd32_t _a)
	{
		const simd32_f32_ref_t a      = bitCast<simd32_f32_ref_t>(_a);
		const simd32_i32_ref_t result = { .i32 = int32_t(round(a.f32) ) };
		return bitCast<simd32_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd32_t simd32_i32_itof(simd32_t _a)
	{
		const simd32_i32_ref_t a      = bitCast<simd32_i32_ref_t>(_a);
		const simd32_f32_ref_t result = { .f32 = float(a.i32) };
		return bitCast<simd32_t>(result);
	}

} // namespace bx
