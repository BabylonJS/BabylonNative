#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable: 4057) // warning C4057: indirection to slightly different base types
#	pragma warning(disable: 4090) // warning C4090: different 'const' qualifiers
#	pragma warning(disable: 4100) // warning C4100: unreferenced formal parameter
#	pragma warning(disable: 4127) // warning C4127: conditional expression is constant
#	pragma warning(disable: 4152) // warning C4152: function/data pointer conversion in expression
#	pragma warning(disable: 4189) // warning C4189: local variable is initialized but not referenced
#	pragma warning(disable: 4200) // warning C4200: zero-sized array in struct/union
#	pragma warning(disable: 4201) // warning C4201: nonstandard extension: nameless struct/union
#	pragma warning(disable: 4244) // warning C4244: conversion, possible loss of data
#	pragma warning(disable: 4245) // warning C4245: conversion, signed/unsigned mismatch
#	pragma warning(disable: 4324) // warning C4324: structure was padded due to alignment specifier
#	pragma warning(disable: 4389) // warning C4389: signed/unsigned mismatch
#	pragma warning(disable: 4456) // warning C4456: declaration hides previous local declaration
#	pragma warning(disable: 4457) // warning C4457: declaration hides function parameter
#	pragma warning(disable: 4701) // warning C4701: potentially uninitialized local variable used
#	pragma warning(disable: 4702) // warning C4702: unreachable code
#	pragma warning(disable: 4703) // warning C4703: potentially uninitialized local pointer variable used
#	pragma warning(disable: 5287) // warning C5287: operands are different enum types
#elif defined(__GNUC__) || defined(__clang__)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-parameter"
#	pragma GCC diagnostic ignored "-Wunused-function"
#	pragma GCC diagnostic ignored "-Wunused-variable"
#	pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#	pragma GCC diagnostic ignored "-Wsign-compare"
#	pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#	pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#	pragma GCC diagnostic ignored "-Wtype-limits"
#	pragma GCC diagnostic ignored "-Wshadow"

#	if !defined(__clang__)
#		pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#	endif //
#endif //
