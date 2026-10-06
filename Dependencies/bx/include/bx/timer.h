/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_TIMER_H_HEADER_GUARD
#define BX_TIMER_H_HEADER_GUARD

#include "bx.h"

namespace bx
{
	///
	int64_t getHPCounter();

	///
	int64_t getHPFrequency();

	/// Ticks.
	struct Ticks
	{
		/// No default constructor.
		Ticks() = delete;

		/// No initialization.
		Ticks(InitNoneTag);

		/// Initialize to zero.
		constexpr Ticks(InitZeroTag);

		/// Initialize to current time.
		Ticks(InitIdentityTag);

		/// Initialize to specific time in ticks.
		explicit constexpr Ticks(int64_t _ticks);

		/// Binary arithmetic operators.
		constexpr Ticks  operator+ (Ticks  _rhs) const;
		constexpr Ticks  operator- () const;
		constexpr Ticks  operator- (Ticks  _rhs) const;
		constexpr Ticks  operator* (float _rhs) const;
		constexpr Ticks& operator+=(Ticks  _rhs);
		constexpr Ticks& operator-=(Ticks  _rhs);
		constexpr Ticks& operator*=(float _rhs);

		/// Comparison operators.
		constexpr bool   operator==(Ticks  _rhs) const;
		constexpr bool   operator!=(Ticks  _rhs) const;
		constexpr bool   operator< (Ticks  _rhs) const;
		constexpr bool   operator<=(Ticks  _rhs) const;
		constexpr bool   operator> (Ticks  _rhs) const;
		constexpr bool   operator>=(Ticks  _rhs) const;

		static const Ticks  s_kStartup; //!< App start time.
		static const Ticks  s_kFreq;    //!< Frequency, ticks per second.
		static const double s_kInvFreq; //!< 1.0/s_kFreq

		int64_t ticks; //!< Timer ticks.
	};

	/// Returns current time.
	Ticks getNow();

	/// Returns time since app startup.
	Ticks getTicksSinceStartup();

	/// Returns time in seconds.
	template<typename Ty>
	constexpr Ty toSeconds(const Ticks& _time);

	/// Returns time in milliseconds.
	template<typename Ty>
	constexpr Ty toMilliseconds(const Ticks& _time); 

} // namespace bx

#include "inline/timer.inl"

#endif // BX_TIMER_H_HEADER_GUARD
