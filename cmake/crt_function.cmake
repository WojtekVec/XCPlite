# === C-RT Function DLL Project ===

# Define the target name for the shared C-RT library
set(CRT_DLL_TARGET_NAME crt_function)

# Rursively collect all source and header files from the demo project
file(GLOB_RECURSE crt_function_SOURCES
    "${PROJECT_ROOT}/src/xcpPlTl.h"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/*.cpp"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/*.h"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/private/*.cpp"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/private/*.h"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/*.cpp"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/*.h"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/via/*.h"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/xcp/*.cpp"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/xcp/*.h"
)

# Create a shared C-RT library target with the collected sources
add_library(${CRT_DLL_TARGET_NAME} SHARED ${crt_function_SOURCES})

# Explicitly set the linker language to C++
set_target_properties(${CRT_DLL_TARGET_NAME} PROPERTIES LINKER_LANGUAGE C++)

# Specify public include directories for the target
target_include_directories(${CRT_DLL_TARGET_NAME} PUBLIC
    "${PROJECT_ROOT}"
    "${PROJECT_ROOT}/src"
    "${PROJECT_ROOT}/examples/crt_function_demo/src"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/private"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/via"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/xcp"
)

# Apply source grouping only when using Visual Studio on Windows
if(MSVC)
    # private files
    source_group("private" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/private/crt.cpp")
    source_group("private" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/private/crt.h")
    source_group("private" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/private/nlapml.cpp")
    source_group("private" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/private/nlapml.h")
    source_group("private" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/private/nlapml_i.h")
    source_group("private" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/private/nlapml_v.h")
    source_group("private" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/private/variable.cpp")
    source_group("private" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/private/variable.h")
    # shared source files
    source_group("shared" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/cnpcomm.h")
    source_group("shared" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/cnpcomm.cpp")
    source_group("shared" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/Timer.h")
    source_group("shared" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/Timer.cpp")
    # via/types files
    source_group("shared/via" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/via/mbd_types.h")
    source_group("shared/via" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/via/via.h")
    # XCP files
    source_group("xcp" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/xcp/xcpPlTl.h")
    source_group("xcp" FILES "${PROJECT_ROOT}/examples/crt_function_demo/src/xcp/xcpPlTl.cpp")
endif()

# Apply common warning flags if defined
if(COMMON_WARNING_FLAGS)
    target_compile_options(${CRT_DLL_TARGET_NAME} PRIVATE ${COMMON_WARNING_FLAGS})
endif()

# Define preprocessor macros for Windows builds

if(WIN32)
    target_compile_definitions(${CRT_DLL_TARGET_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _BUILDNODELAYERDLL
        XCP_ENABLE_ABS_ADDRESSING
    )
endif()

# Link against the xcplib library
target_link_libraries(${CRT_DLL_TARGET_NAME} PRIVATE xcplib)
