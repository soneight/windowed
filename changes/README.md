## CHANGES
> [keep a changelog](https://keepachangelog.com/en/1.1.0/) with [semantic versioning](https://semver.org/spec/v2.0.0.html)

## MAIN branch

### Unreleased

- refine error handling
- aspect ratio and window size configuration
- wrap input handling

### `v0.5.3` - 2026-07-13

#### Added

- `Window` throws on construction when `Config::LingerUS` exceeds 20 milliseconds limit

#### Changed

- linger behavior instead of sleeping for no matter what, now sleeps until it not exceeds `Config::LingerUS` microseconds after running poll events and user callback
- private `Window` method name from `throw_Error` to `if_Error_Throw` to better indicate that there if condition involved

### `v0.5.2` - 2026-07-13

#### Added

- `Window::Error` enum class, 0 is not an error
- `is_error` `Window` public method to check is some functions return error
- private `Window` class method `throw_Error`: that throw exception if input error code was non-zero

#### Changed

- use raw pointer instead of `unique_ptr` to not include memory header in library public headers
- `init_opengl` now returns error codes instead of throwing an exception

### `v0.5.1` - 2026-07-12

#### Added

- `Config` class `LingerUS`: unsigned microseconds to linger (sleep) when swap buffers is not involved, to mitigate `cpu` throttling

#### Changed

- `Window` class implementation preserve `Config` values as const property

#### Fixed

- initializing `OpenGL` multiple times throws ( example `run_swap` plus `run` combo bug)
- `glfw` window hint state messing `OpenGL` profile when recreating windows with different versions in a loop (use `glfwDefaultWindowHints` on each window creation)

### `v0.5.0` - 2026-07-10

#### Added

- missing `OpenGL` constant for version `4.6 compatibility`
- enforcing initializing `glfw`, creating windows and handling poll events only on main thread
- enforcing `Window` count only to one per process
- throwing exception if `glad` failed to load `OpenGL` functions
- `Window` class public methods for handling buffer swapping, polling events, closing window and initializing `OpenGL` functions via `glad`
- `Window` blocking run methods: `run_{poll,swap,void}` for fine-tune behavior
- `Window` `run` method now initializing `OpenGL` before loop and freeing it after loop

#### Changed

- use of `OpenGL` enum constant instead of plain unsigned for initializing default value
- `Window` `method` loop now call poll events before user callback and swapping buffers after user callback
- `CMake` library target is forced to be `STATIC`

#### Removed

- include `README` file was pointless


### `v0.4.0` - 2026-04-22

#### Added

- this file
- [`notice`](../NOTICE) file
- [`glfw`](../licenses/glfw.md) license file

#### Changed

- License from `Boost Software License 1.0` to `Apache License 2.0`
- bump `son8__overglad` target version `0.0.6`
- use public instead of interface `MSVC` compile related options

#### Merged

- sync `glfw` branch `0.0.4` changes with `main` `0.4.0` branch

### `v0.3.0` - 2025-12-22

#### Added

- [`window`](../include/son8/windowed/window.hxx) header: move `Windowed` class to this header
- deprecated alias `Windowed` to `Window` class

#### Changed

- rename `Windowed` class as `Window` class

### `v0.2.1` - 2025-12-20
> bump `son8__overglad` target version to `0.0.5` (probably some fix)

### `v0.2.0` - 2025-12-18

#### Added

- `glfw` subdirectory: for finding `glfw` target with specific [`son8 header`](../glfw/include/glfw/son8.hxx) and `CMake` interface target `glfw::son8`
- [`Makefile`](../Makefile) convenient commands file

#### Changed

- using hashtags for fetching content instead of version tags
- `son8__overglad` target bump version 0.0.4
- windowed source file: include `son8 header` instead of system `glfw` one

### `v0.1.0` - 2025-05-17

#### Added

- include headers:
  - [config](../include/son8/windowed/config.hxx): helper configuration class (Config) and `OpenGL` version enum selector for window creation
  - [windowed](../include/son8/windowed.hxx): window class wrapper (Windowed) that uses `pImpl` idiom and includes `config` header

- source files:
  - [windowed](../source/windowed.cxx): windowed library implementation

- CMake:
  - if not `glfw` target presents find `glfw3` package or fullback to fetching it from `github` repository, version 3.3.10
  - if not `son8__overglad` target present fetching it from `github` repository, version 0.0.1

## GLFW branch

### `v0.0.4` - 2026-04-21
> mostly licensing related changes

#### Added

- `glfw` license into licenses for completeness

#### Changed

- license from `BSL-1.0` to `Apache-2.0`
- bump default fetching `overglad` version to patch 6

### `v0.0.3` - 2025-12-20 IMPORTANT seems to be kind of broken because not appearing in git log
> CMake minimum required maximum 3.27, fetching `overglad` patch 5 instead patch 4

### `v0.0.2` - 2025-12-18
> create special `glfw` folder to find `glfw` target, add fetching `overglad` target, fix MSVC compile options

### `v0.0.1` - 2025-03-21
> find or fetch `glfw` from `github` and provide interface target for it
