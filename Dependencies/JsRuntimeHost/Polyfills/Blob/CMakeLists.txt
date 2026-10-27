set(SOURCES 
    "Include/Babylon/Polyfills/Blob.h"
    "InternalInclude/Babylon/Polyfills/BlobInternal.h"
    "Source/Blob.cpp"
    "Source/Blob.h")

add_library(Blob ${SOURCES})
warnings_as_errors(Blob)

target_include_directories(Blob PUBLIC "Include")
target_include_directories(Blob PRIVATE "InternalInclude")

target_link_libraries(Blob
    PUBLIC JsRuntime)

set_property(TARGET Blob PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Seam for other polyfills in this repo that need to read a Blob's contents. Deliberately not part
# of the public "Include" directory: the contract is not one we are ready to commit to for embedders.
add_library(BlobInternal INTERFACE)
target_include_directories(BlobInternal INTERFACE "InternalInclude")
target_link_libraries(BlobInternal INTERFACE Blob)
