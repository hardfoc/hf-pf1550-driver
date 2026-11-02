# Testing and CI

How the driver is checked: host unit tests against a register model of the chip, compiled documentation, ESP32
builds and static analysis, on every pull request.

## Host unit tests

The tests build the real driver against a mock bus that models a PF1550 at the register level: it answers only
at its address, reports DEVICE_ID 0x7C and STATE_INFO RUN, treats the interrupt status registers as
write-1-to-clear, models the indirect OTP read behind the three keys, and records every transfer, strap-pin
change and delay.

```bash
cmake -S . -B build-host -DHF_PF1550_BUILD_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

| File | Covers |
| --- | --- |
| [`tests/test_driver.cpp`](../tests/test_driver.cpp) | Identification and its errors, profile order and waits, a profile that stops at a failed write, rail and VBUS setters, strap pins, the OTP unlock sequence, fault clearing, the diagnostic snapshot and self-test, the data sheet's code tables |
| [`tests/mock_pf1550.hpp`](../tests/mock_pf1550.hpp) | The register model |
| [`tests/test_harness.hpp`](../tests/test_harness.hpp), [`tests/test_main.cpp`](../tests/test_main.cpp) | A dependency-free test runner |

The fault-decoder bit positions that differ from the data sheet ([Register map](registers.md#interrupt-registers))
are not covered by a test yet: one belongs with the fix.

## Documentation snippets

Every C++ block on this site is included from [`tests/docs/`](../tests/docs/my_i2c.hpp), which the host build
compiles (target `hf_pf1550_doc_snippets`, `-Wall -Wextra`) with the driver instantiated over the documented port.
A change to the API that breaks a page breaks the build first. The README's example is a copy of the Quickstart
snippet that `hardfoc-docs check` keeps in step, and the register table on the [Register map](registers.md) page
is generated from `inc/pf1550_registers.hpp` the same way.

## CI workflows

| Workflow | What it checks |
| --- | --- |
| Host unit tests | GCC and Clang build and run the tests and the doc snippets |
| ESP32 examples build | Every app in `examples/esp32/app_config.yml`, for each listed ESP-IDF version and build type |
| C++ lint, C++ analysis | clang-format, clang-tidy and cppcheck |
| Docs | Strict site build, figure and generated-table drift, Doxygen, link check, Lighthouse budget |
| Markdown lint, YAML lint, link check | Repository text files |

The workflows call the shared, versioned workflows of
[hf-general-ci-tools](https://github.com/hardfoc/hf-general-ci-tools) and
[hf-espidf-ci-tools](https://github.com/hardfoc/hf-espidf-ci-tools).
`scripts/verify_driver_scaffold.sh` is a local check of the repository layout, a CMake configure and the profile
table sizes.

## On hardware

The [ESP32 examples](examples.md) are the hardware checks: `pf1550_esp32c6_diagnostics` runs the self-test at boot
and a snapshot every 5 s, and `pf1550_esp32c6_provision` prints a checklist of the rail voltages to measure.
Measuring the rails, USB behaviour and I2C waveforms stays a bench job.
