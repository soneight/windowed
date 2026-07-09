## CHANGES
> [keep a changelog](https://keepachangelog.com/en/1.1.0/) with [semantic versioning](https://semver.org/spec/v2.0.0.html)

## main branch

### v0.4.0 - 2026-04-22

#### Added

- this file
- [notice](../NOTICE) file
- [glfw](../licenses/glfw.md) license file

#### Changed

- License from `Boost Software License 1.0` to `Apache License 2.0`
- bump `son8__overglad` target version `0.0.6`
- use public instead of interface `MSVC` compile related options

#### Merged

- sync `glfw` branch `0.0.4` changes with `main` `0.4.0` branch

### v0.3.0 - 2025-12-22

#### Added

- [window](../include/son8/windowed/window.hxx) header: move `Windowed` class to this header
- deprecated alias `Windowed` to `Window` class

#### Changed

- rename `Windowed` class as `Window` class

### v0.2.1 - 2025-12-20
> bump `son8__overglad` target version to `0.0.5` (probably some fix)

### v0.2.0 - 2025-12-18

#### Added

- `glfw` subdirectory: for finding `glfw` target with specific [son8 header](../glfw/include/glfw/son8.hxx) and `CMake` interface target `glfw::son8`
- [Makefile](../Makefile) convenient commands file

#### Changed

- using hash tags for fetching content instead of version tags
- `son8__overglad` target bump version 0.0.4
- windowed source file: include `son8 header` instead of system `glfw` one

### v0.1.0 - 2025-05-17

#### Added

- include headers:
  - [config](../include/son8/windowed/config.hxx): helper configuration class (Config) and `OpenGL` version enum selector for window creation
  - [windowed](../include/son8/windowed.hxx): window class wrapper (Windowed) that uses `pImpl` idiom and includes `config` header

- source files:
  - [windowed](../source/windowed.cxx): windowed library implementation

- CMake:
  - if not `glfw` target presents find `glfw3` package or fullback to fetching it from `github` repository, version 3.3.10
  - if not `son8__overglad` target present fetching it from `github` repository, version 0.0.1

## glfw branch

### v0.0.4 - 2026-04-21
> mostly licensing related changes

#### Added

- `glfw` license into licenses for completeness

#### Changed

- license from `BSL-1.0` to `Apache-2.0`
- bump default fetching `overglad` version to patch 6

### v0.0.3 - 2025-12-20 IMPORTANT seems to be kind of broken because not appearing in git log
> cmake minimum required maximum 3.27, fetching `overglad` patch 5 instead patch 4

### v0.0.2 - 2025-12-18
> create special `glfw` folder to find `glfw` target, add fetching `overglad` target, fix MSVC compile options

### v0.0.1 - 2025-03-21
> find or fetch `glfw` from github and provide interface target for it
