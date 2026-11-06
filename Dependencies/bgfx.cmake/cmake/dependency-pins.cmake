get_filename_component(INPUT_FILE "${INPUT_FILE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
get_filename_component(OUTPUT_FILE "${OUTPUT_FILE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
file(READ "${INPUT_FILE}" contents)
set(pins "")
foreach(dep IN ITEMS bx bimg bgfx)
	string(TOUPPER "${dep}" upper)
	# Read inline pins and the variable assignments used by older releases.
	string(REGEX MATCHALL
		"(set\\(${upper}_GIT_TAG|FetchContent_Declare\\([ \t\r\n]*${dep}[ \t\r\n]+[^)]*GIT_TAG)[ \t\r\n]+[^ \t\r\n)]+"
		matches "${contents}"
	)
	list(LENGTH matches count)
	if(NOT count EQUAL 1)
		message(FATAL_ERROR "Expected one pinned ${dep} declaration in ${INPUT_FILE}")
	endif()
	string(REGEX MATCH "[^ \t\r\n)]+$" pin "${matches}")
	string(LENGTH "${pin}" length)
	if(NOT length EQUAL 40 OR NOT pin MATCHES "^[0-9a-fA-F]+$")
		message(FATAL_ERROR "Expected a full commit SHA for ${dep} in ${INPUT_FILE}")
	endif()
	string(APPEND pins "${dep} ${pin}\n")
endforeach()
file(WRITE "${OUTPUT_FILE}" "${pins}")
