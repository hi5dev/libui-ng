# Building on Windows

This guide outlines how to build this library on Windows with [MSYS2] and [GNU C].

## [Index](../../README.md)

- [doc](../README.md)
  -*[build](./README.md)
    - **[Win32](./win32.md)**
    - [Cross-Compiling](./cross_compiling.md)

### Windows

This project includes a [PowerShell] script that will install everything required to build this on Windows using
[MSYS2] with [GNU C]. This script is tested regularly using [Windows Sandbox] (requires Windows Pro or higher). The
script automates these tasks:

- It installs [WinGet] if necessary. This is Microsoft's Linux-style command-line package manager.
- Installs or updates [Doxygen] using WinGet.
- Installs or updates [MSYS2] using WinGet.
- Installs all required libraries and build tools for building with [MinGW] using [MSYS2].

[GNU C]: https://www.gnu.org/software/gnu-c-manual/
[MSYS2]: https://www.msys2.org/
[MinGW]: https://www.mingw-w64.org/
[PowerShell]: https://learn.microsoft.com/en-us/powershell/
[WinGet]: https://learn.microsoft.com/en-us/windows/package-manager/winget/
[Windows Sandbox]: https://learn.microsoft.com/en-us/windows/security/application-security/application-isolation/windows-sandbox/windows-sandbox-overview
