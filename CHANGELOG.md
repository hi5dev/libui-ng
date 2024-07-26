# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog] and this project adheres to [Semantic Versioning].

# [Unreleased]

This update is a complete overhaul of the project, and mainly addresses issues with building for Windows 10 & 11.

## Key Changes

- Enabled DPI awareness and compatibility with Windows 10 & 11.
- Fixed many issues in the Windows resource and manifest files.
- Removed many deprecated and "OLD" files.
- Dropped support for building shared libraries in this project.
- Using CMake instead of Meson, which has far better IDE and cross-platform support.

## Removed

- Windows manifests for shared libraries.
- Doxygen's configuration files - using CMake's Doxygen module instead.
- Custom Windows Resource compiler - using CMake for this now.
- Meson build files.
- GitHub workflow files.
- `old` directory and its contents, and all other files labeled as "OLD."
- `subprojects` - now using CMake's `FetchContent` to download third-party libraries.
- All previous entries to this change log as it was clearly not being maintained, and not properly structured.
- There were approximately 600 TODO items scattered throughout the code that were removed.
  - Most were questions that could only be answered by the developer that added the comments.
  - Many of which referenced broken links that would be too difficult and/or time consuming to track down.
  - The majority of these todo items have been in the code for many, many years, and will likely never be resolved.
  - Too many were comments from a previous developer questioning his abilities as a developer, and perhaps suffering
    from some kind of existential crisis. Hey, man, we've all been there. You're doing fine. No need to second
    guess yourself here.
  - Let's just keep these kind of things out of this code, shall we? Bugs, optimizations, enhancements, and things
    of that sort can be sent to the proper channels, like a GitHub issue, or a psychiatrist. Somewhere we can have a
    proper, open discussion on such topics, rather than hard-coded into the source itself.

## Changed

- Updated Windows manifests for compatibility with latest Windows versions.
  - Enabled DPI awareness for high-resolution displays.
  - Enabled compatibility up to Windows 11.
  - Replaced placeholder description and product names with actual values.
- Cleaned up all the Windows resource files and fixed issues specified in the TODO comments.
  - Removed C macros that were creating conflicts.
  - Moved macros to CMake (only the UNICODE macros remain).
  - Replaced the shared-library manifests with static-library manifests.
  - Replaced C-only headers with C++ header (e.g. `stdlib.h` with `cstdlib`).
- Using [CMake] instead of [Meson].
- Refactored project structure:
  - The project's root directory contains only configuration and about files.
  - Moved examples to `doc`.
  - Moved all public headers to `include`.
  - Moved all sources to subdirectories of `src`.

## Added

- Documentation for writing Windows apps using LibUI, and an example Windows manifest and resource file.
- XSD schemas for Windows manifests - mainly for IDE integration and XML validation tools.
- CMake function to generate Windows manifests and resource files.
- Windows manifest schemas for IDE integration and validation tools.
- [Clang] formatter and linter configuration files.
- PowerShell and Bash scripts for getting setup easily on Windows with [MSYS2].
- `assert_no_error` unit testing macro, which dumps the error message instead of just checking for null.

[//]: # (External Links)
[Keep a Changelog]: https://keepachangelog.com/en/1.0.0/
[Semantic Versioning]: https://semver.org/spec/v2.0.0.html
[CMake]: https://cmake.org/
[Clang]: https://clang.llvm.org
[MSYS2]: https://www.msys2.org/
[Meson]: https://mesonbuild.com/

[//]: # (Version Control Links)
[Unreleased]: https://github.com/hi5dev/libui-ng/tree/main
