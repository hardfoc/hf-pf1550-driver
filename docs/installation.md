# Installation

## Requirements

- **C++20** compiler
- **CMake 3.16+**
- Optional: **ESP-IDF v5.4+** for ESP32-C6 examples

## Header-only driver

No static library is required. Include paths:

- `inc/` — hand-written headers (`pf1550.hpp`, registers, profiles)
- Generated `pf1550_version.h` — from CMake `configure_file`

## CMake (standalone)

```cmake
add_subdirectory(path/to/hf-pf1550-driver)
target_link_libraries(my_firmware PRIVATE hf::pf1550)
```

Or after install:

```cmake
find_package(hf_pf1550 REQUIRED)
target_link_libraries(my_firmware PRIVATE hf::pf1550)
```

## ESP-IDF component

See `examples/esp32/components/hf_pf1550/` — wraps `cmake/hf_pf1550_build_settings.cmake`.

## hf-core integration

In the parent HAL build:

```cmake
-DHF_CORE_ENABLE_PF1550=ON
```

Handler: `handlers/pf1550/Pf1550Handler.{h,cpp}`.

## Datasheet (local)

```bash
./scripts/fetch_datasheet.sh
./scripts/extract_register_reference.sh
```

**Next:** [Quick start →](quickstart.md)
