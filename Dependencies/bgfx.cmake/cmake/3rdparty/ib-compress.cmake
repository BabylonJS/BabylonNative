# Copyright (c) 2016 Joshua Brookover

file( GLOB IB_COMPRESS_SOURCES ${BGFX_DIR}/3rdparty/ib-compress/*.cpp ${BGFX_DIR}/3rdparty/ib-compress/*.h )

add_library( ib-compress STATIC ${IB_COMPRESS_SOURCES} )
target_include_directories( ib-compress PUBLIC ${BGFX_DIR}/3rdparty )
set_target_properties( ib-compress PROPERTIES FOLDER "bgfx/3rdparty" )
