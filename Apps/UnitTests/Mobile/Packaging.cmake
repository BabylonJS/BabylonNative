include_guard(GLOBAL)

function(package_mobile_unit_tests target bundle_id)
    if(ANDROID)
        target_sources(${target} PRIVATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Android.cpp")
        target_link_libraries(${target} PRIVATE AndroidExtensions android log)
    elseif(IOS)
        if(NOT target STREQUAL "UnitTests")
            target_sources(${target} PRIVATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Apple.mm")
        endif()
        set_target_properties(${target} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Info.plist"
            MACOSX_BUNDLE_GUI_IDENTIFIER "${bundle_id}"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${bundle_id}"
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${DEPLOYMENT_TARGET}"
            XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
    endif()
endfunction()
