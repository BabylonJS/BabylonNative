#pragma once

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#define XR_CHECK(OPERATION) do { XrResult result = OPERATION; if (XR_FAILED(result)) throw std::exception{}; } while (false)
#define XR_ASSERT(OPERATION) do { XrResult result = OPERATION; assert(!XR_FAILED(result)); } while (false)
