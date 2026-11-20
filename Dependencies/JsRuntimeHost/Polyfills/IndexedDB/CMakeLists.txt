set(INDEXEDDB_POLYFILL_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fake-indexeddb/fake-indexeddb.js")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${INDEXEDDB_POLYFILL_FILE}")
file(READ "${INDEXEDDB_POLYFILL_FILE}" INDEXEDDB_POLYFILL_SOURCE)

# MSVC limits an individual narrow string literal to 16,380 characters
# (C2026). Keep the generated literals comfortably below that limit and join
# them at initialization time without changing bytes at chunk boundaries.
set(INDEXEDDB_POLYFILL_CHUNK_SIZE 8000)
string(LENGTH "${INDEXEDDB_POLYFILL_SOURCE}" INDEXEDDB_POLYFILL_LENGTH)
set(INDEXEDDB_POLYFILL_OFFSET 0)
set(INDEXEDDB_POLYFILL_PARTS "")
while(INDEXEDDB_POLYFILL_OFFSET LESS INDEXEDDB_POLYFILL_LENGTH)
    math(EXPR INDEXEDDB_POLYFILL_REMAINING
        "${INDEXEDDB_POLYFILL_LENGTH} - ${INDEXEDDB_POLYFILL_OFFSET}")
    if(INDEXEDDB_POLYFILL_REMAINING GREATER INDEXEDDB_POLYFILL_CHUNK_SIZE)
        set(INDEXEDDB_POLYFILL_PART_LENGTH ${INDEXEDDB_POLYFILL_CHUNK_SIZE})
    else()
        set(INDEXEDDB_POLYFILL_PART_LENGTH ${INDEXEDDB_POLYFILL_REMAINING})
    endif()
    string(SUBSTRING
        "${INDEXEDDB_POLYFILL_SOURCE}"
        ${INDEXEDDB_POLYFILL_OFFSET}
        ${INDEXEDDB_POLYFILL_PART_LENGTH}
        INDEXEDDB_POLYFILL_PART)
    string(APPEND INDEXEDDB_POLYFILL_PARTS
        "        std::string_view{R\"JSRHINDEXEDDB(${INDEXEDDB_POLYFILL_PART})JSRHINDEXEDDB\"},\n")
    math(EXPR INDEXEDDB_POLYFILL_OFFSET
        "${INDEXEDDB_POLYFILL_OFFSET} + ${INDEXEDDB_POLYFILL_PART_LENGTH}")
endwhile()

configure_file(
    "Source/IndexedDBScripts.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Generated/IndexedDBScripts.h"
    @ONLY)

set(SOURCES
    "Include/Babylon/Polyfills/IndexedDB.h"
    "Source/IndexedDB.cpp"
    "Source/IndexedDBScripts.h.in"
    "ThirdParty/fake-indexeddb/BuildEntry.js"
    "ThirdParty/fake-indexeddb/StorageClone.js"
    "ThirdParty/fake-indexeddb/LICENSE"
    "ThirdParty/fake-indexeddb/fake-indexeddb.js")

add_library(IndexedDB ${SOURCES})
warnings_as_errors(IndexedDB)

target_include_directories(IndexedDB
    PUBLIC "Include"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Generated")

target_link_libraries(IndexedDB
    PUBLIC JsRuntime
    PRIVATE Scheduling)

set_property(TARGET IndexedDB PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
