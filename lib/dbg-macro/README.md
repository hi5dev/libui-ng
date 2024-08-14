# Third-Party Libraries: dbg-macro

A set of dbg(…) macros for C.

## [Index](../../README.md)
- [Third-Party Libraries](../README.md)
  - **[dbg-macro](./README.md)**

These minor changes were made to this library:

- Reformatted the code to match the [.clang-format](../../.clang-format) rules in this project.
- Replaced `static inline` with `inline`.
- Redundant parentheses were removed.
- Using `strerror_s` instead of `strerror_r`.
- Using `const` variables where applicable.

Other changes may have been made not listed above. Most changes were necessary to get the library to compile
without any warnings or errors.
