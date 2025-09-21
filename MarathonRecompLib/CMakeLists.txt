project("MarathonRecompLib")

add_compile_options(
    -fno-strict-aliasing
)

if (WIN32)
    add_compile_options(/fp:strict)
else()
    add_compile_options(-ffp-model=strict)
endif()

target_compile_definitions(XenonRecomp PRIVATE 
    XENON_RECOMP_CONFIG_FILE_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/config/Marathon.toml\"
    XENON_RECOMP_HEADER_FILE_PATH=\"${MARATHON_RECOMP_TOOLS_ROOT}/XenonRecomp/XenonUtils/ppc_context.h\")

set(MARATHON_RECOMP_PPC_RECOMPILED_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_context.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_func_mapping.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_recomp_shared.h"
)

foreach(i RANGE 0 145)
    list(APPEND MARATHON_RECOMP_PPC_RECOMPILED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_recomp.${i}.cpp")
endforeach()

add_custom_command(
    OUTPUT 
        ${MARATHON_RECOMP_PPC_RECOMPILED_SOURCES}
    COMMAND 
        $<TARGET_FILE:XenonRecomp>
    DEPENDS 
        "${CMAKE_CURRENT_SOURCE_DIR}/private/default.xex"
        "${CMAKE_CURRENT_SOURCE_DIR}/config/Marathon.toml"
    USES_TERMINAL
)

add_custom_command(
    OUTPUT
        "${CMAKE_CURRENT_SOURCE_DIR}/private/shader"
    COMMAND
        $<TARGET_FILE:u8extract> "${CMAKE_CURRENT_SOURCE_DIR}/private/shader.arc" "${CMAKE_CURRENT_SOURCE_DIR}/private/shader"
    COMMAND
        $<TARGET_FILE:u8extract> "${CMAKE_CURRENT_SOURCE_DIR}/private/shader_lt.arc" "${CMAKE_CURRENT_SOURCE_DIR}/private/shader"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/private/shader.arc"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/shader_lt.arc"
)

set(XENOS_RECOMP_ROOT "${MARATHON_RECOMP_TOOLS_ROOT}/XenosRecomp/XenosRecomp")
set(XENOS_RECOMP_INCLUDE "${XENOS_RECOMP_ROOT}/shader_common.h")

target_compile_definitions(XenosRecomp PRIVATE 
    XENOS_RECOMP_INPUT=\"${CMAKE_CURRENT_SOURCE_DIR}/private/shader\" 
    XENOS_RECOMP_OUTPUT=\"${CMAKE_CURRENT_SOURCE_DIR}/shader/shader_cache.cpp\"
    XENOS_RECOMP_INCLUDE_INPUT=\"${XENOS_RECOMP_INCLUDE}\"
    MARATHON_RECOMP
)

file(GLOB XENOS_RECOMP_SOURCES 
    "${XENOS_RECOMP_ROOT}/*.h"
    "${XENOS_RECOMP_ROOT}/*.cpp"
)

add_custom_command(
    OUTPUT 
        "${CMAKE_CURRENT_SOURCE_DIR}/shader/shader_cache.cpp"
    COMMAND
        $<TARGET_FILE:XenosRecomp>
    DEPENDS 
        "${CMAKE_CURRENT_SOURCE_DIR}/private/shader"
        ${XENOS_RECOMP_SOURCES}
        ${XENOS_RECOMP_INCLUDE}
    USES_TERMINAL
)

add_library(MarathonRecompLib
    ${MARATHON_RECOMP_PPC_RECOMPILED_SOURCES}
    "shader/shader_cache.h"
    "shader/shader_cache.cpp"
)

target_include_directories(MarathonRecompLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(MarathonRecompLib PRIVATE "${MARATHON_RECOMP_TOOLS_ROOT}/XenonRecomp/thirdparty/simde")
target_precompile_headers(MarathonRecompLib PUBLIC "ppc/ppc_recomp_shared.h")
