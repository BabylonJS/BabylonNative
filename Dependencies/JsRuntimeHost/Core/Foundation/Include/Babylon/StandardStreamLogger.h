#pragma once

#include "Api.h"

namespace Babylon::StandardStreamLogger
{
    /**
     * Starts process-wide standard-stream forwarding.
     *
     * Android forwards to logcat, Apple platforms forward to os_log, and Windows
     * forwards to OutputDebugString while preserving the original stream destination.
     * Other Unix platforms already expose standard streams and leave them unchanged.
     *
     * Returns false if a platform stream could not be redirected. Repeated calls are
     * idempotent.
     */
    bool BABYLON_API Start();

    /**
     * Flushes pending output, restores the original streams, and stops forwarding.
     *
     * Returns false if an original stream could not be restored or pending output
     * could not be drained before the shutdown timeout. Repeated calls are idempotent.
     */
    bool BABYLON_API Stop();

    /**
     * Returns whether Start() has successfully begun process-wide forwarding and
     * Stop() has not yet completed.
     *
     * This is the logical started flag, not a live probe of the underlying file
     * descriptors. On platforms that leave stdout/stderr unchanged (plain Linux
     * and other non-Android Unix hosts), Start() still succeeds and IsStarted()
     * reports true even though no redirection was installed.
     */
    bool BABYLON_API IsStarted();
}