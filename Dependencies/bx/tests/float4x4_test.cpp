/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/float4x4_t.h>
#include <bx/math.h>
#include <bx/string.h>

static void loadMtx(bx::float4x4_t& _dst, const float* _src)
{
	_dst.col[0] = bx::simd128_ld(_src[ 0], _src[ 1], _src[ 2], _src[ 3]);
	_dst.col[1] = bx::simd128_ld(_src[ 4], _src[ 5], _src[ 6], _src[ 7]);
	_dst.col[2] = bx::simd128_ld(_src[ 8], _src[ 9], _src[10], _src[11]);
	_dst.col[3] = bx::simd128_ld(_src[12], _src[13], _src[14], _src[15]);
}

static void storeMtx(float* _dst, const bx::float4x4_t& _src)
{
	bx::simd128_st(&_dst[ 0], _src.col[0]);
	bx::simd128_st(&_dst[ 4], _src.col[1]);
	bx::simd128_st(&_dst[ 8], _src.col[2]);
	bx::simd128_st(&_dst[12], _src.col[3]);
}

static void checkMtxEqual(const float* _result, const float* _ref, float _tolerance)
{
	for (uint32_t ii = 0; ii < 16; ++ii)
	{
		const float diff = bx::abs(_result[ii] - _ref[ii]);
		INFO("ii=" << ii << " result=" << _result[ii] << " ref=" << _ref[ii] << " diff=" << diff);
		CHECK(bx::isEqual(_result[ii], _ref[ii], _tolerance) );
	}
}

static const float s_projMtx[] =
{
	1.20629f,     0.0f,      0.0f,  0.0f,
	    0.0f, 2.14451f,      0.0f,  0.0f,
	    0.0f,     0.0f, -1.00001f, -1.0f,
	    0.0f,     0.0f, -2.00001f,  0.0f,
};

static const float s_modelMtx[] =
{
	 0.866025f,  0.5f,      0.0f, 0.0f,
	-0.5f,       0.866025f, 0.0f, 0.0f,
	 0.0f,       0.0f,      1.0f, 0.0f,
	 1.0f,       2.0f,      3.0f, 1.0f,
};

TEST_CASE("float4x4_inverse", "[float4x4]")
{
	bx::float4x4_t mtx;
	loadMtx(mtx, s_projMtx);

	bx::float4x4_t inv;
	bx::float4x4_inverse(&inv, &mtx);

	float ref[16];
	bx::mtxInverse(ref, s_projMtx);

	BX_ALIGN_DECL_16(float) result[16];
	storeMtx(result, inv);

	checkMtxEqual(result, ref, 0.0001f);
}

TEST_CASE("float4x4_mul", "[float4x4]")
{
	bx::float4x4_t a, b;
	loadMtx(a, s_modelMtx);
	loadMtx(b, s_projMtx);

	bx::float4x4_t r;
	bx::float4x4_mul(&r, &a, &b);

	float ref[16];
	bx::mtxMul(ref, s_modelMtx, s_projMtx);

	BX_ALIGN_DECL_16(float) result[16];
	storeMtx(result, r);

	checkMtxEqual(result, ref, 0.0001f);
}

TEST_CASE("float4x4_transpose", "[float4x4]")
{
	bx::float4x4_t mtx;
	loadMtx(mtx, s_projMtx);

	bx::float4x4_t tr;
	bx::float4x4_transpose(&tr, &mtx);

	float ref[16];
	bx::mtxTranspose(ref, s_projMtx);

	BX_ALIGN_DECL_16(float) result[16];
	storeMtx(result, tr);

	checkMtxEqual(result, ref, 0.0f);
}

TEST_CASE("model4x4_mul", "[float4x4]")
{
	bx::float4x4_t a, b;
	loadMtx(a, s_modelMtx);
	loadMtx(b, s_modelMtx);

	bx::float4x4_t r;
	bx::model4x4_mul(&r, &a, &b);

	float ref[16];
	bx::mtxMul(ref, s_modelMtx, s_modelMtx);

	BX_ALIGN_DECL_16(float) result[16];
	storeMtx(result, r);

	checkMtxEqual(result, ref, 0.0001f);
}

TEST_CASE("model4x4_mul_viewproj4x4", "[float4x4]")
{
	bx::float4x4_t model, viewProj;
	loadMtx(model, s_modelMtx);
	loadMtx(viewProj, s_projMtx);

	bx::float4x4_t r;
	bx::model4x4_mul_viewproj4x4(&r, &model, &viewProj);

	float ref[16];
	bx::mtxMul(ref, s_modelMtx, s_projMtx);

	BX_ALIGN_DECL_16(float) result[16];
	storeMtx(result, r);

	checkMtxEqual(result, ref, 0.0001f);
}

TEST_CASE("simd128_mul", "[float4x4]")
{
	bx::float4x4_t mtx;
	loadMtx(mtx, s_projMtx);

	const float vec[4] = { 1.5f, -2.25f, 3.75f, 0.5f };
	const bx::simd128_t v = bx::simd128_ld(vec[0], vec[1], vec[2], vec[3]);

	const bx::simd128_t r = bx::simd128_mul(v, &mtx);

	float ref[4];
	bx::vec4MulMtx(ref, vec, s_projMtx);

	BX_ALIGN_DECL_16(float) result[4];
	bx::simd128_st(result, r);

	for (uint32_t ii = 0; ii < 4; ++ii)
	{
		const float diff = bx::abs(result[ii] - ref[ii]);
		INFO("ii=" << ii << " result=" << result[ii] << " ref=" << ref[ii] << " diff=" << diff);
		CHECK(bx::isEqual(result[ii], ref[ii], 0.0001f) );
	}
}

TEST_CASE("simd128_mul_xyz1", "[float4x4]")
{
	bx::float4x4_t mtx;
	loadMtx(mtx, s_projMtx);

	const float vec[4] = { 1.5f, -2.25f, 3.75f, 1.0f };
	const bx::simd128_t v = bx::simd128_ld(vec[0], vec[1], vec[2], vec[3]);

	const bx::simd128_t r = bx::simd128_mul_xyz1(v, &mtx);

	float ref[4];
	bx::vec4MulMtx(ref, vec, s_projMtx);

	BX_ALIGN_DECL_16(float) result[4];
	bx::simd128_st(result, r);

	for (uint32_t ii = 0; ii < 4; ++ii)
	{
		const float diff = bx::abs(result[ii] - ref[ii]);
		INFO("ii=" << ii << " result=" << result[ii] << " ref=" << ref[ii] << " diff=" << diff);
		CHECK(bx::isEqual(result[ii], ref[ii], 0.0001f) );
	}
}
