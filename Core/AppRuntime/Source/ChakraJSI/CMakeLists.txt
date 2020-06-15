add_library(chakra-jsi
    "Common/unicode.cpp"
    "Common/unicode.h"
    "Common/utilities.h"
    "JSI/Shared/ByteArrayBuffer.h"
    "JSI/Shared/ChakraObjectRef.cpp"
    "JSI/Shared/ChakraObjectRef.h"
    "JSI/Shared/ChakraRuntime.cpp"
    "JSI/Shared/ChakraRuntime.h"
    "JSI/Shared/ChakraRuntimeArgs.h"
    "JSI/Shared/ChakraRuntimeFactory.h"
    "JSI/Shared/RuntimeHolder.h"
    "JSI/Shared/ScriptStore.h")

target_compile_definitions(chakra-jsi PUBLIC USE_EDGEMODE_JSRT)

target_include_directories(chakra-jsi
    PUBLIC "Common")

target_link_libraries(chakra-jsi
    PRIVATE jsi
    "chakrart.lib")
