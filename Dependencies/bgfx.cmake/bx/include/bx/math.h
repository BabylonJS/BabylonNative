/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_MATH_H_HEADER_GUARD
#define BX_MATH_H_HEADER_GUARD

#include "bx.h"

namespace bx
{
	/// Linear interpolation function pointer type.
	///
	/// @param _a Start value.
	/// @param _b End value.
	/// @param _t Interpolation factor [0, 1].
	///
	typedef float (*LerpFn)(float _a, float _b, float _t);

	/// Coordinate system handedness.
	struct Handedness
	{
		enum Enum
		{
			Left,  //!< Left-handed coordinate system.
			Right, //!< Right-handed coordinate system.
		};
	};

	/// Near/far plane convention for projection matrices.
	struct NearFar
	{
		enum Enum
		{
			Default, //!< Default near/far convention.
			Reverse, //!< Reversed depth (near=1, far=0).
		};
	};

	/// 3-component float vector.
	struct Vec3
	{
		Vec3() = delete;

		/// Construct uninitialized.
		Vec3(InitNoneTag);

		/// Construct with all components set to zero.
		constexpr Vec3(InitZeroTag);

		/// Construct with identity value (1, 0, 0).
		constexpr Vec3(InitIdentityTag);

		/// Construct with all components set to _v.
		///
		/// @param[in] _v Scalar value for all components.
		///
		explicit constexpr Vec3(float _v);

		/// Construct from individual components.
		///
		/// @param[in] _x X component.
		/// @param[in] _y Y component.
		/// @param[in] _z Z component.
		///
		constexpr Vec3(float _x, float _y, float _z);

		float x, y, z;
	};

	/// Plane defined by a normal vector and signed distance from origin.
	struct Plane
	{
		Plane() = delete;

		/// Construct uninitialized.
		Plane(InitNoneTag);

		/// Construct with zero normal and zero distance.
		constexpr Plane(InitZeroTag);

		/// Construct with identity value.
		constexpr Plane(InitIdentityTag);

		/// Construct from normal and distance.
		///
		/// @param[in] _normal Plane normal vector.
		/// @param[in] _dist Signed distance from origin along normal.
		///
		constexpr Plane(Vec3 _normal, float _dist);

		Vec3  normal;
		float dist;
	};

	/// Quaternion representing a rotation.
	struct Quaternion
	{
		Quaternion() = delete;

		/// Construct uninitialized.
		Quaternion(InitNoneTag);

		/// Construct with all components set to zero.
		constexpr Quaternion(InitZeroTag);

		/// Construct with identity rotation {0, 0, 0, 1}.
		constexpr Quaternion(InitIdentityTag);

		/// Construct from individual components.
		///
		/// @param[in] _x X component (imaginary i).
		/// @param[in] _y Y component (imaginary j).
		/// @param[in] _z Z component (imaginary k).
		/// @param[in] _w W component (real/scalar).
		///
		constexpr Quaternion(float _x, float _y, float _z, float _w);

		float x, y, z, w;
	};

	/// Returns converted the argument _deg to radians.
	///
	/// @param[in] _deg Angle in degrees.
	///
	/// @returns Angle in radians.
	///
	BX_CONSTEXPR_FUNC float toRad(float _deg);

	/// Returns converted the argument _rad to degrees.
	///
	/// @param[in] _rad Angle in radians.
	///
	/// @returns Angle in degrees.
	///
	BX_CONSTEXPR_FUNC float toDeg(float _rad);

	/// Reinterprets the bit pattern of _a as uint32_t.
	///
	/// @param[in] _a Float value.
	///
	/// @returns Bit pattern as uint32_t.
	///
	BX_CONSTEXPR_FUNC uint32_t floatToBits(float _a);

	/// Reinterprets the bit pattern of _a as float.
	///
	/// @param[in] _a Bit pattern.
	///
	/// @returns Float value.
	///
	BX_CONSTEXPR_FUNC float bitsToFloat(uint32_t _a);

	/// Reinterprets the bit pattern of _a as uint64_t.
	///
	/// @param[in] _a Double value.
	///
	/// @returns Bit pattern as uint64_t.
	///
	BX_CONSTEXPR_FUNC uint64_t doubleToBits(double _a);

	/// Reinterprets the bit pattern of _a as double.
	///
	/// @param[in] _a Bit pattern.
	///
	/// @returns Double value.
	///
	BX_CONSTEXPR_FUNC double bitsToDouble(uint64_t _a);

	/// Returns sortable floating point value.
	///
	/// @param[in] _value Float bits as uint32_t.
	///
	/// @returns Sortable uint32_t value.
	///
	BX_CONSTEXPR_FUNC uint32_t floatFlip(uint32_t _value);

	/// Returns true if _f is a number that is NaN.
	///
	/// @param[in] _f Float value.
	///
	/// @returns True if _f is NaN.
	///
	BX_CONSTEXPR_FUNC bool isNan(float _f);

	/// Returns true if _f is a number that is NaN.
	///
	/// @param[in] _f Double value.
	///
	/// @returns True if _f is NaN.
	///
	BX_CONSTEXPR_FUNC bool isNan(double _f);

	/// Returns true if _f is not infinite and is not a NaN.
	///
	/// @param[in] _f Float value.
	///
	/// @returns True if _f is finite.
	///
	BX_CONSTEXPR_FUNC bool isFinite(float _f);

	/// Returns true if _f is not infinite and is not a NaN.
	///
	/// @param[in] _f Double value.
	///
	/// @returns True if _f is finite.
	///
	BX_CONSTEXPR_FUNC bool isFinite(double _f);

	/// Returns true if _f is infinite and is not a NaN.
	///
	/// @param[in] _f Float value.
	///
	/// @returns True if _f is infinite.
	///
	BX_CONSTEXPR_FUNC bool isInfinite(float _f);

	/// Returns true if _f is infinite and is not a NaN.
	///
	/// @param[in] _f Double value.
	///
	/// @returns True if _f is infinite.
	///
	BX_CONSTEXPR_FUNC bool isInfinite(double _f);

	/// Returns the largest integer value not greater than _f.
	///
	/// @param[in] _f Input value.
	///
	/// @returns Floor of _f.
	///
	BX_CONSTEXPR_FUNC float floor(float _f);

	/// Returns the smallest integer value not less than _f.
	///
	/// @param[in] _f Input value.
	///
	/// @returns Ceiling of _f.
	///
	BX_CONSTEXPR_FUNC float ceil(float _f);

	/// Returns the nearest integer value to _f, rounding halfway cases away from zero.
	///
	/// @param[in] _f Input value.
	///
	/// @returns Rounded value.
	///
	BX_CONSTEXPR_FUNC float round(float _f);

	/// Returns linear interpolation between two values _a and _b.
	///
	/// @param[in] _a Start value.
	/// @param[in] _b End value.
	/// @param[in] _t Interpolation factor [0, 1].
	///
	/// @returns Interpolated value _a + (_b - _a) * _t.
	///
	BX_CONSTEXPR_FUNC float lerp(float _a, float _b, float _t);

