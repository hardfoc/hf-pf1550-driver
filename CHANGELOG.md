# Changelog

All notable changes to this driver are listed here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project uses
[Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added

- Host unit tests (`-DHF_PF1550_BUILD_TESTS=ON`) against a register model of the PF1550, and a CI workflow that
  runs them with GCC and Clang.

### Changed

- Documentation moved to the HardFOC MkDocs platform: new page set (quickstart, hardware, architecture, rails and
  OTP, register map, Portenta H7 profiles, API guide, porting, external provisioning, testing), code snippets
  compiled by the host build from `tests/docs/`, Doxygen under `/api/`, versioned with mike. The Jekyll site files
  and the doxygen-awesome submodule are gone. The curated register list moved from `docs/datasheet/` to the
  Register map page.

## 0.1.0 (version in the build settings; not tagged)

- PF1550 driver over a CRTP bus interface: identification, register access, Portenta H7 default and carrier
  profiles, rail set points, VBUS input limit, strap pins, indirect OTP reads, interrupt masking, diagnostic
  snapshot and boot self-test; the ESP32-C6 probe, register dump, diagnostics and provisioning examples.
