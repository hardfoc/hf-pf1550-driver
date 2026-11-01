set(HF_PF1550_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." CACHE PATH
    "Path to hf-pf1550-driver root")

get_filename_component(HF_PF1550_ROOT "${HF_PF1550_ROOT}" ABSOLUTE)
if(NOT EXISTS "${HF_PF1550_ROOT}/cmake/hf_pf1550_build_settings.cmake")
    message(FATAL_ERROR "Cannot find PF1550 driver root at ${HF_PF1550_ROOT}")
endif()

include("${HF_PF1550_ROOT}/cmake/hf_pf1550_build_settings.cmake")

idf_component_register(
    INCLUDE_DIRS ${HF_PF1550_PUBLIC_INCLUDE_DIRS}
    REQUIRES ${HF_PF1550_IDF_REQUIRES}
)

target_compile_features(${COMPONENT_LIB} INTERFACE cxx_std_20)