	/// Returns inverse linear interpolation of _value between two values _a and _b.
	///
	/// @param[in] _a Start value.
	/// @param[in] _b End value.
	/// @param[in] _value Value to map.
	///
	/// @returns Factor t such that lerp(_a, _b, t) == _value.
	///
	BX_CONSTEXPR_FUNC float invLerp(float _a, float _b, float _value);

	/// Extracts the sign of value `_a`.
	///
	/// @param[in] _a Value.
	///
	/// @returns -1 if `_a` is less than zero, 0 if `_a` is equal to 0, or +1 if `_a` is greater than zero.
	///
	BX_CONSTEXPR_FUNC float sign(float _a);

	/// Returns `true` if the velue `_a` is negative.
	///
	/// @param[in] _a Value.
	///
	/// @returns `true` if `_a` is less than zero, otherwise returns `false`.
	///
	BX_CONSTEXPR_FUNC bool signBit(float _a);

	/// Returns value with the magnitude `_value`, and the sign of `_sign`.
	///
	/// @param[in] _value Value.
	/// @param[in] _sign Sign.
	///
	/// @returns Value with the magnitude `_value`, and the sign of `_sign`.
	///
	BX_CONSTEXPR_FUNC float copySign(float _value, float _sign);

	/// Returns the absolute of _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Absolute value of _a.
	///
	BX_CONSTEXPR_FUNC float abs(float _a);

	/// Returns the square of _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns _a * _a.
	///
	BX_CONSTEXPR_FUNC float square(float _a);

	/// Returns the both sine and cosine of the argument _a.
	///
	/// @param[out] _outSinApprox Output approximate sine value.
	/// @param[out] _outCos Output cosine value.
	/// @param[in] _a Angle in radians.
	///
	/// @remarks The function calculates cosine, and then approximates sine based on the cosine
	///   result. Therefore calculation of sine is less accurate than calling `bx::sin` function.
	///
	void sinCosApprox(float& _outSinApprox, float& _outCos, float _a);

	/// Returns the sine of the argument _a.
	///
	/// @param[in] _a Angle in radians.
	///
	/// @returns Sine of _a.
	///
	BX_CONSTEXPR_FUNC float sin(float _a);

	/// Returns hyperbolic sine of the argument _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Hyperbolic sine of _a.
	///
	BX_CONSTEXPR_FUNC float sinh(float _a);

	/// Returns radian angle between -pi/2 and +pi/2 whose sine is _a.
	///
	/// @param[in] _a Sine value [-1, 1].
	///
	/// @returns Arcsine of _a in radians.
	///
	BX_CONSTEXPR_FUNC float asin(float _a);

	/// Returns the cosine of the argument _a.
	///
	/// @param[in] _a Angle in radians.
	///
	/// @returns Cosine of _a.
	///
	BX_CONSTEXPR_FUNC float cos(float _a);

	/// Returns hyperbolic cosine of the argument _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Hyperbolic cosine of _a.
	///
	BX_CONSTEXPR_FUNC float cosh(float _a);

	/// Returns radian angle between 0 and pi whose cosine is _a.
	///
	/// @param[in] _a Cosine value [-1, 1].
	///
	/// @returns Arccosine of _a in radians.
	///
	BX_CONSTEXPR_FUNC float acos(float _a);

	/// Returns the circular tangent of the radian argument _a.
	///
	/// @param[in] _a Angle in radians.
	///
	/// @returns Tangent of _a.
	///
	BX_CONSTEXPR_FUNC float tan(float _a);

	/// Returns hyperbolic tangent of the argument _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Hyperbolic tangent of _a.
	///
	BX_CONSTEXPR_FUNC float tanh(float _a);

	/// Returns radian angle between -pi/2 and +pi/2 whose tangent is _a.
	///
	/// @param[in] _a Tangent value.
	///
	/// @returns Arctangent of _a in radians.
	///
	BX_CONSTEXPR_FUNC float atan(float _a);

	/// Returns the inverse tangent of _y/_x.
	///
	/// @param[in] _y Y component.
	/// @param[in] _x X component.
	///
	/// @returns Angle in radians [-pi, pi].
	///
	BX_CONSTEXPR_FUNC float atan2(float _y, float _x);

	/// Computes _a raised to the _b power.
	///
	/// @param[in] _a Base.
	/// @param[in] _b Exponent.
	///
	/// @returns _a^_b.
	///
	BX_CONSTEXPR_FUNC float pow(float _a, float _b);

	/// Returns the result of multiplying _a by 2 raised to the power of the exponent `_a * (2^_b)`.
	///
	/// @param[in] _a Significand.
	/// @param[in] _b Exponent.
	///
	/// @returns _a * 2^_b.
	///
	BX_CONSTEXPR_FUNC float ldexp(float _a, int32_t _b);

	/// Returns decomposed given floating point value _a into a normalized fraction and
	/// an integral power of two.
	///
	/// @param[in] _a Input value.
	/// @param[out] _outExp Output exponent.
	///
	/// @returns Normalized fraction in [0.5, 1.0).
	///
	float frexp(float _a, int32_t* _outExp);

	/// Returns e (2.71828...) raised to the _a power.
	///
	/// @param[in] _a Exponent.
	///
	/// @returns e^_a.
	///
	BX_CONSTEXPR_FUNC float exp(float _a);

	/// Returns 2 raised to the _a power.
	///
	/// @param[in] _a Exponent.
	///
	/// @returns 2^_a.
	///
	BX_CONSTEXPR_FUNC float exp2(float _a);

	/// Returns the base e (2.71828...) logarithm of _a.
	///
	/// @param[in] _a Input value (must be > 0).
	///
	/// @returns ln(_a).
	///
	BX_CONSTEXPR_FUNC float log(float _a);

	/// Returns the base 2 logarithm of _a.
	///
	/// @param[in] _a Input value (must be > 0).
	///
	/// @returns log2(_a).
	///
	BX_CONSTEXPR_FUNC float log2(float _a);

	/// Count number of bits set.
	///
	/// @param[in] _val Input value.
	///
	/// @returns Number of set bits (popcount).
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC uint8_t countBits(Ty _val);

	/// Count number of leading zeros.
	///
	/// @param[in] _val Input value.
	///
	/// @returns Number of leading zero bits.
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC uint8_t countLeadingZeros(Ty _val);

	/// Count number of trailing zeros.
	///
	/// @param[in] _val Input value.
	///
	/// @returns Number of trailing zero bits.
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC uint8_t countTrailingZeros(Ty _val);

	/// Find first set.
	///
	/// @param[in] _val Input value.
	///
	/// @returns 1-based index of the least significant set bit, or 0 if none.
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC uint8_t findFirstSet(Ty _val);

	/// Find last set.
	///
	/// @param[in] _val Input value.
	///
	/// @returns 1-based index of the most significant set bit, or 0 if none.
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC uint8_t findLastSet(Ty _val);

	/// Returns the next smallest integer base 2 logarithm of _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Ceiling of log2(_a).
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC uint8_t ceilLog2(Ty _a);

	/// Returns the next biggest integer base 2 logarithm of _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Floor of log2(_a).
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC uint8_t floorLog2(Ty _a);

	/// Returns the next smallest power of two value.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Smallest power of two >= _a.
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC Ty nextPow2(Ty _a);

