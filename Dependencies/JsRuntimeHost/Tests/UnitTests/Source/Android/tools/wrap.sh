#!/system/bin/sh

# Ensure ASan runtime is available; prefer copy in /data/local/tmp if present.
ASAN_RT_BASENAME=${ASAN_RT_BASENAME:-libclang_rt.asan-aarch64-android.so}
ASAN_RT_LOCAL="/data/local/tmp/${ASAN_RT_BASENAME}"

if [ -f "${ASAN_RT_LOCAL}" ]; then
  ASAN_RT_PATH="${ASAN_RT_LOCAL}"
else
  ASAN_RT_PATH="${ASAN_RT_BASENAME}"
fi

export ASAN_OPTIONS=${ASAN_OPTIONS:-log_to_syslog=1:allow_user_segv_handler=1:disable_core=1:abort_on_error=0}
export LD_PRELOAD="${ASAN_RT_PATH}"

exec "$@"
