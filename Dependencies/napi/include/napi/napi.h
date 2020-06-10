#ifndef NAPI_H_
#define NAPI_H_

#include "js_native_api_types.h"

#ifndef NODE_ADDON_API_DISABLE_NODE_SPECIFIC
#include <node_api.h>
#else
#include "js_native_api.h"
#endif

#include "js_napi.h"
#include "napi-inl.h"

#include "env.h"

#endif // NAPI_H_