	/// Returns the square root of _a.
	///
	/// @param[in] _a Input value (must be >= 0).
	///
	/// @returns Square root of _a.
	///
	BX_CONSTEXPR_FUNC float sqrt(float _a);

	/// Returns reciprocal square root of _a.
	///
	/// @param[in] _a Input value (must be > 0).
	///
	/// @returns 1 / sqrt(_a).
	///
	BX_CONSTEXPR_FUNC float rsqrt(float _a);

	/// Returns the nearest integer not greater in magnitude than _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Truncated value.
	///
	BX_CONSTEXPR_FUNC float trunc(float _a);

	/// Returns the fractional (or decimal) part of _a, which is greater than or equal to 0
	/// and less than 1.
	///
	/// @param[in] _a Input value.
	///
	/// @returns Fractional part of _a.
	///
	BX_CONSTEXPR_FUNC float fract(float _a);

	/// Returns result of negated multiply-sub operation -(_a * _b - _c) -> _c - _a * _b.
	///
	/// @param[in] _a First factor.
	/// @param[in] _b Second factor.
	/// @param[in] _c Value to subtract from.
	///
	/// @returns _c - _a * _b.
	///
	BX_CONSTEXPR_FUNC float nms(float _a, float _b, float _c);

	/// Returns result of addition (_a + _b).
	///
	/// @param[in] _a First value.
	/// @param[in] _b Second value.
	///
	/// @returns _a + _b.
	///
	BX_CONSTEXPR_FUNC float add(float _a, float _b);

	/// Returns result of subtracion (_a - _b).
	///
	/// @param[in] _a First value.
	/// @param[in] _b Second value.
	///
	/// @returns _a - _b.
	///
	BX_CONSTEXPR_FUNC float sub(float _a, float _b);

	/// Saturating integer add. Clamps the result to the representable range
	/// of `Ty` instead of wrapping around. Supports signed and unsigned
	/// integer types.
	///
	/// @param[in] _a Left operand.
	/// @param[in] _b Right operand.
	///
	/// @returns Sum clamped to `[LimitsT<Ty>::min, LimitsT<Ty>::max]`.
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC Ty satAdd(Ty _a, Ty _b);

	/// Saturating integer subtract. Clamps the result to the representable
	/// range of `Ty` instead of wrapping around. Supports signed and
	/// unsigned integer types.
	///
	/// @param[in] _a Left operand.
	/// @param[in] _b Right operand.
	///
	/// @returns Difference clamped to `[LimitsT<Ty>::min, LimitsT<Ty>::max]`.
	///
	template<typename Ty>
	BX_CONSTEXPR_FUNC Ty satSub(Ty _a, Ty _b);

	/// Returns result of multiply (_a * _b).
	///
	/// @param[in] _a First value.
	/// @param[in] _b Second value.
	///
	/// @returns _a * _b.
	///
	BX_CONSTEXPR_FUNC float mul(float _a, float _b);

	/// Returns result of multiply and add (_a * _b + _c).
	///
	/// @param[in] _a First factor.
	/// @param[in] _b Second factor.
	/// @param[in] _c Addend.
	///
	/// @returns _a * _b + _c.
	///
	BX_CONSTEXPR_FUNC float mad(float _a, float _b, float _c);

	/// Returns reciprocal of _a.
	///
	/// @param[in] _a Input value.
	///
	/// @returns 1 / _a.
	///
	BX_CONSTEXPR_FUNC float rcp(float _a);

	/// Returns reciprocal of _a. Avoids divide by zero.
	///
	/// @param[in] _a Input value.
	///
	/// @returns 1 / _a, safe from division by zero.
	///
	BX_CONSTEXPR_FUNC float rcpSafe(float _a);

	/// Returns result of division (_a / _b).
	///
	/// @param[in] _a Numerator.
	/// @param[in] _b Denominator.
	///
	/// @returns _a / _b.
	///
	BX_CONSTEXPR_FUNC float div(float _a, float _b);

	/// Returns result of division (_a / _b). Avoids divide by zero.
	///
	/// @param[in] _a Numerator.
	/// @param[in] _b Denominator.
	///
	/// @returns _a / _b, safe from division by zero.
	///
	BX_CONSTEXPR_FUNC float divSafe(float _a, float _b);

	/// Returns the floating-point remainder of the division operation _a/_b.
	///
	/// @param[in] _a Numerator.
	/// @param[in] _b Denominator.
	///
	/// @returns Remainder of _a / _b.
	///
	BX_CONSTEXPR_FUNC float mod(float _a, float _b);

	/// Returns true if _a and _b are equal within _epsilon tolerance.
	///
	/// @param[in] _a First value.
	/// @param[in] _b Second value.
	/// @param[in] _epsilon Tolerance.
	///
	/// @returns True if |_a - _b| < _epsilon.
	///
	BX_CONSTEXPR_FUNC bool isEqual(float _a, float _b, float _epsilon);

	/// Returns true if arrays _a and _b are element-wise equal within _epsilon tolerance.
	///
	/// @param[in] _a First array of floats.
	/// @param[in] _b Second array of floats.
	/// @param[in] _num Number of elements to compare.
	/// @param[in] _epsilon Tolerance.
	///
	/// @returns True if all elements are within _epsilon.
	///
	BX_CONST_FUNC bool isEqual(const float* _a, const float* _b, uint32_t _num, float _epsilon);

	/// Returns true if _v is approximately zero.
	///
	/// @param[in] _v Input value.
	///
	/// @returns True if _v is near zero.
	///
	BX_CONSTEXPR_FUNC bool isNearZero(float _v);

	/// Returns _a wrapped to [0, _wrap) range.
	///
	/// @param[in] _a Input value.
	/// @param[in] _wrap Wrap range.
	///
	/// @returns Wrapped value.
	///
	BX_CONSTEXPR_FUNC float wrap(float _a, float _wrap);

	/// Returns 0.0 if _a < _edge, otherwise 1.0.
	///
	/// @param[in] _edge Step edge threshold.
	/// @param[in] _a Input value.
	///
	/// @returns Step result (0.0 or 1.0).
	///
	BX_CONSTEXPR_FUNC float step(float _edge, float _a);

	/// Returns 1.0 if _a is within [_start, _end), otherwise 0.0.
	///
	/// @param[in] _a Input value.
	/// @param[in] _start Pulse start.
	/// @param[in] _end Pulse end.
	///
	/// @returns Pulse result (0.0 or 1.0).
	///
	BX_CONSTEXPR_FUNC float pulse(float _a, float _start, float _end);

	/// Returns smooth Hermite interpolation for _a in [0, 1] range.
	///
	/// @param[in] _a Input value [0, 1].
	///
	/// @returns Smooth-stepped value.
	///
	BX_CONSTEXPR_FUNC float smoothStep(float _a);

	/// Returns inverse of smoothStep.
	///
	/// @param[in] _a Input value [0, 1].
	///
	/// @returns Inverse smooth-stepped value.
	///
	BX_CONSTEXPR_FUNC float invSmoothStep(float _a);

	/// Returns bias curve: _time^(log(_bias)/log(0.5)).
	///
	/// @param[in] _time Input value [0, 1].
	/// @param[in] _bias Bias factor.
	///
	/// @returns Biased value.
	///
	BX_CONSTEXPR_FUNC float bias(float _time, float _bias);

