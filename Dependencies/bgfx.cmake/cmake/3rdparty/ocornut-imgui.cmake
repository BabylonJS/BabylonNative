# Copyright (c) 2016 Joshua Brookover

file( GLOB OCORNUT_IMGUI_SOURCES ${BGFX_DIR}/3rdparty/ocornut-imgui/*.cpp ${BGFX_DIR}/3rdparty/ocornut-imgui/*.h ${BGFX_DIR}/3rdparty/ocornut-imgui/*.inl )

add_library( ocornut-imgui STATIC ${OCORNUT_IMGUI_SOURCES} )
target_compile_definitions( ocornut-imgui PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" )
target_include_directories( ocornut-imgui PUBLIC ${BGFX_DIR}/3rdparty )
target_link_libraries( ocornut-imgui PUBLIC bx )
set_target_properties( ocornut-imgui PROPERTIES FOLDER "3rdparty" )
