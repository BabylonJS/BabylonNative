/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_OS_H_HEADER_GUARD
#define BX_OS_H_HEADER_GUARD

#include "filepath.h"

#if BX_PLATFORM_OSX
#	define BX_DL_EXT "dylib"
#elif BX_PLATFORM_WINDOWS
#	define BX_DL_EXT "dll"
#else
#	define BX_DL_EXT "so"
#endif //

BX_ERROR_RESULT(kErrorMemoryMapFailed,   BX_MAKEFOURCC('b', 'x', '8', '0') );
BX_ERROR_RESULT(kErrorMemoryUnmapFailed, BX_MAKEFOURCC('b', 'x', '8', '1') );

namespace bx
{
	/// Virtual memory flags used by `memoryMap`, `memoryUnmap`, and `memoryPageSize`.
	/// Flags are grouped into mutually exclusive fields (State, Protection, Page, Advise).
	/// Zero in a field means "no change" to that aspect of an existing region.
	struct Memory
	{
		enum Enum : uint32_t
		{
			// State (bits 0..3) - zero = no change to existing region.
			Reserve          = 0x00000001, //!< Reserve address range (no backing).
			Commit           = 0x00000003, //!< Reserve + back with physical pages.
			Decommit         = 0x00000004, //!< Drop physical pages (keep reservation).
			StateMask        = 0x0000000f,

			// Protection (bits 4..7) - zero = no change to existing region. Mutually exclusive values.
			ProtectNone      = 0x00000010, //!< No access.
			ProtectRead      = 0x00000020, //!< Read-only.
			ProtectReadWrite = 0x00000030, //!< Read/Write.
			ProtectReadExec  = 0x00000040, //!< Read/Execute.
			ProtectMask      = 0x000000f0,

			// Page size (bits 8..11) - zero = default system page size. Applied only on fresh reservation.
			PageLarge        = 0x00000100, //!< Large/huge pages (~2 MiB).
			PageHuge         = 0x00000200, //!< Gigantic pages (~1 GiB) where supported.
			PageMask         = 0x00000f00,

			// Access advice (bits 12..15) - zero = no change. Mutually exclusive values.
			Normal           = 0x00001000, //!< Reset access pattern to default.
			SequentialAccess = 0x00002000, //!< Hint: memory is accessed sequentially.
			RandomAccess     = 0x00003000, //!< Hint: memory is accessed randomly.
			Prefetch         = 0x00004000, //!< Fault-in / prefault pages.
			DontNeed         = 0x00005000, //!< Can drop pages opportunistically.
			AdviseMask       = 0x0000f000,

			ReadWrite        = Commit | ProtectReadWrite, //!< Reserve + commit + read/write.
		};
	};

	/// Suspend calling thread for at least `_ms` milliseconds.
	///
	/// @param[in] _ms Minimum sleep duration, in milliseconds.
	///
	void sleep(uint32_t _ms);

	/// Yield remainder of the current thread's time slice to another runnable thread.
	void yield();

	/// Get the OS-level identifier of the calling thread.
	///
	/// @returns Platform-specific thread id.
	///
	uint32_t getTid();

	/// Get resident memory used by the current process, in bytes.
	///
	/// @returns Process resident memory footprint in bytes, or 0 if unavailable.
	///
	size_t getProcessMemoryUsed();

	/// Open a dynamic library.
	///
	/// @param[in] _filePath Path to the library (extension may be platform-specific, see `BX_DL_EXT`).
	/// @returns Handle to the loaded library, or NULL on failure.
	///
	void* dlopen(const FilePath& _filePath);

	/// Close a dynamic library previously opened with `dlopen`.
	///
	/// @param[in] _handle Handle returned by `dlopen`.
	///
	void dlclose(void* _handle);

	/// Look up a symbol in a dynamic library.
	///
	/// @param[in] _handle Handle returned by `dlopen`.
	/// @param[in] _symbol Symbol name to resolve.
	/// @returns Address of the symbol, or NULL if not found.
	///
	void* dlsym(void* _handle, const StringView& _symbol);