	/// Returns gain curve using bias.
	///
	/// @param[in] _time Input value [0, 1].
	/// @param[in] _gain Gain factor.
	///
	/// @returns Gained value.
	///
	BX_CONSTEXPR_FUNC float gain(float _time, float _gain);

	/// Returns the smallest signed angle difference between _a and _b.
	///
	/// @param[in] _a First angle in radians.
	/// @param[in] _b Second angle in radians.
	///
	/// @returns Signed angle difference in [-pi, pi].
	///
	BX_CONSTEXPR_FUNC float angleDiff(float _a, float _b);

	/// Returns shortest distance linear interpolation between two angles.
	///
	/// @param[in] _a Start angle in radians.
	/// @param[in] _b End angle in radians.
	/// @param[in] _t Interpolation factor [0, 1].
	///
	/// @returns Interpolated angle in radians.
	///
	BX_CONSTEXPR_FUNC float angleLerp(float _a, float _b, float _t);

	/// Loads a value of type Ty from memory.
	///
	/// @param[in] _ptr Pointer to source data.
	///
	/// @returns Loaded value.
	///
	template<typename Ty>
	Ty load(const void* _ptr);

	/// Stores a value of type Ty to memory.
	///
	/// @param[out] _ptr Pointer to destination.
	/// @param[in] _a Value to store.
	///
	template<typename Ty>
	void store(void* _ptr, const Ty& _a);

	/// Returns component-wise rounded value of _a.
	///
	/// @param[in] _a Input vector.
	///
	/// @returns Component-wise rounded vector.
	///
	BX_CONSTEXPR_FUNC Vec3 round(const Vec3& _a);

	/// Returns component-wise absolute value of _a.
	///
	/// @param[in] _a Input vector.
	///
	/// @returns Component-wise absolute value vector.
	///
	BX_CONSTEXPR_FUNC Vec3 abs(const Vec3& _a);

	/// Returns negated vector _a.
	///
	/// @param[in] _a Input vector.
	///
	/// @returns Negated vector (-_a).
	///
	BX_CONSTEXPR_FUNC Vec3 neg(const Vec3& _a);

	/// Returns component-wise addition of _a and _b.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector.
	///
	/// @returns Result of (_a + _b).
	///
	BX_CONSTEXPR_FUNC Vec3 add(const Vec3& _a, const Vec3& _b);

	/// Returns vector _a with scalar _b added to each component.
	///
	/// @param[in] _a Input vector.
	/// @param[in] _b Scalar value.
	///
	/// @returns Result of (_a + _b).
	///
	BX_CONSTEXPR_FUNC Vec3 add(const Vec3& _a, float _b);

	/// Returns component-wise subtraction of _b from _a.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector.
	///
	/// @returns Result of (_a - _b).
	///
	BX_CONSTEXPR_FUNC Vec3 sub(const Vec3& _a, const Vec3& _b);

	/// Returns vector _a with scalar _b subtracted from each component.
	///
	/// @param[in] _a Input vector.
	/// @param[in] _b Scalar value.
	///
	/// @returns Result of (_a - _b).
	///
	BX_CONSTEXPR_FUNC Vec3 sub(const Vec3& _a, float _b);

	/// Returns component-wise multiplication of _a and _b.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector.
	///
	/// @returns Result of (_a * _b).
	///
	BX_CONSTEXPR_FUNC Vec3 mul(const Vec3& _a, const Vec3& _b);

	/// Returns vector _a scaled by _b.
	///
	/// @param[in] _a Input vector.
	/// @param[in] _b Scalar value.
	///
	/// @returns Result of (_a * _b).
	///
	BX_CONSTEXPR_FUNC Vec3 mul(const Vec3& _a, float _b);

	/// Returns component-wise division of _a by _b.
	///
	/// @param[in] _a Numerator vector.
	/// @param[in] _b Denominator vector.
	///
	/// @returns Result of (_a / _b).
	///
	BX_CONSTEXPR_FUNC Vec3 div(const Vec3& _a, const Vec3& _b);

	/// Returns component-wise division of _a by _b. Avoids divide by zero.
	///
	/// @param[in] _a Numerator vector.
	/// @param[in] _b Denominator vector.
	///
	/// @returns Result of (_a / _b), safe from division by zero.
	///
	BX_CONSTEXPR_FUNC Vec3 divSafe(const Vec3& _a, const Vec3& _b);

	/// Returns vector _a divided by scalar _b.
	///
	/// @param[in] _a Numerator vector.
	/// @param[in] _b Denominator scalar.
	///
	/// @returns Result of (_a / _b).
	///
	BX_CONSTEXPR_FUNC Vec3 div(const Vec3& _a, float _b);

	/// Returns vector _a divided by scalar _b. Avoids divide by zero.
	///
	/// @param[in] _a Numerator vector.
	/// @param[in] _b Denominator scalar.
	///
	/// @returns Result of (_a / _b), safe from division by zero.
	///
	BX_CONSTEXPR_FUNC Vec3 divSafe(const Vec3& _a, float _b);

	/// Returns result of negated multiply-sub operation -(_a * _b - _c) -> _c - _a * _b.
	///
	/// @param[in] _a Input vector.
	/// @param[in] _b Scalar multiplier.
	/// @param[in] _c Subtrahend vector.
	///
	/// @returns _c - _a * _b.
	///
	BX_CONSTEXPR_FUNC Vec3 nms(const Vec3& _a, const float _b, const Vec3& _c);

	/// Returns result of negated multiply-sub operation -(_a * _b - _c) -> _c - _a * _b.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector (multiplier).
	/// @param[in] _c Subtrahend vector.
	///
	/// @returns _c - _a * _b.
	///
	BX_CONSTEXPR_FUNC Vec3 nms(const Vec3& _a, const Vec3& _b, const Vec3& _c);

	/// Returns component-wise multiply and add: _a * _b + _c.
	///
	/// @param[in] _a Input vector.
	/// @param[in] _b Scalar multiplier.
	/// @param[in] _c Addend vector.
	///
	/// @returns Result of (_a * _b + _c).
	///
	BX_CONSTEXPR_FUNC Vec3 mad(const Vec3& _a, const float _b, const Vec3& _c);

	/// Returns component-wise multiply and add: _a * _b + _c.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector (multiplier).
	/// @param[in] _c Addend vector.
	///
	/// @returns Result of (_a * _b + _c).
	///
	BX_CONSTEXPR_FUNC Vec3 mad(const Vec3& _a, const Vec3& _b, const Vec3& _c);

	/// Returns dot product of _a and _b.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector.
	///
	/// @returns Dot product (_a . _b).
	///
	BX_CONSTEXPR_FUNC float dot(const Vec3& _a, const Vec3& _b);

	/// Returns cross product of _a and _b.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector.
	///
	/// @returns Cross product (_a x _b).
	///
	BX_CONSTEXPR_FUNC Vec3 cross(const Vec3& _a, const Vec3& _b);

	/// Returns length (magnitude) of vector _a.
	///
	/// @param[in] _a Input vector.
	///
	/// @returns Length of _a.
	///
	BX_CONSTEXPR_FUNC float length(const Vec3& _a);

