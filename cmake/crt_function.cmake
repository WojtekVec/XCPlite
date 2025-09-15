# === C-RT Function DLL Project ===

set(CRT_DLL_TARGET_NAME crt_function)

file(GLOB_RECURSE crt_function_SOURCES
    "${PROJECT_ROOT}/examples/crt_function_demo/src/*.cpp"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/*.h"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/private/*.cpp"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/private/*.h"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/*.cpp"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/*.h"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/via/*.h"
)

add_library(${CRT_DLL_TARGET_NAME} SHARED ${crt_function_SOURCES})
set_target_properties(${CRT_DLL_TARGET_NAME} PROPERTIES LINKER_LANGUAGE C++)

target_include_directories(${CRT_DLL_TARGET_NAME} PUBLIC
    "${PROJECT_ROOT}"
    "${PROJECT_ROOT}/src"
    "${PROJECT_ROOT}/examples/crt_function_demo/src"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/private"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared"
    "${PROJECT_ROOT}/examples/crt_function_demo/src/shared/via"
)

if(COMMON_WARNING_FLAGS)
    target_compile_options(${CRT_DLL_TARGET_NAME} PRIVATE ${COMMON_WARNING_FLAGS})
endif()

if(WIN32)
    target_compile_definitions(${CRT_DLL_TARGET_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS _BUILDNODELAYERDLL XCP_ENABLE_ABS_ADDRESSING)
endif()

target_link_libraries(${CRT_DLL_TARGET_NAME} PRIVATE xcplib)
