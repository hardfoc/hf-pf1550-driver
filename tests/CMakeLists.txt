# =============================================================================
# hf-pf1550-driver — host unit tests
# =============================================================================
# Builds the driver headers against a register model of the PF1550 behind the bus interface and runs
# the tests with CTest. No hardware and no ESP-IDF needed:
#
#   cmake -S . -B build-host -DHF_PF1550_BUILD_TESTS=ON
#   cmake --build build-host
#   ctest --test-dir build-host --output-on-failure
# =============================================================================

add_executable(hf_pf1550_host_tests
  test_main.cpp
  test_driver.cpp
  test_conformance.cpp
)

target_link_libraries(hf_pf1550_host_tests PRIVATE ${HF_PF1550_TARGET_NAME})
target_include_directories(hf_pf1550_host_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(hf_pf1550_host_tests PRIVATE cxx_std_20)
target_compile_options(hf_pf1550_host_tests PRIVATE -Wall -Wextra)

add_test(NAME hf_pf1550_host_tests COMMAND hf_pf1550_host_tests)

# The code shown on the documentation site (docs/ includes these files with pymdownx.snippets), built so a
# page cannot show code that does not compile against the current headers. Compiled, never linked or run.
add_library(hf_pf1550_doc_snippets OBJECT
  docs/quickstart.cpp
  docs/api_tasks.cpp
  docs/ports.cpp
  docs/configuration.cpp
)
target_link_libraries(hf_pf1550_doc_snippets PRIVATE ${HF_PF1550_TARGET_NAME})
target_include_directories(hf_pf1550_doc_snippets PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/docs)
target_compile_features(hf_pf1550_doc_snippets PRIVATE cxx_std_20)
target_compile_options(hf_pf1550_doc_snippets PRIVATE -Wall -Wextra)

# The installed package must be usable on its own: install into the build tree, then compile a consumer
# against the installed headers only.
add_test(NAME hf_pf1550_install_check
  COMMAND ${CMAKE_COMMAND}
    -DBUILD_DIR=${CMAKE_BINARY_DIR}
    -DPREFIX=${CMAKE_BINARY_DIR}/install-check
    -DCXX=${CMAKE_CXX_COMPILER}
    -DINCDIR=${CMAKE_INSTALL_INCLUDEDIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/install_check.cmake)

# A bus without DelayUs() must be rejected at compile time with a clear message: the driver's waits
# (profile delays, the 2 ms OTP wait) would otherwise be skipped silently. Passes only when the
# compile fails with the driver's static_assert. The source is named .cpp.in so the linters, which
# compile every .cpp, skip it.
add_test(NAME hf_pf1550_requires_delay_hook
  COMMAND ${CMAKE_CXX_COMPILER} -std=c++20 -fsyntax-only
          "-I$<JOIN:$<TARGET_PROPERTY:${HF_PF1550_TARGET_NAME},INTERFACE_INCLUDE_DIRECTORIES>,;-I>"
          -x c++ ${CMAKE_CURRENT_SOURCE_DIR}/compile_fail/no_delay_hook.cpp.in
  COMMAND_EXPAND_LISTS)
set_tests_properties(hf_pf1550_requires_delay_hook PROPERTIES
  PASS_REGULAR_EXPRESSION "PF1550 bus must provide DelayUs")