	/// Returns squared distance between _a and _b.
	///
	/// @param[in] _a First point.
	/// @param[in] _b Second point.
	///
	/// @returns Squared Euclidean distance.
	///
	BX_CONSTEXPR_FUNC float distanceSq(const Vec3& _a, const Vec3& _b);

	/// Returns distance between _a and _b.
	///
	/// @param[in] _a First point.
	/// @param[in] _b Second point.
	///
	/// @returns Euclidean distance.
	///
	BX_CONSTEXPR_FUNC float distance(const Vec3& _a, const Vec3& _b);

	/// Returns linear interpolation between _a and _b by scalar factor _t.
	///
	/// @param[in] _a Start vector.
	/// @param[in] _b End vector.
	/// @param[in] _t Interpolation factor [0, 1].
	///
	/// @returns Interpolated vector.
	///
	BX_CONSTEXPR_FUNC Vec3 lerp(const Vec3& _a, const Vec3& _b, float _t);

	/// Returns component-wise linear interpolation between _a and _b.
	///
	/// @param[in] _a Start vector.
	/// @param[in] _b End vector.
	/// @param[in] _t Per-component interpolation factors [0, 1].
	///
	/// @returns Interpolated vector.
	///
	BX_CONSTEXPR_FUNC Vec3 lerp(const Vec3& _a, const Vec3& _b, const Vec3& _t);

	/// Returns normalized (unit length) vector _a.
	///
	/// @param[in] _a Input vector.
	///
	/// @returns Unit vector in the direction of _a.
	///
	BX_CONSTEXPR_FUNC Vec3 normalize(const Vec3& _a);

	/// Returns component-wise minimum of _a and _b.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector.
	///
	/// @returns Component-wise minimum vector.
	///
	BX_CONSTEXPR_FUNC Vec3 min(const Vec3& _a, const Vec3& _b);

	/// Returns component-wise maximum of _a and _b.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector.
	///
	/// @returns Component-wise maximum vector.
	///
	BX_CONSTEXPR_FUNC Vec3 max(const Vec3& _a, const Vec3& _b);

	/// Returns component wise reciprocal of _a.
	///
	/// @param[in] _a Input vector.
	///
	/// @returns Component-wise 1 / _a.
	///
	BX_CONSTEXPR_FUNC Vec3 rcp(const Vec3& _a);

	/// Returns component wise reciprocal of _a. Avoids divide by zero.
	///
	/// @param[in] _a Input vector.
	///
	/// @returns Component-wise 1 / _a, safe from division by zero.
	///
	BX_CONSTEXPR_FUNC Vec3 rcpSafe(const Vec3& _a);

	/// Returns true if vectors _a and _b are equal within _epsilon tolerance.
	///
	/// @param[in] _a First vector.
	/// @param[in] _b Second vector.
	/// @param[in] _epsilon Tolerance.
	///
	/// @returns True if all components are within _epsilon.
	///
	BX_CONSTEXPR_FUNC bool isEqual(const Vec3& _a, const Vec3& _b, float _epsilon);

	/// Returns true if vector _v is approximately zero.
	///
	/// @param[in] _v Input vector.
	///
	/// @returns True if all components are near zero.
	///
	BX_CONSTEXPR_FUNC bool isNearZero(const Vec3& _v);

	/// Calculates tangent and bitangent vectors from a normal.
	///
	/// @param[out] _outT Output tangent vector.
	/// @param[out] _outB Output bitangent vector.
	/// @param[in] _n Input normal vector.
	///
	void calcTangentFrame(Vec3& _outT, Vec3& _outB, const Vec3& _n);

	/// Calculates tangent and bitangent vectors from a normal, rotated by _angle.
	///
	/// @param[out] _outT Output tangent vector.
	/// @param[out] _outB Output bitangent vector.
	/// @param[in] _n Input normal vector.
	/// @param[in] _angle Rotation angle in radians.
	///
	void calcTangentFrame(Vec3& _outT, Vec3& _outB, const Vec3& _n, float _angle);

	/// Converts lat/long UV coordinates to a direction vector.
	///
	/// @param[in] _u Horizontal coordinate [0, 1].
	/// @param[in] _v Vertical coordinate [0, 1].
	///
	/// @returns Unit direction vector.
	///
	BX_CONSTEXPR_FUNC Vec3 fromLatLong(float _u, float _v);

	/// Converts a direction vector to lat/long UV coordinates.
	///
	/// @param[out] _outU Output horizontal coordinate [0, 1].
	/// @param[out] _outV Output vertical coordinate [0, 1].
	/// @param[in] _dir Input direction vector.
	///
	void toLatLong(float* _outU, float* _outV, const Vec3& _dir);

	/// Returns the negation of a quaternion (all components negated).
	/// The negated quaternion -q represents the same rotation as q.
	///
	/// @param[in] _a Input quaternion.
	///
	/// @returns Negated quaternion {-x, -y, -z, -w}.
	///
	BX_CONSTEXPR_FUNC Quaternion neg(const Quaternion& _a);

	/// Returns the conjugate of a quaternion (negated imaginary part).
	///
	/// @param[in] _a Input quaternion.
	///
	/// @returns Conjugated quaternion {-x, -y, -z, w}.
	///
	BX_CONSTEXPR_FUNC Quaternion conjugate(const Quaternion& _a);

	/// Returns the xyz part of quaternion multiplication _a * _b.
	///
	/// @param[in] _a First quaternion.
	/// @param[in] _b Second quaternion.
	///
	/// @returns Vec3 xyz components of the quaternion product.
	///
	BX_CONSTEXPR_FUNC Vec3 mulXyz(const Quaternion& _a, const Quaternion& _b);

	/// Returns component-wise addition of quaternions _a and _b.
	///
	/// @param[in] _a First quaternion.
	/// @param[in] _b Second quaternion.
	///
	/// @returns Sum quaternion (_a + _b).
	///
	BX_CONSTEXPR_FUNC Quaternion add(const Quaternion& _a, const Quaternion& _b);

	/// Returns component-wise subtraction of quaternion _b from _a.
	///
	/// @param[in] _a First quaternion.
	/// @param[in] _b Second quaternion.
	///
	/// @returns Difference quaternion (_a - _b).
	///
	BX_CONSTEXPR_FUNC Quaternion sub(const Quaternion& _a, const Quaternion& _b);

	/// Returns quaternion _a scaled by _b.
	///
	/// @param[in] _a Input quaternion.
	/// @param[in] _b Scalar value.
	///
	/// @returns Scaled quaternion (_a * _b).
	///
	BX_CONSTEXPR_FUNC Quaternion mul(const Quaternion& _a, float _b);

	/// Returns Hamilton product of quaternions _a and _b.
	///
	/// @param[in] _a First quaternion.
	/// @param[in] _b Second quaternion.
	///
	/// @returns Quaternion product (_a * _b).
	///
	BX_CONSTEXPR_FUNC Quaternion mul(const Quaternion& _a, const Quaternion& _b);

	/// Rotates vector _v by quaternion _q. Applies forward rotation: q * v * q*.
	///
	/// @param[in] _v Input vector.
	/// @param[in] _q Rotation quaternion.
	///
	/// @returns Rotated vector.
	///
	BX_CONSTEXPR_FUNC Vec3 mul(const Vec3& _v, const Quaternion& _q);