	/// Look up a typed symbol in a dynamic library.
	///
	/// @param[in] _handle Handle returned by `dlopen`.
	/// @param[in] _symbol Symbol name to resolve.
	/// @returns Address of the symbol cast to `ProtoT`, or NULL if not found.
	///
	template<typename ProtoT>
	ProtoT dlsym(void* _handle, const StringView& _symbol);

	/// Read an environment variable.
	///
	/// @param[out]    _out       Buffer that receives the value (may be NULL to query required size).
	/// @param[in,out] _inOutSize On input the capacity of `_out`, on output the required size including terminator.
	/// @param[in]     _name      Name of the environment variable.
	/// @returns true if the variable exists and its value fits in `_out`.
	///
	bool getEnv(char* _out, uint32_t* _inOutSize, const StringView& _name);

	/// Set an environment variable.
	///
	/// @param[in] _name  Name of the environment variable.
	/// @param[in] _value Value to assign.
	///
	void setEnv(const StringView& _name, const StringView& _value);

	/// Change current working directory of the process.
	///
	/// @param[in] _path Target directory path.
	/// @returns 0 on success, non-zero on failure.
	///
	int chdir(const char* _path);

	/// Execute a program in a child process.
	///
	/// @param[in] _argv NULL-terminated argument vector; `_argv[0]` is the executable path.
	/// @returns Platform-specific process handle, or NULL on failure.
	///
	void* exec(const char* const* _argv);

	/// Terminate the current process.
	///
	/// @param[in] _exitCode Exit code returned to the OS.
	/// @param[in] _cleanup  When true, run cleanup (atexit handlers, etc.); when false, exit immediately.
	///
	[[noreturn]] void exit(int32_t _exitCode, bool _cleanup = true);

	/// Map, reconfigure, or reshape a virtual memory region.
	///
	/// - If `_address` is NULL: reserves a new region of `_size`, aligned to `_alignment`
	///   (0 = page size), and applies the requested state/protect/advise flags. Flags
	///   must include `Memory::Reserve`.
	/// - If `_address` is non-NULL: reconfigures the existing region according to the
	///   flags present. Each flag group (State / Protection / Advise) is only touched
	///   when its bits are non-zero, allowing targeted changes.
	///
	/// Page-size bits select which page size to use for a fresh reservation; they
	/// are ignored for in-place reconfiguration of an existing region.
	///
	/// @param[in]  _address   Existing base, or NULL to allocate a new region.
	/// @param[in]  _size      Size in bytes, rounded up to the selected page size.
	/// @param[in]  _alignment Base alignment in bytes, 0 = page size.
	/// @param[in]  _flags     OR-combination of `Memory::Enum` values.
	/// @param[out] _err       Error state.
	/// @returns Base address of the region (same as `_address` for reconfiguration), or NULL on failure.
	///
	void* memoryMap(void* _address, size_t _size, size_t _alignment, uint32_t _flags, Error* _err);

	/// Release a region previously returned by `memoryMap`. Pages are decommitted and
	/// the reservation is returned to the OS.
	///
	/// @param[in]  _address Base address returned by `memoryMap`.
	/// @param[in]  _size    Size in bytes of the region to release.
	/// @param[out] _err     Error state.
	///
	void memoryUnmap(void* _address, size_t _size, Error* _err);

	/// Query virtual memory page size.
	///
	/// @param[in] _flags Pass `Memory::PageLarge` / `Memory::PageHuge` to query large/huge
	///                   page sizes; 0 returns the default system page size.
	/// @returns Page size in bytes, or 0 when the requested page kind is unsupported.
	///
	size_t memoryPageSize(uint32_t _flags = 0);

} // namespace bx

#include "inline/os.inl"

#endif // BX_OS_H_HEADER_GUARD
