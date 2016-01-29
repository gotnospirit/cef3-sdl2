cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME "cef3-sdl2")

# Overwrite default configurations RelWithDebInfo-- & MinSizeRel--
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)

project(${PROJECT_NAME})

if (MSVC)
    # 0x0501 for Windows XP
    # 0x0502 for Windows Server 2003
    # 0x0600 for Windows Vista
    # 0x0601 for Windows 7
    # 0x0602 for Windows 8
    # 0x0603 for Windows 8.1
    #
    # @see http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
    add_definitions(
        -D_WIN32_WINNT=0x0602
        -DNOMINMAX
        -DWIN32_LEAN_AND_MEAN
        -D_CRT_SECURE_NO_WARNINGS
    )

    # MD : MultiThread DLL
    # MT : MultiThread (static)
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif ()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

execute_process(COMMAND
    ${CMAKE_COMMAND} -E remove_directory ${EXECUTABLE_OUTPUT_PATH}
)

set(LNK_LIBRARIES
    "${CMAKE_SOURCE_DIR}/vendor/sdl/lib/x86/SDL2.lib"
    "${CMAKE_SOURCE_DIR}/vendor/sdl/lib/x86/SDL2main.lib"
    "${CMAKE_SOURCE_DIR}/vendor/cef/lib/cef_sandbox.lib"
    "${CMAKE_SOURCE_DIR}/vendor/cef/lib/libcef.lib"
    "${CMAKE_SOURCE_DIR}/vendor/cef/lib/libcef_dll_wrapper.lib"
)

set(COPY_DLL
    "vendor/sdl/lib/x86/SDL2.dll"

    "vendor/cef/lib/d3dcompiler_43.dll"
    "vendor/cef/lib/d3dcompiler_47.dll"
    "vendor/cef/lib/libcef.dll"
    "vendor/cef/lib/libEGL.dll"
    "vendor/cef/lib/libGLESv2.dll"
    "vendor/cef/lib/natives_blob.bin"
    "vendor/cef/lib/snapshot_blob.bin"
    "vendor/cef/lib/widevinecdmadapter.dll"
    "vendor/cef/lib/wow_helper.exe"
)

include_directories(
    include
    vendor/sdl/include
    vendor/cef
    vendor/cef/include
)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${LNK_LIBRARIES}
)

foreach(dll ${COPY_DLL})
    get_filename_component(dll_filename ${dll} NAME)

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/${dll}" "${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>/${dll_filename}"
    )
endforeach()

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/vendor/cef/Resources" "${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>"
)