	/// Returns dot product of quaternions _a and _b.
	///
	/// @param[in] _a First quaternion.
	/// @param[in] _b Second quaternion.
	///
	/// @returns Dot product.
	///
	BX_CONSTEXPR_FUNC float dot(const Quaternion& _a, const Quaternion& _b);

	/// Returns normalized (unit length) quaternion _a.
	///
	/// @param[in] _a Input quaternion.
	///
	/// @returns Unit quaternion.
	///
	BX_CONSTEXPR_FUNC Quaternion normalize(const Quaternion& _a);

	/// Returns linear interpolation between quaternions _a and _b.
	///
	/// @param[in] _a Start quaternion.
	/// @param[in] _b End quaternion.
	/// @param[in] _t Interpolation factor [0, 1].
	///
	/// @returns Interpolated quaternion (not normalized).
	///
	BX_CONSTEXPR_FUNC Quaternion lerp(const Quaternion& _a, const Quaternion& _b, float _t);

	/// Returns spherical linear interpolation between quaternions _a and _b.
	/// Interpolates along the shortest arc on the unit quaternion hypersphere.
	///
	/// @param[in] _a Start quaternion (should be normalized).
	/// @param[in] _b End quaternion (should be normalized).
	/// @param[in] _t Interpolation factor [0, 1].
	///
	/// @returns Interpolated unit quaternion.
	///
	BX_CONST_FUNC Quaternion slerp(const Quaternion& _a, const Quaternion& _b, float _t);

	/// Creates a quaternion from Euler angles (pitch, yaw, roll).
	///
	/// @param[in] _euler Euler angles {pitch, yaw, roll} in radians.
	///
	/// @returns Rotation quaternion.
	///
	BX_CONST_FUNC Quaternion fromEuler(const Vec3& _euler);

	/// Converts quaternion to Euler angles (pitch, yaw, roll).
	///
	/// @param[in] _a Input quaternion.
	///
	/// @returns Euler angles {pitch, yaw, roll} in radians.
	///
	BX_CONST_FUNC Vec3 toEuler(const Quaternion& _a);

	/// Extracts the local X axis from quaternion _a.
	///
	/// @param[in] _a Input quaternion.
	///
	/// @returns X basis vector of the rotation.
	///
	BX_CONST_FUNC Vec3 toXAxis(const Quaternion& _a);

	/// Extracts the local Y axis from quaternion _a.
	///
	/// @param[in] _a Input quaternion.
	///
	/// @returns Y basis vector of the rotation.
	///
	BX_CONST_FUNC Vec3 toYAxis(const Quaternion& _a);

	/// Extracts the local Z axis from quaternion _a.
	///
	/// @param[in] _a Input quaternion.
	///
	/// @returns Z basis vector of the rotation.
	///
	BX_CONST_FUNC Vec3 toZAxis(const Quaternion& _a);

	/// Creates a quaternion from an axis-angle rotation.
	///
	/// @param[in] _axis Rotation axis (should be normalized).
	/// @param[in] _angle Rotation angle in radians.
	///
	/// @returns Rotation quaternion.
	///
	BX_CONST_FUNC Quaternion fromAxisAngle(const Vec3& _axis, float _angle);

	/// Converts quaternion to axis-angle representation.
	///
	/// @param[out] _outAxis Output rotation axis.
	/// @param[out] _outAngle Output rotation angle in radians.
	/// @param[in] _a Input quaternion.
	///
	void toAxisAngle(Vec3& _outAxis, float& _outAngle, const Quaternion& _a);

	/// Creates a quaternion representing rotation around the X axis.
	///
	/// @param[in] _ax Rotation angle in radians.
	///
	/// @returns Rotation quaternion.
	///
	BX_CONST_FUNC Quaternion rotateX(float _ax);

	/// Creates a quaternion representing rotation around the Y axis.
	///
	/// @param[in] _ay Rotation angle in radians.
	///
	/// @returns Rotation quaternion.
	///
	BX_CONST_FUNC Quaternion rotateY(float _ay);

	/// Creates a quaternion representing rotation around the Z axis.
	///
	/// @param[in] _az Rotation angle in radians.
	///
	/// @returns Rotation quaternion.
	///
	BX_CONST_FUNC Quaternion rotateZ(float _az);

	/// Returns true if quaternions _a and _b are equal within _epsilon tolerance.
	///
	/// @param[in] _a First quaternion.
	/// @param[in] _b Second quaternion.
	/// @param[in] _epsilon Tolerance.
	///
	/// @returns True if all components are within _epsilon.
	///
	BX_CONSTEXPR_FUNC bool isEqual(const Quaternion& _a, const Quaternion& _b, float _epsilon);

	/// Sets _result to a 4x4 identity matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	///
	void mtxIdentity(float* _result);

	/// Sets _result to a translation matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _tx Translation along X.
	/// @param[in] _ty Translation along Y.
	/// @param[in] _tz Translation along Z.
	///
	void mtxTranslate(float* _result, float _tx, float _ty, float _tz);

	/// Sets _result to a non-uniform scale matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _sx Scale along X.
	/// @param[in] _sy Scale along Y.
	/// @param[in] _sz Scale along Z.
	///
	void mtxScale(float* _result, float _sx, float _sy, float _sz);

	/// Sets _result to a uniform scale matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _scale Uniform scale factor.
	///
	void mtxScale(float* _result, float _scale);

	/// Sets _result to a transform matrix oriented along _normal.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _normal Up direction for the transform.
	/// @param[in] _scale Uniform scale factor.
	/// @param[in] _pos Translation position.
	///
	void mtxFromNormal(
		  float* _result
		, const Vec3& _normal
		, float _scale
		, const Vec3& _pos
		);

	/// Sets _result to a transform matrix oriented along _normal, with rotation.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _normal Up direction for the transform.
	/// @param[in] _scale Uniform scale factor.
	/// @param[in] _pos Translation position.
	/// @param[in] _angle Rotation angle around _normal in radians.
	///
	void mtxFromNormal(
		  float* _result
		, const Vec3& _normal
		, float _scale
		, const Vec3& _pos
		, float _angle
		);

	/// Sets _result to a rotation matrix from a quaternion.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _rotation Input quaternion.
	///
	void mtxFromQuaternion(float* _result, const Quaternion& _rotation);

	/// Sets _result to an inverse rigid-body transform matrix from rotation and translation.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _rotation Input quaternion.
	/// @param[in] _translation Input translation.
	///
	void mtxFromQuaternion(float* _result, const Quaternion& _rotation, const Vec3& _translation);

	/// Sets _result to a look-at view matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _eye Eye (camera) position.
	/// @param[in] _at Target position to look at.
	/// @param[in] _up Up direction vector.
	/// @param[in] _handedness Coordinate system handedness.
	///
	void mtxLookAt(
		  float* _result
		, const Vec3& _eye
		, const Vec3& _at
		, const Vec3& _up = { 0.0f, 1.0f, 0.0f }
		, Handedness::Enum _handedness = Handedness::Left
		);

