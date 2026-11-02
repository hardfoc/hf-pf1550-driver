# CMake integration

## Target

| Item | Value |
|------|-------|
| CMake target | `hf_pf1550` |
| Alias | `hf::pf1550` |
| Settings file | `cmake/hf_pf1550_build_settings.cmake` |

## Variables (from build settings)

| Variable | Description |
|----------|-------------|
| `HF_PF1550_VERSION` | Semantic version string |
| `HF_PF1550_PUBLIC_INCLUDE_DIRS` | `inc/` + generated version dir |
| `HF_PF1550_SOURCE_FILES` | Empty (header-only) |
| `HF_PF1550_IDF_REQUIRES` | `driver` (ESP-IDF) |

## Options

| Option | Default | Description |
|--------|---------|-------------|
| `HF_PF1550_ENABLE_WARNINGS` | OFF | Propagate `-Wall -Wextra -Wpedantic` |

## hf-core

When `HF_CORE_ENABLE_PF1550=ON`:

- Includes `hf_pf1550_build_settings.cmake`
- Compiles `Pf1550Handler.cpp`
- Defines `HARDFOC_PF1550_SUPPORT=1`
