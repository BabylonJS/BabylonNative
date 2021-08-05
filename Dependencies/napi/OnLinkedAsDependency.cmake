# Callback to perform custom behavior -- in this case, copying runtime output artifacts like DLLs -- when 
# linked from an executable target as a library.
function(on_linked_as_dependency target)
    # Propagate this file to the target so that it will be transitively available to targets that
    # link to that one, too.
    propagate_on_linked_as_dependency_cmake_file(napi ${target})
    if (DEFINED NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS)
        # We only need to actually copy files if we're being linked from an executable.
        get_target_property(type ${target} TYPE)
        if(${type} STREQUAL "EXECUTABLE")
            if (WINDOWS_STORE)
                # WINDOWS_STORE allows us to use the VS_DEPLOYMENT_CONTENT property.
                target_sources(${target} PRIVATE ${NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS})

                if ((DEFINED NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS_DEBUG) AND (DEFINED NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS_RELEASE))
                    set_property(SOURCE ${NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS_DEBUG} PROPERTY VS_DEPLOYMENT_CONTENT $<CONFIG:Debug>)
                    set_property(SOURCE ${NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS_RELEASE} PROPERTY VS_DEPLOYMENT_CONTENT $<NOT:$<CONFIG:Debug>>)
                else()
                    set_property(SOURCE ${NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS} PROPERTY VS_DEPLOYMENT_CONTENT 1)
                endif()
                
                set_property(SOURCE ${NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS} PROPERTY VS_DEPLOYMENT_LOCATION ".")
            else()
                # Without the VS_DEPLOYMENT_CONTENT property, create custom rules to copy the artifacts.
                foreach(artifact ${NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS})
                    add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${artifact} $<TARGET_FILE_DIR:${target}>)
                endforeach()
            endif()
        endif()
    endif()
endfunction()