	/// Sets _result to a perspective projection matrix from asymmetric frustum bounds.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _ut Tangent of the top frustum angle.
	/// @param[in] _dt Tangent of the bottom frustum angle.
	/// @param[in] _lt Tangent of the left frustum angle.
	/// @param[in] _rt Tangent of the right frustum angle.
	/// @param[in] _near Near clipping plane distance.
	/// @param[in] _far Far clipping plane distance.
	/// @param[in] _homogeneousNdc True for [-1,1] NDC (OpenGL), false for [0,1] (D3D).
	/// @param[in] _handedness Coordinate system handedness.
	///
	void mtxProj(
		  float* _result
		, float _ut
		, float _dt
		, float _lt
		, float _rt
		, float _near
		, float _far
		, bool _homogeneousNdc
		, Handedness::Enum _handedness = Handedness::Left
		);

	/// Sets _result to a perspective projection matrix from a 4-component FOV array.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _fov Four-component FOV {up, down, left, right} tangents.
	/// @param[in] _near Near clipping plane distance.
	/// @param[in] _far Far clipping plane distance.
	/// @param[in] _homogeneousNdc True for [-1,1] NDC (OpenGL), false for [0,1] (D3D).
	/// @param[in] _handedness Coordinate system handedness.
	///
	void mtxProj(
		  float* _result
		, const float _fov[4]
		, float _near
		, float _far
		, bool _homogeneousNdc
		, Handedness::Enum _handedness = Handedness::Left
		);

	/// Sets _result to a perspective projection matrix from vertical FOV and aspect ratio.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _fovy Vertical field of view in radians.
	/// @param[in] _aspect Aspect ratio (width/height).
	/// @param[in] _near Near clipping plane distance.
	/// @param[in] _far Far clipping plane distance.
	/// @param[in] _homogeneousNdc True for [-1,1] NDC (OpenGL), false for [0,1] (D3D).
	/// @param[in] _handedness Coordinate system handedness.
	///
	void mtxProj(
		  float* _result
		, float _fovy
		, float _aspect
		, float _near
		, float _far
		, bool _homogeneousNdc
		, Handedness::Enum _handedness = Handedness::Left
		);

	/// Sets _result to an infinite far plane projection matrix from a 4-component FOV array.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _fov Four-component FOV {up, down, left, right} tangents.
	/// @param[in] _near Near clipping plane distance.
	/// @param[in] _homogeneousNdc True for [-1,1] NDC (OpenGL), false for [0,1] (D3D).
	/// @param[in] _handedness Coordinate system handedness.
	/// @param[in] _nearFar Near/far plane convention.
	///
	void mtxProjInf(
		  float* _result
		, const float _fov[4]
		, float _near
		, bool _homogeneousNdc
		, Handedness::Enum _handedness = Handedness::Left
		, NearFar::Enum _nearFar = NearFar::Default
		);

	/// Sets _result to an infinite far plane projection matrix from asymmetric frustum bounds.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _ut Tangent of the top frustum angle.
	/// @param[in] _dt Tangent of the bottom frustum angle.
	/// @param[in] _lt Tangent of the left frustum angle.
	/// @param[in] _rt Tangent of the right frustum angle.
	/// @param[in] _near Near clipping plane distance.
	/// @param[in] _homogeneousNdc True for [-1,1] NDC (OpenGL), false for [0,1] (D3D).
	/// @param[in] _handedness Coordinate system handedness.
	/// @param[in] _nearFar Near/far plane convention.
	///
	void mtxProjInf(
		  float* _result
		, float _ut
		, float _dt
		, float _lt
		, float _rt
		, float _near
		, bool _homogeneousNdc
		, Handedness::Enum _handedness = Handedness::Left
		, NearFar::Enum _nearFar = NearFar::Default
		);

	/// Sets _result to an infinite far plane projection matrix from vertical FOV and aspect ratio.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _fovy Vertical field of view in radians.
	/// @param[in] _aspect Aspect ratio (width/height).
	/// @param[in] _near Near clipping plane distance.
	/// @param[in] _homogeneousNdc True for [-1,1] NDC (OpenGL), false for [0,1] (D3D).
	/// @param[in] _handedness Coordinate system handedness.
	/// @param[in] _nearFar Near/far plane convention.
	///
	void mtxProjInf(
		  float* _result
		, float _fovy
		, float _aspect
		, float _near
		, bool _homogeneousNdc
		, Handedness::Enum _handedness = Handedness::Left
		, NearFar::Enum _nearFar = NearFar::Default
		);

	/// Sets _result to an orthographic projection matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _left Left clipping plane.
	/// @param[in] _right Right clipping plane.
	/// @param[in] _bottom Bottom clipping plane.
	/// @param[in] _top Top clipping plane.
	/// @param[in] _near Near clipping plane distance.
	/// @param[in] _far Far clipping plane distance.
	/// @param[in] _offset Depth offset.
	/// @param[in] _homogeneousNdc True for [-1,1] NDC (OpenGL), false for [0,1] (D3D).
	/// @param[in] _handedness Coordinate system handedness.
	///
	void mtxOrtho(
		  float* _result
		, float _left
		, float _right
		, float _bottom
		, float _top
		, float _near
		, float _far
		, float _offset
		, bool _homogeneousNdc
		, Handedness::Enum _handedness = Handedness::Left
		);

	/// Sets _result to a rotation matrix around the X axis.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _ax Rotation angle in radians.
	///
	void mtxRotateX(float* _result, float _ax);

	/// Sets _result to a rotation matrix around the Y axis.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _ay Rotation angle in radians.
	///
	void mtxRotateY(float* _result, float _ay);

	/// Sets _result to a rotation matrix around the Z axis.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _az Rotation angle in radians.
	///
	void mtxRotateZ(float* _result, float _az);

	/// Sets _result to a combined X then Y rotation matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _ax X-axis rotation angle in radians.
	/// @param[in] _ay Y-axis rotation angle in radians.
	///
	void mtxRotateXY(float* _result, float _ax, float _ay);

	/// Sets _result to a combined X, Y, then Z rotation matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _ax X-axis rotation angle in radians.
	/// @param[in] _ay Y-axis rotation angle in radians.
	/// @param[in] _az Z-axis rotation angle in radians.
	///
	void mtxRotateXYZ(float* _result, float _ax, float _ay, float _az);

	/// Sets _result to a combined Z, Y, then X rotation matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _ax X-axis rotation angle in radians.
	/// @param[in] _ay Y-axis rotation angle in radians.
	/// @param[in] _az Z-axis rotation angle in radians.
	///
	void mtxRotateZYX(float* _result, float _ax, float _ay, float _az);

	/// Sets _result to a scale-rotate-translate matrix (rotation order ZYX, matching fromEuler).
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _sx Scale along X.
	/// @param[in] _sy Scale along Y.
	/// @param[in] _sz Scale along Z.
	/// @param[in] _ax X-axis rotation angle in radians.
	/// @param[in] _ay Y-axis rotation angle in radians.
	/// @param[in] _az Z-axis rotation angle in radians.
	/// @param[in] _tx Translation along X.
	/// @param[in] _ty Translation along Y.
	/// @param[in] _tz Translation along Z.
	///
	void mtxSRT(
		  float* _result
		, float _sx
		, float _sy
		, float _sz
		, float _ax
		, float _ay
		, float _az
		, float _tx
		, float _ty
		, float _tz
		);

