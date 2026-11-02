#===============================================================================
# PF1550 Driver - Root CMake
#===============================================================================

cmake_minimum_required(VERSION 3.16)

project(hf_pf1550_driver LANGUAGES CXX)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/hf_pf1550_build_settings.cmake)
include(GNUInstallDirs)

# pf1550.hpp includes "../src/pf1550.ipp", so the install keeps the inc/ and src/ pair side by side:
# <includedir>/hf_pf1550/inc/*.hpp and <includedir>/hf_pf1550/src/pf1550.ipp.
set(HF_PF1550_INSTALL_ROOT "${CMAKE_INSTALL_INCLUDEDIR}/hf_pf1550")
set(HF_PF1550_INSTALL_INC  "${HF_PF1550_INSTALL_ROOT}/inc")

if(HF_PF1550_SOURCE_FILES)
    add_library(${HF_PF1550_TARGET_NAME} STATIC ${HF_PF1550_SOURCE_FILES})
    target_include_directories(${HF_PF1550_TARGET_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/inc>
            $<BUILD_INTERFACE:${HF_PF1550_VERSION_HEADER_DIR}>
            $<INSTALL_INTERFACE:${HF_PF1550_INSTALL_INC}>
    )
    target_compile_features(${HF_PF1550_TARGET_NAME} PUBLIC cxx_std_20)
else()
    add_library(${HF_PF1550_TARGET_NAME} INTERFACE)
    target_include_directories(${HF_PF1550_TARGET_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/inc>
            $<BUILD_INTERFACE:${HF_PF1550_VERSION_HEADER_DIR}>
            $<INSTALL_INTERFACE:${HF_PF1550_INSTALL_INC}>
    )
    target_compile_features(${HF_PF1550_TARGET_NAME} INTERFACE cxx_std_20)
endif()

add_library(hf::pf1550 ALIAS ${HF_PF1550_TARGET_NAME})

set_target_properties(${HF_PF1550_TARGET_NAME} PROPERTIES
    VERSION ${HF_PF1550_VERSION}
    SOVERSION ${HF_PF1550_VERSION_MAJOR})

option(HF_PF1550_ENABLE_WARNINGS "Enable extra warnings for PF1550 driver" OFF)
if(HF_PF1550_ENABLE_WARNINGS)
    if(TARGET ${HF_PF1550_TARGET_NAME})
        target_compile_options(${HF_PF1550_TARGET_NAME} INTERFACE -Wall -Wextra -Wpedantic)
    endif()
endif()

# ── Host unit tests (register model; no hardware) ────────────────────────────
option(HF_PF1550_BUILD_TESTS "Build the host unit tests in tests/" OFF)
if(HF_PF1550_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

include(CMakePackageConfigHelpers)

install(TARGETS ${HF_PF1550_TARGET_NAME} EXPORT hf_pf1550Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${HF_PF1550_INSTALL_INC}
)

install(DIRECTORY inc/ DESTINATION ${HF_PF1550_INSTALL_INC}
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.h.in" EXCLUDE)
install(FILES "${HF_PF1550_VERSION_HEADER}" DESTINATION ${HF_PF1550_INSTALL_INC})
install(FILES src/pf1550.ipp DESTINATION ${HF_PF1550_INSTALL_ROOT}/src)

install(EXPORT hf_pf1550Targets
    FILE hf_pf1550Targets.cmake
    NAMESPACE hf::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_pf1550
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/hf_pf1550Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/hf_pf1550Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_pf1550
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/hf_pf1550ConfigVersion.cmake"
    VERSION ${HF_PF1550_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/hf_pf1550Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/hf_pf1550ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_pf1550
)
