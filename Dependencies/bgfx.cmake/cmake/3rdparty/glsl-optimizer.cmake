# Copyright (c) 2016 Joshua Brookover

set( GLSL-OPTIMIZER_INCLUDES
	${BGFX_DIR}/3rdparty/glsl-optimizer/include
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/mapi
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl
	${BGFX_DIR}/3rdparty/glsl-optimizer/src
)

# glcpp
file( GLOB GLCPP_SOURCES ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/glcpp/*.c ${BGFX_DIR}/3rdparty/glsl-optimizer/src/util/*.c )
add_library( glcpp ${GLCPP_SOURCES} )
target_include_directories( glcpp PUBLIC ${GLSL-OPTIMIZER_INCLUDES} )
set_target_properties( glcpp PROPERTIES COMPILE_FLAGS "/W0" )
set_target_properties( glcpp PROPERTIES FOLDER "3rdparty" )

# mesa
file( GLOB MESA_SOURCES ${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa/program/*.c ${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa/main/*.c )
add_library( mesa ${MESA_SOURCES} )
target_include_directories( mesa PUBLIC ${GLSL-OPTIMIZER_INCLUDES} )
set_target_properties( mesa PROPERTIES COMPILE_FLAGS "/W0" )
set_target_properties( mesa PROPERTIES FOLDER "3rdparty" )

# glsl_optimizer
file( GLOB GLSL-OPTIMIZER_SOURCES ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/*.cpp ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/*.c )
file( GLOB GLSL-OPTIMIZER_SOURCES_REMOVE ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/main.cpp ${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/builtin_stubs.cpp )
list( REMOVE_ITEM GLSL-OPTIMIZER_SOURCES ${GLSL-OPTIMIZER_SOURCES_REMOVE} )
add_library( glsl-optimizer ${GLSL-OPTIMIZER_SOURCES} )
target_link_libraries( glsl-optimizer glcpp mesa )
set_target_properties( glsl-optimizer PROPERTIES COMPILE_FLAGS "/W0" )
set_target_properties( glsl-optimizer PROPERTIES FOLDER "3rdparty" )
