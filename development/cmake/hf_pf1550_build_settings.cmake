#===============================================================================
# PF1550 Driver - Build Settings
#===============================================================================

include_guard(GLOBAL)

set(HF_PF1550_TARGET_NAME "hf_pf1550")

set(HF_PF1550_VERSION_MAJOR 0)
set(HF_PF1550_VERSION_MINOR 1)
set(HF_PF1550_VERSION_PATCH 0)
set(HF_PF1550_VERSION "${HF_PF1550_VERSION_MAJOR}.${HF_PF1550_VERSION_MINOR}.${HF_PF1550_VERSION_PATCH}")

set(HF_PF1550_VERSION_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/../inc/pf1550_version.h.in")
set(HF_PF1550_VERSION_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/hf_pf1550_generated")
set(HF_PF1550_VERSION_HEADER     "${HF_PF1550_VERSION_HEADER_DIR}/pf1550_version.h")

file(MAKE_DIRECTORY "${HF_PF1550_VERSION_HEADER_DIR}")

if(EXISTS "${HF_PF1550_VERSION_TEMPLATE}")
    configure_file(
        "${HF_PF1550_VERSION_TEMPLATE}"
        "${HF_PF1550_VERSION_HEADER}"
        @ONLY
    )
    message(STATUS "PF1550 driver v${HF_PF1550_VERSION} — generated pf1550_version.h")
else()
    message(WARNING "pf1550_version.h.in not found at ${HF_PF1550_VERSION_TEMPLATE}")
endif()

set(HF_PF1550_PUBLIC_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/../inc"
    "${HF_PF1550_VERSION_HEADER_DIR}"
)

set(HF_PF1550_SOURCE_FILES)

set(HF_PF1550_IDF_REQUIRES driver)
