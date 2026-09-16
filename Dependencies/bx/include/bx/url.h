/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_URL_H_HEADER_GUARD
#define BX_URL_H_HEADER_GUARD

#include "string.h"

namespace bx
{
	/// Splits URL into tokens. Tokens are views into the parsed string, and they
	/// don't own any storage.
	///
	class UrlView
	{
	public:
		/// URL token.
		///
		enum Enum
		{
			Scheme,   //!< Scheme, without `://` separator.
			UserName, //!< User name from authority's user info.
			Password, //!< Password from authority's user info.
			Host,     //!< Host name, or IP address.
			Port,     //!< Port, as it appears in URL, without `:` separator.
			Path,     //!< Path, including leading `/`.
			Query,    //!< Query, without leading `?`.
			Fragment, //!< Fragment, without leading `#`.

			Count
		};

		/// Constructor. All tokens are empty until `parse` succeeds.
		///
		UrlView();

		/// Reset all tokens to empty string view.
		///
		void clear();

		/// Parse URL into tokens.
		///
		/// @param[in] _url URL to parse. It must have scheme, or path, or both.
		///   It's not copied, and it must outlive UrlView.
		///
		/// @returns True if URL is parsed successfully. Otherwise tokens are
		///   left in unspecified state, and must not be used.
		///
		bool parse(const StringView& _url);

		/// Get token.
		///
		/// @param[in] _token Token to get.
		///
		/// @returns Token, or empty string view if URL doesn't contain it.
		///
		const StringView& get(Enum _token) const;

	private:
		StringView m_tokens[Count];
	};

	/// Characters `urlEncode` passes through unescaped, in addition to the
	/// unreserved set `A-Z a-z 0-9 - _ . ~`.
	///
	struct UrlEncoding
	{
		/// Encoding.
		///
		enum Enum
		{
			Component, //!< Nothing. Use for single path segment, query value, etc.
			Path,      //!< `/` separators. Use for whole path.

			Count
		};
	};

	/// Percent-encode string.
	///
	/// @param[out] _out Output buffer. It's always zero terminated, unless
	///   `_max` is zero. Encoding stops on truncation, and escape sequence is
	///   never split.
	/// @param[in] _max Size of output buffer, including zero terminator.
	/// @param[in] _str String to encode.
	/// @param[in] _encoding Characters to pass through unescaped.
	///
	/// @returns Number of characters written, excluding zero terminator.
	///
	int32_t urlEncode(
		  char* _out
		, int32_t _max
		, const StringView& _str
		, UrlEncoding::Enum _encoding = UrlEncoding::Component
		);

} // namespace bx

#endif // BX_URL_H_HEADER_GUARD
