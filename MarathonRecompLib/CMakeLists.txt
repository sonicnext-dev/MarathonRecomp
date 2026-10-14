project("MarathonRecompLib")

add_compile_options(
    -fno-strict-aliasing
)

if (WIN32)
    add_compile_options(/fp:strict)
else()
    add_compile_options(-ffp-model=strict)
endif()

set(GENERATED_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(XENON_RECOMP_IN_SWITCH_TABLE "${CMAKE_CURRENT_SOURCE_DIR}/config/switch_table.toml")
set(XENON_RECOMP_IN_XEX "${CMAKE_CURRENT_SOURCE_DIR}/private/default.xex")
set(XENON_RECOMP_OUT_DIR "${GENERATED_OUT_DIR}/ppc")
file(MAKE_DIRECTORY "${XENON_RECOMP_OUT_DIR}")

set(XENON_RECOMP_IN_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/config/Marathon.toml.in")
set(XENON_RECOMP_OUT_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(XENON_RECOMP_OUT_CONFIG "${XENON_RECOMP_OUT_CONFIG_DIR}/Marathon.toml")

# XenonRecomp wants paths relative to the config file directory.
file(RELATIVE_PATH XENON_RECOMP_IN_SWITCH_TABLE_REL "${XENON_RECOMP_OUT_CONFIG_DIR}" "${XENON_RECOMP_IN_SWITCH_TABLE}")
file(RELATIVE_PATH XENON_RECOMP_IN_XEX_REL "${XENON_RECOMP_OUT_CONFIG_DIR}" "${XENON_RECOMP_IN_XEX}")
file(RELATIVE_PATH XENON_RECOMP_OUT_DIR_REL "${XENON_RECOMP_OUT_CONFIG_DIR}" "${XENON_RECOMP_OUT_DIR}")

configure_file("${XENON_RECOMP_IN_CONFIG}" "${XENON_RECOMP_OUT_CONFIG}")

set(XENON_RECOMP_IN_PPC_CONTEXT "${MARATHON_RECOMP_TOOLS_ROOT}/XenonRecomp/XenonUtils/ppc_context.h")

set(MARATHON_RECOMP_PPC_RECOMPILED_SOURCES
    "${XENON_RECOMP_OUT_DIR}/ppc_config.h"
    "${XENON_RECOMP_OUT_DIR}/ppc_context.h"
    "${XENON_RECOMP_OUT_DIR}/ppc_func_mapping.cpp"
    "${XENON_RECOMP_OUT_DIR}/ppc_recomp_shared.h"
)

foreach(i RANGE 0 145)
    list(APPEND MARATHON_RECOMP_PPC_RECOMPILED_SOURCES "${XENON_RECOMP_OUT_DIR}/ppc_recomp.${i}.cpp")
endforeach()

add_custom_command(
    OUTPUT 
        ${MARATHON_RECOMP_PPC_RECOMPILED_SOURCES}
    COMMAND 
        $<TARGET_FILE:XenonRecomp> "${XENON_RECOMP_OUT_CONFIG}" "${XENON_RECOMP_IN_PPC_CONTEXT}"
    DEPENDS
        XenonRecomp
        "${XENON_RECOMP_OUT_CONFIG}"
        "${XENON_RECOMP_IN_PPC_CONTEXT}"
        "${XENON_RECOMP_IN_SWITCH_TABLE}"
        "${XENON_RECOMP_IN_XEX}"
    USES_TERMINAL
)

set(XENOS_RECOMP_IN_DIR "${CMAKE_CURRENT_BINARY_DIR}/shader")
set(XENOS_RECOMP_OUT_DIR "${GENERATED_OUT_DIR}/shader")
set(XENOS_RECOMP_OUT_FILE "${XENOS_RECOMP_OUT_DIR}/shader_cache.cpp")
file(MAKE_DIRECTORY "${XENOS_RECOMP_IN_DIR}")
file(MAKE_DIRECTORY "${XENOS_RECOMP_OUT_DIR}")

add_custom_command(
    OUTPUT
        "${XENOS_RECOMP_IN_DIR}"
    COMMAND
        $<TARGET_FILE:u8extract> "${CMAKE_CURRENT_SOURCE_DIR}/private/shader.arc" "${XENOS_RECOMP_IN_DIR}"
    COMMAND
        $<TARGET_FILE:u8extract> "${CMAKE_CURRENT_SOURCE_DIR}/private/shader_lt.arc" "${XENOS_RECOMP_IN_DIR}"
    DEPENDS
        u8extract
        "${CMAKE_CURRENT_SOURCE_DIR}/private/shader.arc"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/shader_lt.arc"
)

set(XENOS_RECOMP_ROOT "${MARATHON_RECOMP_TOOLS_ROOT}/XenosRecomp/XenosRecomp")
set(XENOS_RECOMP_INCLUDE "${XENOS_RECOMP_ROOT}/shader_common.h")

target_compile_definitions(XenosRecomp PRIVATE MARATHON_RECOMP)

add_custom_command(
    OUTPUT 
        "${XENOS_RECOMP_OUT_FILE}"
    COMMAND
        $<TARGET_FILE:XenosRecomp> "${XENOS_RECOMP_IN_DIR}" "${XENOS_RECOMP_OUT_FILE}" "${XENOS_RECOMP_INCLUDE}"
    DEPENDS
        XenosRecomp
        "${XENOS_RECOMP_IN_DIR}"
        ${XENOS_RECOMP_INCLUDE}
    USES_TERMINAL
)

# Generated shader_cache.cpp references header next to it.
set(XENOS_RECOMP_IN_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/shader/shader_cache.h")
set(XENOS_RECOMP_OUT_HEADER "${XENOS_RECOMP_OUT_DIR}/shader_cache.h")

add_custom_command(
    OUTPUT
        "${XENOS_RECOMP_OUT_HEADER}"
    COMMAND
        ${CMAKE_COMMAND} -E copy "${XENOS_RECOMP_IN_HEADER}" "${XENOS_RECOMP_OUT_HEADER}"
    DEPENDS
        "${XENOS_RECOMP_IN_HEADER}"
)

add_library(MarathonRecompLib
    ${MARATHON_RECOMP_PPC_RECOMPILED_SOURCES}
    "${XENOS_RECOMP_OUT_HEADER}"
    "${XENOS_RECOMP_OUT_FILE}"
)

target_include_directories(MarathonRecompLib PUBLIC ${GENERATED_OUT_DIR})
target_include_directories(MarathonRecompLib PRIVATE "${MARATHON_RECOMP_TOOLS_ROOT}/XenonRecomp/thirdparty/simde")
target_precompile_headers(MarathonRecompLib PUBLIC "${XENON_RECOMP_OUT_DIR}/ppc_recomp_shared.h")
