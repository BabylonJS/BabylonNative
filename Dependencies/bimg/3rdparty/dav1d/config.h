/*
 * dav1d build configuration for bimg.
 */

#ifndef DAV1D_CONFIG_H
#define DAV1D_CONFIG_H

#define ARCH_AARCH64       0
#define ARCH_ARM           0
#define ARCH_LOONGARCH     0
#define ARCH_LOONGARCH32   0
#define ARCH_LOONGARCH64   0
#define ARCH_PPC64LE       0
#define ARCH_RISCV         0
#define ARCH_RV32          0
#define ARCH_RV64          0
#define ARCH_X86           0
#define ARCH_X86_32        0
#define ARCH_X86_64        0
#define HAVE_ASM           0
#define CONFIG_8BPC        1
#define CONFIG_16BPC       1
#define CONFIG_LOG         1
#define TRIM_DSP_FUNCTIONS 0
#define ENDIANNESS_BIG     0

#if defined(_WIN32)
#	define HAVE_ALIGNED_ALLOC          0
#	define HAVE_POSIX_MEMALIGN         0
#	define HAVE_MEMALIGN               0
#	define HAVE_CLOCK_GETTIME          0
#	define HAVE_SIGACTION              0
#	define HAVE_DLSYM                  0
#	define HAVE_UNISTD_H               0
#	define HAVE_IO_H                   1
#	define HAVE_SYS_TYPES_H            1
#	define HAVE_PTHREAD_GETAFFINITY_NP 0
#	define HAVE_PTHREAD_SETAFFINITY_NP 0
#	define HAVE_PTHREAD_SETNAME_NP     0
#	define HAVE_PTHREAD_SET_NAME_NP    0
#	define HAVE_PTHREAD_NP_H           0
#	define _WIN32_WINNT                0x0601
#else
#	define HAVE_ALIGNED_ALLOC          1
#	define HAVE_POSIX_MEMALIGN         1
#	define HAVE_MEMALIGN               0
#	define HAVE_CLOCK_GETTIME          1
#	define HAVE_SIGACTION              1
#	define HAVE_DLSYM                  1
#	define HAVE_UNISTD_H               1
#	define HAVE_IO_H                   0
#	define HAVE_SYS_TYPES_H            1

#	if defined(__linux__) && !defined(__ANDROID__)
#		define HAVE_PTHREAD_GETAFFINITY_NP 1
#		define HAVE_PTHREAD_SETAFFINITY_NP 1
#	else
#		define HAVE_PTHREAD_GETAFFINITY_NP 0
#		define HAVE_PTHREAD_SETAFFINITY_NP 0
#	endif //

#	if defined(__linux__) || defined(__APPLE__)
#		define HAVE_PTHREAD_SETNAME_NP     1
#	else
#		define HAVE_PTHREAD_SETNAME_NP     0
#	endif //

#	define HAVE_PTHREAD_SET_NAME_NP 0
#	define HAVE_PTHREAD_NP_H        0
#endif

#define HAVE_GETAUXVAL    0
#define HAVE_ELF_AUX_INFO 0
#define HAVE_C11_GENERIC  0

#if defined(_WIN32)
#	ifndef UNICODE
#		define UNICODE  1
#	endif // UNICODE

#	ifndef _UNICODE
#		define _UNICODE 1
#	endif // _UNICODE

#	if defined(_MSC_VER)
#		define _CRT_DECLARE_NONSTDC_NAMES 1
#		define fseeko                  _fseeki64
#		define ftello                  _ftelli64
#	else
#		define _FILE_OFFSET_BITS       64
#		define __USE_MINGW_ANSI_STDIO  1
#	endif // defined(_MSC_VER)
#endif // defined(_WIN32)

#endif // DAV1D_CONFIG_H
