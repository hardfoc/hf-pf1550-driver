# Installation

Add the driver to a CMake or ESP-IDF project. It is a header-only C++20 library with no dependencies.

## Requirements

- A C++20 compiler (the driver uses `std::span`): GCC 10+, Clang 12+ or a current MSVC.
- CMake 3.16 or newer; ESP-IDF v5.4 or v5.5 for the ESP32-C6 examples.
- No RTOS, heap or exceptions: every call is `noexcept` and returns `bool` or a value, and failures also set a
  sticky error flag.

## Get the source

A git submodule keeps the driver pinned to a version you chose:

```bash
git submodule add https://github.com/hardfoc/hf-pf1550-driver.git external/hf-pf1550-driver
git submodule update --init external/hf-pf1550-driver
```

Only `inc/`, `src/`, `cmake/` and `CMakeLists.txt` are needed to build. The examples have their own submodule
(`examples/esp32/scripts`); you do not need it to use the driver.

## CMake

```cmake
add_subdirectory(external/hf-pf1550-driver)
target_link_libraries(my_app PRIVATE hf::pf1550)
```

`hf::pf1550` is an alias of the `hf_pf1550` INTERFACE target. It carries the include directories (`inc/` and the
generated version header) and requires C++20 (`cxx_std_20`), so your target does not need to set either.
`find_package(hf_pf1550)` works after `cmake --install`.

| Option | Default | Effect |
| --- | --- | --- |
| `HF_PF1550_ENABLE_WARNINGS` | `OFF` | `-Wall -Wextra -Wpedantic` on targets that link the driver |
| `HF_PF1550_BUILD_TESTS` | `OFF` | Builds the host unit tests in `tests/` ([Testing and CI](testing.md)) |

## ESP-IDF

The ESP-IDF component wrapper is
[`examples/esp32/components/hf_pf1550/`](../examples/esp32/components/hf_pf1550/CMakeLists.txt). Copy that folder
into your project's `components/` (or point `EXTRA_COMPONENT_DIRS` at it) and set the `HF_PF1550_ROOT` cache
variable to the driver checkout; the wrapper stops with a clear message when the path is wrong. Your component
then declares `REQUIRES hf_pf1550`. The wrapper requires the IDF `driver` component, which the ESP32 reference
port uses.

## How the build is organised

| Layer | File | Role |
| --- | --- | --- |
| Settings | [`cmake/hf_pf1550_build_settings.cmake`](../cmake/hf_pf1550_build_settings.cmake) | Target name, include directories, version: the single source of truth |
| Library | [`CMakeLists.txt`](../CMakeLists.txt) | INTERFACE target, install and package export |
| ESP-IDF | [`examples/esp32/components/hf_pf1550/CMakeLists.txt`](../examples/esp32/components/hf_pf1550/CMakeLists.txt) | IDF component wrapper over the same settings |

The version is set once in the settings file and written into `pf1550_version.h` at configure time. Read it at
compile or run time:

```cpp
const char* v = pf1550::PF1550<MyI2c>::GetDriverVersion();   // "0.1.0"
```

## The data sheet

NXP's data sheet is not committed. Two scripts fetch it into the git-ignored `_local_reference/` folder and
regenerate the plain-text register extract from it:

```bash
./scripts/fetch_datasheet.sh              # _local_reference/datasheet/PF1550.pdf
./scripts/extract_register_reference.sh   # docs/datasheet/PF1550-i2c-register-reference.md
```

## Next

[Quickstart](quickstart.md) wires the chip and reads its rails; [Porting](porting.md) writes the bus interface for
your MCU.
