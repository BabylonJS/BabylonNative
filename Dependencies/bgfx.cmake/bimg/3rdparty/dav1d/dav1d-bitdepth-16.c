/* bimg: amalgamated dav1d 16-bit DSP templates (one translation unit per bitdepth). */

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


#define BITDEPTH 16

#include "dav1d-pragma-ignore.h"
#include "src/cdef_apply_tmpl.c"
#include "src/cdef_tmpl.c"
#include "src/fg_apply_tmpl.c"
#include "src/filmgrain_tmpl.c"
#include "src/ipred_prepare_tmpl.c"
#include "src/ipred_tmpl.c"
#include "src/itx_tmpl.c"
#include "src/lf_apply_tmpl.c"
#include "src/loopfilter_tmpl.c"
#include "src/looprestoration_tmpl.c"
#include "src/lr_apply_tmpl.c"
#include "src/mc_tmpl.c"
#include "src/recon_tmpl.c"
