include(FetchContent)

function(bgfx_resolve_source name repository revision)
	string(TOUPPER "${name}" upper)
	set(root_variable "${upper}_DIR")
	set(override_variable "FETCHCONTENT_SOURCE_DIR_${upper}")
	set(wrapper_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/..")
	file(REAL_PATH "${wrapper_root}" wrapper_root)

	if(NOT "${${root_variable}}" STREQUAL "")
		file(REAL_PATH "${${root_variable}}" source_root BASE_DIRECTORY "${wrapper_root}")
	elseif(NOT "${${override_variable}}" STREQUAL "")
		# Let FetchContent retain its normal override/provider semantics.
		set(fetch_source ON)
	elseif(EXISTS "${wrapper_root}/${name}/include/${name}/${name}.h")
		set(source_root "${wrapper_root}/${name}")
	else()
		set(fetch_source ON)
	endif()

	if(fetch_source)
		FetchContent_Declare(${name}
			GIT_REPOSITORY "${repository}"
			GIT_TAG "${revision}"
			GIT_SUBMODULES ""
			SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/bgfx.cmake-src/${name}"
			SOURCE_SUBDIR "_bgfx_population_only")
		FetchContent_MakeAvailable(${name})
		FetchContent_GetProperties(${name} SOURCE_DIR source_root)
		file(REAL_PATH "${source_root}" source_root)
	endif()

	if(name STREQUAL "bgfx")
		set(sentinels include/bgfx/bgfx.h src/bgfx.cpp scripts/bgfx.idl)
	elseif(name STREQUAL "bx")
		set(sentinels include/bx/bx.h src/bx.cpp)
	else()
		set(sentinels include/bimg/bimg.h src/image.cpp)
	endif()
	foreach(sentinel IN LISTS sentinels)
		if(NOT EXISTS "${source_root}/${sentinel}")
			message(FATAL_ERROR "${root_variable}: '${source_root}' is not a valid ${name} source tree (missing ${sentinel}). Supply sources before configuring disconnected builds.")
		endif()
	endforeach()

	# Resolved roots are normal variables, not cache defaults that would turn
	# a managed checkout into an explicit override on the next configure.
	set(${root_variable} "${source_root}" PARENT_SCOPE)
endfunction()

bgfx_resolve_source(bgfx https://github.com/BabylonJS/bgfx.git 64e7324430604ba61a45b51e1cbeb2b67c5d1d15)
bgfx_resolve_source(bx https://github.com/bkaradzic/bx.git a9a71f8ef449c11042607feaeae0b8f46d22869a)
bgfx_resolve_source(bimg https://github.com/bkaradzic/bimg.git 671fe06dd7012a1ab85aa931019ed16112909756)
