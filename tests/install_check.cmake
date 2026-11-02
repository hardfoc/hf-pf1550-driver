# Installs the driver into PREFIX and compiles a translation unit that includes pf1550.hpp from there.
file(REMOVE_RECURSE "${PREFIX}")
execute_process(COMMAND "${CMAKE_COMMAND}" --install "${BUILD_DIR}" --prefix "${PREFIX}"
                RESULT_VARIABLE rc OUTPUT_QUIET)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "cmake --install failed (${rc})")
endif()
set(src "${PREFIX}/consumer.cpp")
file(WRITE "${src}" "#include \"pf1550.hpp\"\nint main() { return pf1550::kExpectedDeviceId == 0x7C ? 0 : 1; }\n")
execute_process(COMMAND "${CXX}" -std=c++20 -fsyntax-only "-I${PREFIX}/${INCDIR}/hf_pf1550/inc" "${src}"
                RESULT_VARIABLE rc ERROR_VARIABLE err)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "installed headers do not compile:\n${err}")
endif()