	/// Multiplies row vector _vec by 4x4 matrix _mat. The w component is taken from _mat[12..14].
	///
	/// @param[in] _vec Input vector.
	/// @param[in] _mat Input 4x4 matrix (16 floats).
	///
	/// @returns Transformed vector.
	///
	Vec3 mul(const Vec3& _vec, const float* _mat);

	/// Multiplies row vector _vec by 4x4 matrix _mat with implicit w=0 (direction transform).
	///
	/// @param[in] _vec Input vector.
	/// @param[in] _mat Input 4x4 matrix (16 floats).
	///
	/// @returns Transformed direction vector (no translation applied).
	///
	Vec3 mulXyz0(const Vec3& _vec, const float* _mat);

	/// Multiplies row vector _vec by 4x4 matrix _mat with perspective divide.
	///
	/// @param[in] _vec Input vector.
	/// @param[in] _mat Input 4x4 matrix (16 floats).
	///
	/// @returns Transformed and perspective-divided vector.
	///
	Vec3 mulH(const Vec3& _vec, const float* _mat);

	/// Multiplies 4-component row vector _vec by 4x4 matrix _mat.
	///
	/// @param[out] _result Output 4-component vector (4 floats).
	/// @param[in] _vec Input 4-component vector (4 floats).
	/// @param[in] _mat Input 4x4 matrix (16 floats).
	///
	void vec4MulMtx(float* _result, const float* _vec, const float* _mat);

	/// Multiplies two 4x4 matrices: _result = _a * _b.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _a Left-hand 4x4 matrix (16 floats).
	/// @param[in] _b Right-hand 4x4 matrix (16 floats).
	///
	void mtxMul(float* _result, const float* _a, const float* _b);

	/// Transposes a 4x4 matrix.
	///
	/// @param[out] _result Output transposed 4x4 matrix (16 floats).
	/// @param[in] _a Input 4x4 matrix (16 floats).
	///
	void mtxTranspose(float* _result, const float* _a);

	/// Computes the inverse of a 3x3 matrix.
	///
	/// @param[out] _result Output 3x3 matrix (9 floats).
	/// @param[in] _a Input 3x3 matrix (9 floats).
	///
	void mtx3Inverse(float* _result, const float* _a);

	/// Computes the inverse of a 4x4 matrix.
	///
	/// @param[out] _result Output 4x4 matrix (16 floats).
	/// @param[in] _a Input 4x4 matrix (16 floats).
	///
	void mtxInverse(float* _result, const float* _a);

	/// Computes the cofactor matrix of a 3x3 matrix.
	///
	/// @param[out] _result Output 3x3 cofactor matrix (9 floats).
	/// @param[in] _a Input 3x3 matrix (9 floats).
	///
	void mtx3Cofactor(float* _result, const float* _a);

	/// Computes the cofactor matrix of a 4x4 matrix.
	///
	/// @param[out] _result Output 4x4 cofactor matrix (16 floats).
	/// @param[in] _a Input 4x4 matrix (16 floats).
	///
	void mtxCofactor(float* _result, const float* _a);

	/// Calculates the normal of a triangle defined by three vertices.
	///
	/// @param[in] _va First vertex.
	/// @param[in] _vb Second vertex.
	/// @param[in] _vc Third vertex.
	///
	/// @returns Triangle normal (not normalized).
	///
	Vec3 calcNormal(const Vec3& _va, const Vec3& _vb, const Vec3& _vc);

	/// Calculates a plane from three vertices.
	///
	/// @param[out] _outPlane Output plane.
	/// @param[in] _va First vertex.
	/// @param[in] _vb Second vertex.
	/// @param[in] _vc Third vertex.
	///
	void calcPlane(Plane& _outPlane, const Vec3& _va, const Vec3& _vb, const Vec3& _vc);

	/// Calculates a plane from a normal and a point on the plane.
	///
	/// @param[out] _outPlane Output plane.
	/// @param[in] _normal Plane normal (should be normalized).
	/// @param[in] _pos A point on the plane.
	///
	void calcPlane(Plane& _outPlane, const Vec3& _normal, const Vec3& _pos);

	/// Returns signed distance from a plane to a point.
	///
	/// @param[in] _plane Input plane.
	/// @param[in] _pos Point to test.
	///
	/// @returns Signed distance (positive = front side of plane).
	///
	BX_CONSTEXPR_FUNC float distance(const Plane& _plane, const Vec3& _pos);

	/// Returns true if planes _a and _b are equal within _epsilon tolerance.
	///
	/// @param[in] _a First plane.
	/// @param[in] _b Second plane.
	/// @param[in] _epsilon Tolerance.
	///
	/// @returns True if normal and distance are within _epsilon.
	///
	BX_CONSTEXPR_FUNC bool isEqual(const Plane& _a, const Plane& _b, float _epsilon);

	/// Computes a 2D linear least-squares fit: y = _result[0]*x + _result[1].
	///
	/// @param[out] _result Output coefficients (2 floats).
	/// @param[in] _points Array of 2D points.
	/// @param[in] _stride Byte stride between consecutive points.
	/// @param[in] _numPoints Number of points.
	///
	void calcLinearFit2D(float _result[2], const void* _points, uint32_t _stride, uint32_t _numPoints);

	/// Computes a 3D linear least-squares fit: z = _result[0]*x + _result[1]*y + _result[2].
	///
	/// @param[out] _result Output coefficients (3 floats).
	/// @param[in] _points Array of 3D points.
	/// @param[in] _stride Byte stride between consecutive points.
	/// @param[in] _numPoints Number of points.
	///
	void calcLinearFit3D(float _result[3], const void* _points, uint32_t _stride, uint32_t _numPoints);

	/// Converts RGB color to HSV color space.
	///
	/// @param[out] _hsv Output HSV values (3 floats).
	/// @param[in] _rgb Input RGB values (3 floats).
	///
	void rgbToHsv(float _hsv[3], const float _rgb[3]);

	/// Converts HSV color to RGB color space.
	///
	/// @param[out] _rgb Output RGB values (3 floats).
	/// @param[in] _hsv Input HSV values (3 floats).
	///
	void hsvToRgb(float _rgb[3], const float _hsv[3]);

	/// Converts sRGB gamma value to linear space.
	///
	/// @param[in] _a sRGB gamma value.
	///
	/// @returns Linear value.
	///
	BX_CONSTEXPR_FUNC float toLinear(float _a);

	/// Converts linear value to sRGB gamma space.
	///
	/// @param[in] _a Linear value.
	///
	/// @returns sRGB gamma value.
	///
	BX_CONSTEXPR_FUNC float toGamma(float _a);

	/// Convert float to half-float.
	///
	/// @param[in] _a Float value.
	///
	/// @returns Half-float value.
	///
	BX_CONST_FUNC uint16_t halfFromFloat(float _a);

	/// Convert half-float to float.
	///
	/// @param[in] _a Half-float value.
	///
	/// @returns Float value.
	///
	BX_CONST_FUNC float halfToFloat(uint16_t _a);

} // namespace bx

#include "inline/math.inl"

#endif // BX_MATH_H_HEADER_GUARD
