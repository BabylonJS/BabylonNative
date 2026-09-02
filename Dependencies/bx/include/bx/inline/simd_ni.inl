/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{
	// These are used when a platform doesn't have a dedicated instruction.

	template<typename Ty>
	BX_SIMD_INLINE Ty simd128_x32_shuf_xAzC_ni(Ty _a, Ty _b)
	{
		const Ty xAyB   = simd128_x32_shuf_xAyB(_a, _b);
		const Ty zCwD   = simd128_x32_shuf_zCwD(_a, _b);
		const Ty result = simd128_x32_shuf_xyAB(xAyB, zCwD);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd128_x32_shuf_yBwD_ni(Ty _a, Ty _b)
	{
		const Ty xAyB   = simd128_x32_shuf_xAyB(_a, _b);
		const Ty zCwD   = simd128_x32_shuf_zCwD(_a, _b);
		const Ty result = simd128_x32_shuf_zwCD(xAyB, zCwD);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_madd_ni(Ty _a, Ty _b, Ty _c)
	{
		const Ty mul    = simd_f32_mul(_a, _b);
		const Ty result = simd_f32_add(mul, _c);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_nmsub_ni(Ty _a, Ty _b, Ty _c)
	{
		const Ty mul    = simd_f32_mul(_a, _b);
		const Ty result = simd_f32_sub(_c, mul);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_msub_ni(Ty _a, Ty _b, Ty _c)
	{
		const Ty mul    = simd_f32_mul(_a, _b);
		const Ty result = simd_f32_sub(mul, _c);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE int simd_x32_signbitsmask_ni(Ty _a)
	{
		const Ty tmp0   = simd_x32_srl(_a, 31);

		int32_t  lane[4];
		simd_st(&lane, tmp0);

		return (lane[0] & 1) | ((lane[1] & 1) << 1) | ((lane[2] & 1) << 2) | ((lane[3] & 1) << 3);
	}

	template<typename Ty>
	BX_SIMD_INLINE int simd_x8_signbitsmask_ni(Ty _a)
	{
		float tmp[4];
		simd_st(tmp, _a);
		const uint8_t* bytes = reinterpret_cast<const uint8_t*>(tmp);
		int result = 0;
		for (int ii = 0; ii < 16; ++ii)
		{
			result |= ((bytes[ii] >> 7) << ii);
		}
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_div_nr_ni(Ty _a, Ty _b)
	{
		const Ty oneish  = simd_splat<Ty>(uint32_t(0x3f800001));
		const Ty est     = simd_f32_rcp_est(_b);
		const Ty iter0   = simd_f32_mul(_a, est);
		const Ty tmp1    = simd_f32_nmsub(_b, est, oneish);
		const Ty result  = simd_f32_madd(tmp1, iter0, iter0);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_rcp_ni(Ty _a)
	{
		const Ty one    = simd_splat<Ty>(1.0f);
		const Ty result = simd_f32_div(one, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_neg_ni(Ty _a)
	{
		const Ty zero   = simd_zero<Ty>();
		const Ty result = simd_f32_sub(zero, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_abs_ni(Ty _a)
	{
		const Ty a_neg  = simd_f32_neg(_a);
		const Ty result = simd_f32_max(a_neg, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f64_abs_ni(Ty _a)
	{
		const Ty a_neg  = simd_f64_neg(_a);
		const Ty result = simd_f64_max(a_neg, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_andc_ni(Ty _a, Ty _b)
	{
		const Ty nb     = simd_not(_b);
		const Ty result = simd_and(_a, nb);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_u32_cmplt_ni(Ty _a, Ty _b)
	{
		const Ty flip   = simd_splat<Ty>(uint32_t(INT32_MIN));
		const Ty af     = simd_xor(_a, flip);
		const Ty bf     = simd_xor(_b, flip);
		const Ty result = simd_i32_cmplt(af, bf);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_u32_cmpgt_ni(Ty _a, Ty _b)
	{
		const Ty flip   = simd_splat<Ty>(uint32_t(INT32_MIN));
		const Ty af     = simd_xor(_a, flip);
		const Ty bf     = simd_xor(_b, flip);
		const Ty result = simd_i32_cmpgt(af, bf);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f64_madd_ni(Ty _a, Ty _b, Ty _c)
	{
		const Ty mul    = simd_f64_mul(_a, _b);
		const Ty result = simd_f64_add(mul, _c);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f64_nmsub_ni(Ty _a, Ty _b, Ty _c)
	{
		const Ty mul    = simd_f64_mul(_a, _b);
		const Ty result = simd_f64_sub(_c, mul);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f64_neg_ni(Ty _a)
	{
		const Ty zero   = simd_zero<Ty>();
		const Ty result = simd_f64_sub(zero, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f64_lerp_ni(Ty _a, Ty _b, Ty _s)
	{
		const Ty ba     = simd_f64_sub(_b, _a);
		const Ty result = simd_f64_madd(_s, ba, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f64_rcp_ni(Ty _a)
	{
		const Ty one    = simd_splat<Ty>(1.0);
		const Ty result = simd_f64_div(one, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f64_rsqrt_ni(Ty _a)
	{
		const Ty sqr    = simd_f64_sqrt(_a);
		const Ty result = simd_f64_rcp(sqr);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f64_cmpneq_ni(Ty _a, Ty _b)
	{
		const Ty tmp0   = simd_f64_cmpeq(_a, _b);
		const Ty result = simd_not(tmp0);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_min_ni(Ty _a, Ty _b)
	{
		const Ty mask   = simd_f32_cmplt(_a, _b);
		const Ty result = simd_selb(mask, _a, _b);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_max_ni(Ty _a, Ty _b)
	{
		const Ty mask   = simd_f32_cmpgt(_a, _b);
		const Ty result = simd_selb(mask, _a, _b);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_clamp_ni(Ty _a, Ty _min, Ty _max)
	{
		const Ty tmp    = simd_f32_min(_a, _max);
		const Ty result = simd_f32_max(tmp, _min);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_lerp_ni(Ty _a, Ty _b, Ty _s)
	{
		const Ty ba     = simd_f32_sub(_b, _a);
		const Ty result = simd_f32_madd(_s, ba, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_sqrt_nr_ni(Ty _a)
	{
		const Ty half   = simd_splat<Ty>(0.5f);
		const Ty one    = simd_splat<Ty>(1.0f);
		const Ty tmp0   = simd_f32_rsqrt_est(_a);
		const Ty tmp1   = simd_f32_mul(tmp0, _a);
		const Ty tmp2   = simd_f32_mul(tmp1, half);
		const Ty tmp3   = simd_f32_nmsub(tmp0, tmp1, one);
		const Ty result = simd_f32_madd(tmp3, tmp2, tmp1);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_sqrt_nr1_ni(Ty _a)
	{
		const Ty half = simd_splat<Ty>(0.5f);

		Ty result = _a;
		for (uint32_t ii = 0; ii < 11; ++ii)
		{
			const Ty tmp1 = simd_f32_div(_a, result);
			const Ty tmp2 = simd_f32_add(tmp1, result);
			result        = simd_f32_mul(tmp2, half);
		}

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_rsqrt_ni(Ty _a)
	{
		const Ty one    = simd_splat<Ty>(1.0f);
		const Ty sqr    = simd_f32_sqrt(_a);
		const Ty result = simd_f32_div(one, sqr);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_rsqrt_nr_ni(Ty _a)
	{
		const Ty rsqrt           = simd_f32_rsqrt_est(_a);
		const Ty iter0           = simd_f32_mul(_a, rsqrt);
		const Ty iter1           = simd_f32_mul(iter0, rsqrt);
		const Ty half            = simd_splat<Ty>(0.5f);
		const Ty half_rsqrt      = simd_f32_mul(half, rsqrt);
		const Ty three           = simd_splat<Ty>(3.0f);
		const Ty three_sub_iter1 = simd_f32_sub(three, iter1);
		const Ty result          = simd_f32_mul(half_rsqrt, three_sub_iter1);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_rsqrt_carmack_ni(Ty _a)
	{
		const Ty half    = simd_splat<Ty>(0.5f);
		const Ty ah      = simd_f32_mul(half, _a);
		const Ty ashift  = simd_x32_sra(_a, 1);
		const Ty magic   = simd_splat<Ty>(uint32_t(0x5f3759df));
		const Ty msuba   = simd_i32_sub(magic, ashift);
		const Ty msubasq = simd_f32_mul(msuba, msuba);
		const Ty tmp0    = simd_splat<Ty>(1.5f);
		const Ty tmp1    = simd_f32_mul(ah, msubasq);
		const Ty tmp2    = simd_f32_sub(tmp0, tmp1);
		const Ty result  = simd_f32_mul(msuba, tmp2);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_cmpneq_ni(Ty _a, Ty _b)
	{
		const Ty tmp0   = simd_f32_cmpeq(_a, _b);
		const Ty result = simd_not(tmp0);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_i32_min_ni(Ty _a, Ty _b)
	{
		const Ty mask   = simd_i32_cmplt(_a, _b);
		const Ty result = simd_selb(mask, _a, _b);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_i32_max_ni(Ty _a, Ty _b)
	{
		const Ty mask   = simd_i32_cmpgt(_a, _b);
		const Ty result = simd_selb(mask, _a, _b);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_i32_neg_ni(Ty _a)
	{
		const Ty zero   = simd_zero<Ty>();
		const Ty result = simd_i32_sub(zero, _a);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_i32_abs_ni(Ty _a)
	{
		const Ty neg    = simd_i32_neg(_a);
		const Ty result = simd_i32_max(_a, neg);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_selb_ni(Ty _mask, Ty _a, Ty _b)
	{
		const Ty sel_a  = simd_and(_a, _mask);
		const Ty sel_b  = simd_andc(_b, _mask);
		const Ty result = simd_or(sel_a, sel_b);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_sels_ni(Ty _test, Ty _a, Ty _b)
	{
		const Ty mask   = simd_x32_sra(_test, 31);
		const Ty result = simd_selb(mask, _a, _b);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_not_ni(Ty _a)
	{
		const Ty mffff  = simd_splat<Ty>(UINT32_MAX);
		const Ty result = simd_xor(_a, mffff);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_orc_ni(Ty _a, Ty _b)
	{
		const Ty nb     = simd_not(_b);
		const Ty result = simd_or(_a, nb);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd128_orx_ni(Ty _a)
	{
		const Ty zwxy   = simd128_x32_swiz_zwxy(_a);
		const Ty tmp0   = simd_or(_a, zwxy);
		const Ty tmp1   = simd128_x32_swiz_yyyy(tmp0);
		const Ty tmp2   = simd_or(tmp0, tmp1);
		const Ty mf000  = simd128_ld<Ty>(UINT32_MAX, 0u, 0u, 0u);
		const Ty result = simd_and(tmp2, mf000);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd128_f32_dot3_ni(Ty _a, Ty _b)
	{
		const Ty xyzw   = simd_f32_mul(_a, _b);
		const Ty xxxx   = simd128_x32_swiz_xxxx(xyzw);
		const Ty yyyy   = simd128_x32_swiz_yyyy(xyzw);
		const Ty zzzz   = simd128_x32_swiz_zzzz(xyzw);
		const Ty tmp1   = simd_f32_add(xxxx, yyyy);
		const Ty result = simd_f32_add(tmp1, zzzz);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd128_f32_dot_ni(Ty _a, Ty _b)
	{
		const Ty xyzw   = simd_f32_mul(_a, _b);
		const Ty zwxy   = simd128_x32_swiz_zwxy(xyzw);
		const Ty tmp0   = simd_f32_add(xyzw, zwxy);
		const Ty yxwz   = simd128_x32_swiz_yxwz(tmp0);
		const Ty result = simd_f32_add(tmp0, yxwz);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd128_f32_cross3_ni(Ty _a, Ty _b)
	{
		// a.yzx * b.zxy - a.zxy * b.yzx
		const Ty a_yzxw = simd128_x32_swiz_yzxw(_a);
		const Ty b_zxyw = simd128_x32_swiz_zxyw(_b);
		const Ty a_zxyw = simd128_x32_swiz_zxyw(_a);
		const Ty b_yzxw = simd128_x32_swiz_yzxw(_b);
		const Ty tmp0   = simd_f32_mul(a_yzxw, b_zxyw);
		const Ty tmp1   = simd_f32_mul(a_zxyw, b_yzxw);
		const Ty result = simd_f32_sub(tmp0, tmp1);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd128_f32_normalize3_ni(Ty _a)
	{
		const Ty dot    = simd128_f32_dot3(_a, _a);
		const Ty invLen = simd_f32_rsqrt(dot);
		const Ty result = simd_f32_mul(_a, invLen);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_ceil_ni(Ty _a)
	{
		const Ty tmp    = simd_f32_ftoi_trunc(_a);
		const Ty tmp0   = simd_i32_itof(tmp);
		const Ty mask   = simd_f32_cmplt(tmp0, _a);
		const Ty one    = simd_splat<Ty>(1.0f);
		const Ty tmp1   = simd_and(one, mask);
		const Ty result = simd_f32_add(tmp0, tmp1);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_floor_ni(Ty _a)
	{
		const Ty tmp    = simd_f32_ftoi_trunc(_a);
		const Ty tmp0   = simd_i32_itof(tmp);
		const Ty mask   = simd_f32_cmpgt(tmp0, _a);
		const Ty one    = simd_splat<Ty>(1.0f);
		const Ty tmp1   = simd_and(one, mask);
		const Ty result = simd_f32_sub(tmp0, tmp1);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_round_ni(Ty _a)
	{
		const Ty half   = simd_splat<Ty>(0.5f);
		const Ty tmp0   = simd_f32_add(_a, half);
		const Ty result = simd_f32_floor(tmp0);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_ldexp_ni(Ty _a, Ty _b)
	{
		const Ty signexpmask = simd_splat<Ty>(uint32_t(kFloatSignMask | kFloatExponentMask) );
		const Ty mantmask    = simd_splat<Ty>(kFloatMantissaMask);
		const Ty masked      = simd_and(_a, signexpmask);
		const Ty expsign0    = simd_x32_sra(masked, kFloatExponentBitShift);
		const Ty tmp         = simd_i32_add(expsign0, _b);
		const Ty expsign1    = simd_x32_sll(tmp, kFloatExponentBitShift);
		const Ty mantissa    = simd_and(_a, mantmask);
		const Ty result      = simd_or(mantissa, expsign1);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_cos_ni(Ty _a)
	{
		const Ty two_over_pi = simd_splat<Ty>(2.0f * kInvPi);
		const Ty pi_half     = simd_splat<Ty>(kPiHalf);
		const Ty scaled      = simd_f32_mul(_a, two_over_pi);
		const Ty real        = simd_f32_floor(scaled);
		const Ty tmp_rp      = simd_f32_mul(real, pi_half);
		const Ty xx          = simd_f32_sub(_a, tmp_rp);
		const Ty ireal       = simd_f32_ftoi_trunc(real);
		const Ty three       = simd_splat<Ty>(uint32_t(3) );
		const Ty ibits       = simd_and(ireal, three);
		const Ty ione        = simd_splat<Ty>(uint32_t(1) );
		const Ty izero       = simd_zero<Ty>();
		const Ty bit0        = simd_and(ibits, ione);
		const Ty evenMask    = simd_i32_cmpeq(bit0, izero);
		const Ty sinC2       = simd_splat<Ty>(-0.16666667163372039794921875f);
		const Ty sinC4       = simd_splat<Ty>( 8.333347737789154052734375e-3f);
		const Ty sinC6       = simd_splat<Ty>(-1.9842604524455964565277099609375e-4f);
		const Ty sinC8       = simd_splat<Ty>( 2.760012648650445044040679931640625e-6f);
		const Ty sinC10      = simd_splat<Ty>(-2.50293279435709337121807038784027099609375e-8f);
		const Ty cosC2       = simd_splat<Ty>(-0.5f);
		const Ty cosC4       = simd_splat<Ty>( 4.166664183139801025390625e-2f);
		const Ty cosC6       = simd_splat<Ty>(-1.388833043165504932403564453125e-3f);
		const Ty cosC8       = simd_splat<Ty>( 2.47562347794882953166961669921875e-5f);
		const Ty cosC10      = simd_splat<Ty>(-2.59630184018533327616751194000244140625e-7f);
		const Ty one         = simd_splat<Ty>(1.0f);
		const Ty c0          = simd_selb(evenMask, one, xx);
		const Ty c2          = simd_selb(evenMask, cosC2,  sinC2);
		const Ty c4          = simd_selb(evenMask, cosC4,  sinC4);
		const Ty c6          = simd_selb(evenMask, cosC6,  sinC6);
		const Ty c8          = simd_selb(evenMask, cosC8,  sinC8);
		const Ty c10         = simd_selb(evenMask, cosC10, sinC10);
		const Ty xsq         = simd_f32_mul(xx, xx);
		const Ty tmp0        = simd_f32_madd(c10,  xsq, c8);
		const Ty tmp1        = simd_f32_madd(tmp0, xsq, c6);
		const Ty tmp2        = simd_f32_madd(tmp1, xsq, c4);
		const Ty tmp3        = simd_f32_madd(tmp2, xsq, c2);
		const Ty tmp4        = simd_f32_madd(tmp3, xsq, one);
		const Ty poly        = simd_f32_mul(tmp4, c0);
		const Ty ibits1      = simd_x32_srl(ibits, 1);
		const Ty bit1        = simd_and(ibits1, ione);
		const Ty negmask     = simd_xor(bit0, bit1);
		const Ty negbits     = simd_x32_sll(negmask, 31);
		const Ty result      = simd_xor(poly, negbits);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_sin_ni(Ty _a)
	{
		const Ty pi_half = simd_splat<Ty>(kPiHalf);
		const Ty shifted = simd_f32_sub(_a, pi_half);
		const Ty result  = simd_f32_cos(shifted);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_log_ni(Ty _a)
	{
		const Ty zero       = simd_zero<Ty>();
		const Ty one        = simd_splat<Ty>(1.0f);
		const Ty negmask    = simd_f32_cmplt(_a, zero);
		const Ty zeromask   = simd_f32_cmpeq(_a, zero);
		const Ty nan_val    = simd_splat<Ty>(uint32_t(kFloatSignMask | kFloatExponentMask | kFloatMantissaMask) );
		const Ty neg_inf    = simd_splat<Ty>(uint32_t(kFloatSignMask | kFloatExponentMask) );
		const Ty expmask_c  = simd_splat<Ty>(kFloatExponentMask);
		const Ty mantmask_c = simd_splat<Ty>(kFloatMantissaMask);
		const Ty exp_half   = simd_splat<Ty>(uint32_t(0x3f000000) );
		const Ty bias_126   = simd_splat<Ty>(uint32_t(kFloatExponentBias - 1) );
		const Ty aexp       = simd_and(_a, expmask_c);
		const Ty rawexp     = simd_x32_srl(aexp, kFloatExponentBitShift);
		const Ty exp_i0     = simd_i32_sub(rawexp, bias_126);
		const Ty mant       = simd_and(_a, mantmask_c);
		const Ty ff0        = simd_or(mant, exp_half);
		const Ty sqrt2_half = simd_splat<Ty>(kSqrt2 * 0.5f);
		const Ty adj_mask   = simd_f32_cmplt(ff0, sqrt2_half);
		const Ty two        = simd_splat<Ty>(2.0f);
		const Ty ff_dbl     = simd_f32_mul(ff0, two);
		const Ty ff1        = simd_selb(adj_mask, ff_dbl, ff0);
		const Ty ione       = simd_splat<Ty>(uint32_t(1) );
		const Ty adj_one    = simd_and(adj_mask, ione);
		const Ty exp_i      = simd_i32_sub(exp_i0, adj_one);
		const Ty kC0        = simd_splat<Ty>(6.666666666666735130e-01f);
		const Ty kC1        = simd_splat<Ty>(3.999999999940941908e-01f);
		const Ty kC2        = simd_splat<Ty>(2.857142874366239149e-01f);
		const Ty kC3        = simd_splat<Ty>(2.222219843214978396e-01f);
		const Ty kC4        = simd_splat<Ty>(1.818357216161805012e-01f);
		const Ty kC5        = simd_splat<Ty>(1.531383769920937332e-01f);
		const Ty kC6        = simd_splat<Ty>(1.479819860511658591e-01f);
		const Ty log2lo     = simd_splat<Ty>(1.90821492927058770002e-10f);
		const Ty lognat2    = simd_splat<Ty>(kLogNat2);
		const Ty half       = simd_splat<Ty>(0.5f);
		const Ty ff         = simd_f32_sub(ff1, one);
		const Ty kk         = simd_i32_itof(exp_i);
		const Ty hi         = simd_f32_mul(kk, lognat2);
		const Ty lo         = simd_f32_mul(kk, log2lo);
		const Ty tpf        = simd_f32_add(two, ff);
		const Ty ss         = simd_f32_div(ff, tpf);
		const Ty s2         = simd_f32_mul(ss, ss);
		const Ty s4         = simd_f32_mul(s2, s2);
		const Ty pe0        = simd_f32_madd(kC6, s4, kC4);
		const Ty pe1        = simd_f32_madd(pe0, s4, kC2);
		const Ty pe2        = simd_f32_madd(pe1, s4, kC0);
		const Ty t1         = simd_f32_mul(s2, pe2);
		const Ty po0        = simd_f32_madd(kC5, s4, kC3);
		const Ty po1        = simd_f32_madd(po0, s4, kC1);
		const Ty t2         = simd_f32_mul(s4, po1);
		const Ty t12        = simd_f32_add(t1, t2);
		const Ty ffsq       = simd_f32_mul(ff, ff);
		const Ty hfsq       = simd_f32_mul(half, ffsq);
		const Ty ht12       = simd_f32_add(hfsq, t12);
		const Ty ss_ht      = simd_f32_mul(ss, ht12);
		const Ty shtlo      = simd_f32_add(ss_ht, lo);
		const Ty inner      = simd_f32_sub(hfsq, shtlo);
		const Ty inff       = simd_f32_sub(inner, ff);
		const Ty raw        = simd_f32_sub(hi, inff);
		const Ty rz         = simd_selb(zeromask, neg_inf, raw);
		const Ty result     = simd_selb(negmask, nan_val, rz);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_exp_ni(Ty _a)
	{
		const Ty zero        = simd_zero<Ty>();
		const Ty one         = simd_splat<Ty>(1.0f);
		const Ty near_zero   = simd_splat<Ty>(kNearZero);
		const Ty absa        = simd_f32_abs(_a);
		const Ty near_mask   = simd_f32_cmple(absa, near_zero);
		const Ty near_result = simd_f32_add(_a, one);
		const Ty exp_min     = simd_splat<Ty>(-87.33654475f);
		const Ty clamp_mask  = simd_f32_cmple(_a, exp_min);
		const Ty kC0         = simd_splat<Ty>( 1.66666666666666019037e-01f);
		const Ty kC1         = simd_splat<Ty>(-2.77777777770155933842e-03f);
		const Ty kC2         = simd_splat<Ty>( 6.61375632143793436117e-05f);
		const Ty kC3         = simd_splat<Ty>(-1.65339022054652515390e-06f);
		const Ty kC4         = simd_splat<Ty>( 4.13813679705723846039e-08f);
		const Ty log2lo      = simd_splat<Ty>(1.90821492927058770002e-10f);
		const Ty lognat2     = simd_splat<Ty>(kLogNat2);
		const Ty invlognat2  = simd_splat<Ty>(kInvLogNat2);
		const Ty two         = simd_splat<Ty>(2.0f);
		const Ty amul        = simd_f32_mul(_a, invlognat2);
		const Ty kk          = simd_f32_round(amul);
		const Ty kkln        = simd_f32_mul(kk, lognat2);
		const Ty hi          = simd_f32_sub(_a, kkln);
		const Ty lo          = simd_f32_mul(kk, log2lo);
		const Ty hml         = simd_f32_sub(hi, lo);
		const Ty hmlsq       = simd_f32_mul(hml, hml);
		const Ty tmp0        = simd_f32_madd(kC4, hmlsq, kC3);
		const Ty tmp1        = simd_f32_madd(tmp0, hmlsq, kC2);
		const Ty tmp2        = simd_f32_madd(tmp1, hmlsq, kC1);
		const Ty tmp3        = simd_f32_madd(tmp2, hmlsq, kC0);
		const Ty tmp3m       = simd_f32_mul(hmlsq, tmp3);
		const Ty tmp4        = simd_f32_sub(hml, tmp3m);
		const Ty tmp4a       = simd_f32_mul(hml, tmp4);
		const Ty tmp4b       = simd_f32_sub(two, tmp4);
		const Ty tmp5        = simd_f32_div(tmp4a, tmp4b);
		const Ty tmp5a       = simd_f32_sub(lo, tmp5);
		const Ty tmp5b       = simd_f32_sub(tmp5a, hi);
		const Ty tmp6        = simd_f32_sub(one, tmp5b);
		const Ty ikk         = simd_f32_ftoi_trunc(kk);
		const Ty raw         = simd_f32_ldexp_ni(tmp6, ikk);
		const Ty rc          = simd_selb(clamp_mask, zero, raw);
		const Ty result      = simd_selb(near_mask, near_result, rc);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_log2_ni(Ty _a)
	{
		const Ty inv_log2 = simd_splat<Ty>(kInvLogNat2);
		const Ty loga     = simd_f32_log(_a);
		const Ty result   = simd_f32_mul(loga, inv_log2);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_exp2_ni(Ty _a)
	{
		const Ty lognat2 = simd_splat<Ty>(kLogNat2);
		const Ty scaled  = simd_f32_mul(_a, lognat2);
		const Ty result  = simd_f32_exp(scaled);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_pow_ni(Ty _a, Ty _b)
	{
		const Ty zero     = simd_zero<Ty>();
		const Ty one      = simd_splat<Ty>(1.0f);
		const Ty smallest = simd_splat<Ty>(kFloatSmallest);
		const Ty signmask = simd_splat<Ty>(kFloatSignMask);

		const Ty absa     = simd_f32_abs(_a);
		const Ty absb     = simd_f32_abs(_b);
		const Ty loga     = simd_f32_log(absa);
		const Ty bloga    = simd_f32_mul(_b, loga);
		const Ty pw       = simd_f32_exp(bloga);
		const Ty asign    = simd_and(_a, signmask);
		const Ty pwabs    = simd_f32_abs(pw);
		const Ty result0  = simd_or(pwabs, asign);
		const Ty bmask    = simd_f32_cmplt(absb, smallest);
		const Ty amask    = simd_f32_cmplt(absa, smallest);
		const Ty result1  = simd_selb(bmask, one, result0);
		const Ty result   = simd_selb(amask, zero, result1);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE bool simd128_test_any_ni(Ty _a)
	{
		const Ty tmp0   = simd_x32_srl(_a, 31);
		const Ty tmp1   = simd128_x32_swiz_yzwx(tmp0);
		const Ty tmp2   = simd_or(tmp0, tmp1);
		const Ty tmp3   = simd128_x32_swiz_zwxy(tmp2);
		const Ty tmp4   = simd_or(tmp2, tmp3);

		int32_t  ii;
		simd128_x32_st1(&ii, tmp4);

		return 0 != ii;
	}

	template<typename Ty>
	BX_SIMD_INLINE bool simd128_test_all_ni(Ty _a)
	{
		const Ty tmp0   = simd_x32_srl(_a, 31);
		const Ty tmp1   = simd128_x32_swiz_yzwx(tmp0);
		const Ty tmp2   = simd_and(tmp0, tmp1);
		const Ty tmp3   = simd128_x32_swiz_zwxy(tmp2);
		const Ty tmp4   = simd_and(tmp2, tmp3);

		int32_t  ii;
		simd128_x32_st1(&ii, tmp4);

		return 0 != ii;
	}

	template<typename Ty>
	BX_SIMD_INLINE bool simd128_test_zero_ni(Ty _a, Ty _b)
	{
		const Ty masked = simd_and(_a, _b);
		const Ty zero   = simd_zero<Ty>();
		const Ty cmp    = simd_i32_cmpeq(masked, zero);
		const bool result = simd_test_all(cmp);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f32_ftoi_round_ni(Ty _a)
	{
		const Ty rounded = simd_f32_round(_a);
		const Ty result  = simd_f32_ftoi_trunc(rounded);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_u32_cntlz_ni(Ty _a)
	{
		// Smear the highest set bit down to all lower bits.
		const Ty shr1   = simd_x32_srl(_a,   1);
		const Ty or1    = simd_or(_a, shr1);
		const Ty shr2   = simd_x32_srl(or1,  2);
		const Ty or2    = simd_or(or1, shr2);
		const Ty shr4   = simd_x32_srl(or2,  4);
		const Ty or4    = simd_or(or2, shr4);
		const Ty shr8   = simd_x32_srl(or4,  8);
		const Ty or8    = simd_or(or4, shr8);
		const Ty shr16  = simd_x32_srl(or8, 16);
		const Ty smear  = simd_or(or8, shr16);
		const Ty inv    = simd_not(smear);

		const Ty c55    = simd_splat<Ty>(uint32_t(0x55555555) );
		const Ty c33    = simd_splat<Ty>(uint32_t(0x33333333) );
		const Ty c0f    = simd_splat<Ty>(uint32_t(0x0f0f0f0f) );
		const Ty c3f    = simd_splat<Ty>(uint32_t(0x3f) );

		const Ty p1s    = simd_x32_srl(inv, 1);
		const Ty p1m    = simd_and(p1s, c55);
		const Ty p1     = simd_u32_sub(inv, p1m);

		const Ty p2a    = simd_and(p1, c33);
		const Ty p2s    = simd_x32_srl(p1, 2);
		const Ty p2m    = simd_and(p2s, c33);
		const Ty p2     = simd_u32_add(p2a, p2m);

		const Ty p4s    = simd_x32_srl(p2, 4);
		const Ty p4a    = simd_u32_add(p2, p4s);
		const Ty p4     = simd_and(p4a, c0f);

		const Ty p8s    = simd_x32_srl(p4,  8);
		const Ty p8     = simd_u32_add(p4, p8s);

		const Ty p16s   = simd_x32_srl(p8, 16);
		const Ty p16    = simd_u32_add(p8, p16s);

		const Ty result = simd_and(p16, c3f);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_x32_srl_ni(Ty _a, Ty _count)
	{
		const Ty bit0    = simd_splat<Ty>(uint32_t(1) );
		const Ty bit1    = simd_splat<Ty>(uint32_t(2) );
		const Ty bit2    = simd_splat<Ty>(uint32_t(4) );
		const Ty bit3    = simd_splat<Ty>(uint32_t(8) );
		const Ty bit4    = simd_splat<Ty>(uint32_t(16) );

		const Ty b0      = simd_and(_count, bit0);
		const Ty mask0   = simd_i32_neg(b0);
		const Ty shr0    = simd_x32_srl(_a, 1);
		const Ty r0      = simd_selb(mask0, shr0, _a);

		const Ty b1      = simd_and(_count, bit1);
		const Ty mask1   = simd_i32_neg(b1);
		const Ty shr1    = simd_x32_srl(r0, 2);
		const Ty r1      = simd_selb(mask1, shr1, r0);

		const Ty b2      = simd_and(_count, bit2);
		const Ty mask2   = simd_i32_neg(b2);
		const Ty shr2    = simd_x32_srl(r1, 4);
		const Ty r2      = simd_selb(mask2, shr2, r1);

		const Ty b3      = simd_and(_count, bit3);
		const Ty mask3   = simd_i32_neg(b3);
		const Ty shr3    = simd_x32_srl(r2, 8);
		const Ty r3      = simd_selb(mask3, shr3, r2);

		const Ty b4      = simd_and(_count, bit4);
		const Ty mask4   = simd_i32_neg(b4);
		const Ty shr4    = simd_x32_srl(r3, 16);
		const Ty result  = simd_selb(mask4, shr4, r3);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_x32_sll_ni(Ty _a, Ty _count)
	{
		const Ty bit0    = simd_splat<Ty>(uint32_t(1) );
		const Ty bit1    = simd_splat<Ty>(uint32_t(2) );
		const Ty bit2    = simd_splat<Ty>(uint32_t(4) );
		const Ty bit3    = simd_splat<Ty>(uint32_t(8) );
		const Ty bit4    = simd_splat<Ty>(uint32_t(16) );

		const Ty b0      = simd_and(_count, bit0);
		const Ty mask0   = simd_i32_neg(b0);
		const Ty sll0    = simd_x32_sll(_a, 1);
		const Ty r0      = simd_selb(mask0, sll0, _a);

		const Ty b1      = simd_and(_count, bit1);
		const Ty mask1   = simd_i32_neg(b1);
		const Ty sll1    = simd_x32_sll(r0, 2);
		const Ty r1      = simd_selb(mask1, sll1, r0);

		const Ty b2      = simd_and(_count, bit2);
		const Ty mask2   = simd_i32_neg(b2);
		const Ty sll2    = simd_x32_sll(r1, 4);
		const Ty r2      = simd_selb(mask2, sll2, r1);

		const Ty b3      = simd_and(_count, bit3);
		const Ty mask3   = simd_i32_neg(b3);
		const Ty sll3    = simd_x32_sll(r2, 8);
		const Ty r3      = simd_selb(mask3, sll3, r2);

		const Ty b4      = simd_and(_count, bit4);
		const Ty mask4   = simd_i32_neg(b4);
		const Ty sll4    = simd_x32_sll(r3, 16);
		const Ty result  = simd_selb(mask4, sll4, r3);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_x32_sra_ni(Ty _a, Ty _count)
	{
		const Ty bit0    = simd_splat<Ty>(uint32_t(1) );
		const Ty bit1    = simd_splat<Ty>(uint32_t(2) );
		const Ty bit2    = simd_splat<Ty>(uint32_t(4) );
		const Ty bit3    = simd_splat<Ty>(uint32_t(8) );
		const Ty bit4    = simd_splat<Ty>(uint32_t(16) );

		const Ty b0      = simd_and(_count, bit0);
		const Ty mask0   = simd_i32_neg(b0);
		const Ty sra0    = simd_x32_sra(_a, 1);
		const Ty r0      = simd_selb(mask0, sra0, _a);

		const Ty b1      = simd_and(_count, bit1);
		const Ty mask1   = simd_i32_neg(b1);
		const Ty sra1    = simd_x32_sra(r0, 2);
		const Ty r1      = simd_selb(mask1, sra1, r0);

		const Ty b2      = simd_and(_count, bit2);
		const Ty mask2   = simd_i32_neg(b2);
		const Ty sra2    = simd_x32_sra(r1, 4);
		const Ty r2      = simd_selb(mask2, sra2, r1);

		const Ty b3      = simd_and(_count, bit3);
		const Ty mask3   = simd_i32_neg(b3);
		const Ty sra3    = simd_x32_sra(r2, 8);
		const Ty r3      = simd_selb(mask3, sra3, r2);

		const Ty b4      = simd_and(_count, bit4);
		const Ty mask4   = simd_i32_neg(b4);
		const Ty sra4    = simd_x32_sra(r3, 16);
		const Ty result  = simd_selb(mask4, sra4, r3);

		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_x8_shuffle_ni(Ty _a, Ty _indices)
	{
		alignas(32) uint8_t aBuf[sizeof(Ty)];
		alignas(32) uint8_t iBuf[sizeof(Ty)];
		alignas(32) uint8_t oBuf[sizeof(Ty)];
		simd_st<Ty>(aBuf, _a);
		simd_st<Ty>(iBuf, _indices);
		// Per-16-byte-lane shuffle: matches PSHUFB / vqtbl1q semantics.
		// For widths < 16 bytes, the lane is the full register.
		constexpr int kLaneBytes = sizeof(Ty) >= 16 ? 16 : int(sizeof(Ty));
		constexpr int kLaneMask  = kLaneBytes - 1;
		for (int lane = 0; lane < int(sizeof(Ty)); lane += kLaneBytes)
		{
			for (int ii = 0; ii < kLaneBytes; ++ii)
			{
				const uint8_t idx = iBuf[lane + ii];
				oBuf[lane + ii] = (idx & 0x80) ? uint8_t(0) : aBuf[lane + (idx & kLaneMask)];
			}
		}
		return simd_ld<Ty>(oBuf);
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_x8_shuffle_ni(Ty _a, Ty _b, Ty _indices)
	{
		alignas(32) uint8_t aBuf[sizeof(Ty)];
		alignas(32) uint8_t bBuf[sizeof(Ty)];
		alignas(32) uint8_t iBuf[sizeof(Ty)];
		alignas(32) uint8_t oBuf[sizeof(Ty)];
		simd_st<Ty>(aBuf, _a);
		simd_st<Ty>(bBuf, _b);
		simd_st<Ty>(iBuf, _indices);
		// Two-source per-16-byte-lane shuffle: indices select from concatenated
		// [a|b] within the matching 16-byte lane (or full register for < 16 bytes).
		// Bit 7 of an index byte zeroes the output byte; bits 0..(log2(2*lane)-1)
		// select within the 2*lane concatenation; remaining bits must be 0.
		constexpr int kLaneBytes = sizeof(Ty) >= 16 ? 16 : int(sizeof(Ty));
		constexpr int kPairMask  = (kLaneBytes * 2) - 1;
		for (int lane = 0; lane < int(sizeof(Ty)); lane += kLaneBytes)
		{
			for (int ii = 0; ii < kLaneBytes; ++ii)
			{
				const uint8_t idx = iBuf[lane + ii];
				if (idx & 0x80)
				{
					oBuf[lane + ii] = 0;
				}
				else
				{
					const int sel = idx & kPairMask;
					oBuf[lane + ii] = sel < kLaneBytes ? aBuf[lane + sel] : bBuf[lane + sel - kLaneBytes];
				}
			}
		}
		return simd_ld<Ty>(oBuf);
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f16_fromf32_ni(Ty _a)
	{
		const Ty f_s_mask                  = simd_splat<Ty>(kFloatSignMask);
		const Ty f_e_mask                  = simd_splat<Ty>(kFloatExponentMask);
		const Ty f_m_mask                  = simd_splat<Ty>(kFloatMantissaMask);
		const Ty f_m_hidden_bit            = simd_splat<Ty>(uint32_t(0x00800000) );
		const Ty f_m_round_bit             = simd_splat<Ty>(uint32_t(0x00001000) );
		const Ty f_snan_mask               = simd_splat<Ty>(uint32_t(0x7fc00000) );
		const Ty h_e_mask                  = simd_splat<Ty>(uint32_t(kHalfExponentMask) );
		const Ty h_snan_mask               = simd_splat<Ty>(uint32_t(0x00007e00) );
		const Ty h_e_mask_value            = simd_splat<Ty>(uint32_t(0x0000001f) );
		const Ty f_h_bias                  = simd_splat<Ty>(uint32_t(0x00000070) );
		const Ty h_nan_min                 = simd_splat<Ty>(uint32_t(0x00007c01) );
		const Ty f_h_e_biased              = simd_splat<Ty>(uint32_t(0x0000008f) );
		const Ty one                       = simd_splat<Ty>(uint32_t(1) );

		const Ty f_s                       = simd_and(_a, f_s_mask); //
		const Ty f_e                       = simd_and(_a, f_e_mask);
		const Ty h_s                       = simd_x32_srl(f_s, 16);
		const Ty f_m                       = simd_and(_a, f_m_mask);
		const Ty f_e_amount                = simd_x32_srl(f_e, kFloatExponentBitShift);
		const Ty f_e_half_bias             = simd_u32_sub(f_e_amount, f_h_bias);
		const Ty f_snan                    = simd_and(_a, f_snan_mask);
		const Ty f_m_round_mask            = simd_and(f_m, f_m_round_bit);
		const Ty f_m_round_offset          = simd_x32_sll(f_m_round_mask, 1);
		const Ty f_m_rounded               = simd_u32_add(f_m, f_m_round_offset);
		const Ty f_m_denorm_sa             = simd_u32_sub(one, f_e_half_bias);
		const Ty f_m_with_hidden           = simd_or(f_m_rounded, f_m_hidden_bit);
		const Ty f_m_denorm                = simd_x32_srl_ni(f_m_with_hidden, f_m_denorm_sa);
		const Ty h_m_denorm                = simd_x32_srl(f_m_denorm, 13);
		const Ty f_m_rounded_overflow      = simd_and(f_m_rounded, f_m_hidden_bit);
		const Ty m_nan                     = simd_x32_srl(f_m, 13);
		const Ty h_em_nan                  = simd_or(h_e_mask, m_nan);
		const Ty h_e_norm_overflow_offset  = simd_u32_add(f_e_half_bias, one);
		const Ty h_e_norm_overflow         = simd_x32_sll(h_e_norm_overflow_offset, kHalfExponentBitShift);
		const Ty h_e_norm                  = simd_x32_sll(f_e_half_bias, kHalfExponentBitShift);
		const Ty h_m_norm                  = simd_x32_srl(f_m_rounded, 13);
		const Ty h_em_norm                 = simd_or(h_e_norm, h_m_norm);
		const Ty is_h_ndenorm_msb          = simd_u32_sub(f_h_bias, f_e_amount);
		const Ty is_f_e_flagged_msb        = simd_u32_sub(f_h_e_biased, f_e_half_bias);
		const Ty is_h_denorm_msb           = simd_not(is_h_ndenorm_msb);
		const Ty is_f_m_eqz_msb            = simd_u32_sub(f_m, one);
		const Ty is_h_nan_eqz_msb          = simd_u32_sub(m_nan, one);
		const Ty is_f_inf_msb              = simd_and(is_f_e_flagged_msb, is_f_m_eqz_msb);
		const Ty is_f_nan_underflow_msb    = simd_and(is_f_e_flagged_msb, is_h_nan_eqz_msb);
		const Ty is_e_overflow_msb         = simd_u32_sub(h_e_mask_value, f_e_half_bias);
		const Ty is_h_inf_msb              = simd_or(is_e_overflow_msb, is_f_inf_msb);
		const Ty is_f_nsnan_msb            = simd_u32_sub(f_snan, f_snan_mask);
		const Ty is_m_norm_overflow_msb    = simd_i32_neg(f_m_rounded_overflow);
		const Ty is_f_snan_msb             = simd_not(is_f_nsnan_msb);

		const Ty h_em_overflow_result      = simd_sels(is_m_norm_overflow_msb, h_e_norm_overflow, h_em_norm); //
		const Ty h_em_nan_result           = simd_sels(is_f_e_flagged_msb, h_em_nan, h_em_overflow_result);
		const Ty h_em_nan_underflow_result = simd_sels(is_f_nan_underflow_msb, h_nan_min, h_em_nan_result);
		const Ty h_em_inf_result           = simd_sels(is_h_inf_msb, h_e_mask, h_em_nan_underflow_result);
		const Ty h_em_denorm_result        = simd_sels(is_h_denorm_msb, h_m_denorm, h_em_inf_result);
		const Ty h_em_snan_result          = simd_sels(is_f_snan_msb, h_snan_mask, h_em_denorm_result);

		const Ty result                    = simd_or(h_s, h_em_snan_result);
		return result;
	}

	template<typename Ty>
	BX_SIMD_INLINE Ty simd_f16_tof32_ni(Ty _a)
	{
		const Ty h_e_mask             = simd_splat<Ty>(uint32_t(kHalfExponentMask) );
		const Ty h_m_mask             = simd_splat<Ty>(uint32_t(kHalfMantissaMask) );
		const Ty h_s_mask             = simd_splat<Ty>(uint32_t(kHalfSignMask) );
		const Ty h_f_bias_offset      = simd_splat<Ty>(uint32_t(0x0001c000) );
		const Ty f_e_mask             = simd_splat<Ty>(kFloatExponentMask);
		const Ty f_m_mask             = simd_splat<Ty>(kFloatMantissaMask);
		const Ty h_f_e_denorm_bias    = simd_splat<Ty>(uint32_t(0x0000007e) );
		const Ty h_f_m_denorm_sa_bias = simd_splat<Ty>(uint32_t(0x00000008) );
		const Ty h_e_mask_minus_one   = simd_splat<Ty>(uint32_t(0x00007bff) );
		const Ty one                  = simd_splat<Ty>(uint32_t(1) );

		const Ty h_e                  = simd_and(_a, h_e_mask); //
		const Ty h_m                  = simd_and(_a, h_m_mask);
		const Ty h_s                  = simd_and(_a, h_s_mask);
		const Ty h_e_f_bias           = simd_u32_add(h_e, h_f_bias_offset);
		const Ty h_m_nlz              = simd_u32_cntlz_ni(h_m);
		const Ty f_s                  = simd_x32_sll(h_s, 16);
		const Ty f_e                  = simd_x32_sll(h_e_f_bias, 13);
		const Ty f_m                  = simd_x32_sll(h_m, 13);
		const Ty f_em                 = simd_or(f_e, f_m);
		const Ty h_f_m_sa             = simd_u32_sub(h_m_nlz, h_f_m_denorm_sa_bias);
		const Ty f_e_denorm_unpacked  = simd_u32_sub(h_f_e_denorm_bias, h_f_m_sa);
		const Ty h_f_m                = simd_x32_sll_ni(h_m, h_f_m_sa);
		const Ty f_m_denorm           = simd_and(h_f_m, f_m_mask);
		const Ty f_e_denorm           = simd_x32_sll(f_e_denorm_unpacked, kFloatExponentBitShift);
		const Ty f_em_denorm          = simd_or(f_e_denorm, f_m_denorm);
		const Ty f_em_nan             = simd_or(f_e_mask, f_m);
		const Ty is_e_eqz_msb         = simd_u32_sub(h_e, one);
		const Ty is_m_nez_msb         = simd_i32_neg(h_m);
		const Ty is_e_flagged_msb     = simd_u32_sub(h_e_mask_minus_one, h_e);
		const Ty is_zero_msb          = simd_andc(is_e_eqz_msb, is_m_nez_msb);
		const Ty is_inf_msb           = simd_andc(is_e_flagged_msb, is_m_nez_msb);
		const Ty is_denorm_msb        = simd_and(is_m_nez_msb, is_e_eqz_msb);
		const Ty is_nan_msb           = simd_and(is_e_flagged_msb, is_m_nez_msb);
		const Ty is_zero              = simd_x32_sra(is_zero_msb, 31);
		const Ty f_zero_result        = simd_andc(f_em, is_zero);

		const Ty f_denorm_result      = simd_sels(is_denorm_msb, f_em_denorm, f_zero_result); //
		const Ty f_inf_result         = simd_sels(is_inf_msb, f_e_mask, f_denorm_result);
		const Ty f_nan_result         = simd_sels(is_nan_msb, f_em_nan, f_inf_result);

		const Ty result               = simd_or(f_s, f_nan_result);
		return result;
	}

} // namespace bx
