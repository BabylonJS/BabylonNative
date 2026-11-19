#pragma once

#include <jni.h>

namespace node_api_tests {
bool InitializeAndroidNodeApiTests(JNIEnv* env, jobject context);
}
