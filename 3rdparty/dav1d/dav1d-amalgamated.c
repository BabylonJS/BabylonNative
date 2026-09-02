/* bimg: amalgamated dav1d core (bitdepth-agnostic) sources. */

/* Platform feature macros must be set before any system header is included. */
#if !defined(_WIN32)
#	ifndef _GNU_SOURCE
#		define _GNU_SOURCE       // posix_memalign, PTHREAD_STACK_MIN (glibc), etc.
#	endif //
#	ifndef _FILE_OFFSET_BITS
#		define _FILE_OFFSET_BITS 64 // 64-bit off_t on 32-bit POSIX (e.g. Android)
#	endif //
#endif //

/* macOS <sys/param.h> defines a 1-arg ALIGN() that clashes with dav1d's 2-arg
 * ALIGN(); include it first (header-guarded) and drop its macro so dav1d's wins. */
#if defined(__APPLE__)
#	include <sys/param.h>
#	undef ALIGN
#endif //

#include "dav1d-pragma-ignore.h"
#include "src/cdf.c"
#include "src/cpu.c"
#include "src/ctx.c"
#include "src/data.c"
#include "src/decode.c"
#include "src/dequant_tables.c"
#include "src/getbits.c"
#include "src/intra_edge.c"
#include "src/itx_1d.c"
#include "src/lf_mask.c"
#include "src/lib.c"
#include "src/log.c"
#include "src/mem.c"
#include "src/msac.c"
#include "src/obu.c"
#include "src/pal.c"
#include "src/picture.c"
#include "src/qm.c"
#include "src/ref.c"
#include "src/refmvs.c"
#include "src/scan.c"
#include "src/tables.c"
#include "src/thread_task.c"
#include "src/warpmv.c"
#include "src/wedge.c"

#if defined(_WIN32)
#	include "src/win32/thread.c"
#endif //
