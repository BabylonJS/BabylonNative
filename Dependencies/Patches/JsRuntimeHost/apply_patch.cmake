cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED source_dir)
    message(FATAL_ERROR "source_dir not provided")
endif()

if(NOT DEFINED patch_file)
    message(FATAL_ERROR "patch_file not provided")
endif()

execute_process(
    COMMAND git -C "${source_dir}" apply --check "${patch_file}"
    RESULT_VARIABLE check_result
    OUTPUT_VARIABLE check_output
    ERROR_VARIABLE check_error)
if(check_result EQUAL 0)
    execute_process(
        COMMAND git -C "${source_dir}" apply "${patch_file}"
        RESULT_VARIABLE apply_result
        OUTPUT_VARIABLE apply_output
        ERROR_VARIABLE apply_error)
    if(NOT apply_result EQUAL 0)
        message(FATAL_ERROR "Failed to apply ${patch_file}: ${apply_error}")
    endif()
else()
    execute_process(
        COMMAND git -C "${source_dir}" apply --reverse --check "${patch_file}"
        RESULT_VARIABLE reverse_result
        OUTPUT_VARIABLE reverse_output
        ERROR_VARIABLE reverse_error)
    if(reverse_result EQUAL 0)
        message(STATUS "Patch ${patch_file} already applied")
    else()
        message(FATAL_ERROR "Failed to validate ${patch_file}: ${check_error}")
    endif()
endif()
