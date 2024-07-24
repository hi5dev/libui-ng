# Win32 Guide

This is a quick-start guide on writing an application that uses LibUI with support for Windows 10 and 11.

- [main.c](#mainc)
- [myapp.exe.manifest](#myappexemanifest)
- [myapp.rc](#myapprc)
- [CMakeLists.txt](#cmakeliststxt)

## main.c

Without going into too much detail, here's a basic starter template that will run on Unix and Windows systems.

```c++
#ifdef WIN32
#include <windows.h>
#endif

#include <ui.h>

#ifdef WIN32
int
WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
  UNREFERENCED_PARAMETER (hInstance);
  UNREFERENCED_PARAMETER (hPrevInstance);
  UNREFERENCED_PARAMETER (lpCmdLine);
  UNREFERENCED_PARAMETER (nShowCmd);

#else
int
main (void)
{
#endif

  // Initialize LibUI
  uiInitOptions ui_init_options = {};
  const char ui_init_error      = uiInit (&ui_init_options);

  // Dump any errors to STDERR and exit with return code 1 when LibUI fails to initialize.
  if (ui_init_error != NULL)
    {
      (void)fprintf (stderr, "%s:%d: %s\n", __FILE_NAME__, __LINE__, ui_init_error);
      uiFreeInitError (ui_init_error);
      return 1;
    }

  // Create and show your window, then call uiMain() here.
  // See the examples for more information.

  // Finalize LibUI
  uiUninit();

  // Exit successfully.
  return 0;
}
```

## myapp.exe.manifest

Windows requires a [manifest] file to determine which versions of Windows the application is compatible with, to load
the appropriate COM libraries, and to enable DPI awareness. Without this, it's likely that the application will
crash when attempting to initialize LibUI. At the very least, the interface will appear blurry on high-DPI monitors.

Name the manifest `myapp.exe.manifest`, replacing `myapp` with your executable file's name. This is a requirement
by Windows. Naming it anything else is likely to fail.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<assembly
  xmlns="urn:schemas-microsoft-com:asm.v1"
  xmlns:asmv3="urn:schemas-microsoft-com:asm.v3"
  manifestVersion="1.0">

  <!-- Change name and version to reflect your application. -->
  <!-- Version is formatted MAJOR.MINOR.PATCH.BUILD -->
  <assemblyIdentity
    type="win32"
    processorArchitecture="*"
    name="division.orginization.application"
    version="0.1.0.0"
  />

  <!-- Set an appropriate description -->
  <description>Example Win32 manifest.</description>

  <!-- Enables modern Windows themes and styles. LibUI does not work on Windows without this. -->
  <!-- https://learn.microsoft.com/en-us/windows/win32/sbscs/enabling-an-assembly-in-an-application-without-extensions -->
  <dependency>
    <dependentAssembly>
      <assemblyIdentity
        type="win32"
        processorArchitecture="*"
        language="*"
        name="Microsoft.Windows.Common-Controls"
        version="6.0.0.0"
        publicKeyToken="6595b64144ccf1df"
      />
    </dependentAssembly>
  </dependency>

  <!-- Specify compatible Windows versions. -->
  <!-- Without this, the default is Windows Vista compatibility on Windows 7. -->
  <compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
    <application>
      <!-- Windows 11, 10 -->
      <supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>

      <!-- Windows 8.1, Server 2012 R12 -->
      <supportedOS Id="{1f676c76-80e1-4239-95bb-83d0f6d0da78}"/>

      <!-- Windows 8, Server 2012 -->
      <supportedOS Id="{4a2f28e3-53b9-4441-ba9c-d69d4a4a6e38}"/>

      <!-- Windows 7, Server 2008 R2 -->
      <supportedOS Id="{35138b9a-5d96-4fbd-8e2d-a2440225f93a}"/>

      <!-- Windows Vista, Server 2008 -->
      <supportedOS Id="{e2011457-1546-43c5-a5fe-008deee3d3f0}"/>
    </application>
  </compatibility>

  <!-- Enable DPI-awareness for high-resolution monitors. -->
  <asmv3:application>
    <asmv3:windowsSettings>
      <dpiAware xmlns="http://schemas.microsoft.com/SMI/2005/WindowsSettings">true</dpiAware>
      <dpiAwareness xmlns="http://schemas.microsoft.com/SMI/2016/WindowsSettings">system</dpiAwareness>
    </asmv3:windowsSettings>
  </asmv3:application>
</assembly>
```

> **_NOTE:_** If the manifest is not formatted correctly, the executable will simply fail to launch. If this
> happens, check the Windows application events with Event Viewer. There will be an error entry with a backtrace to
> let you know why.

## myapp.rc

You can distribute the above manifest file with the application, or you can link it to the executable using a
Windows resource file. Here's a minimal resource file to use as a template.

In this guide, this file is named `myapp.rc`. The only requirement you have in naming this file is the `.rc` extension.

```text
#include <windows.h>

1 24 "myapp.exe.manifest"
```

## CMakeLists.txt

Here's a `CMakeLists.txt` file that will build this example application.

```cmake
# this is the last known version of cmake that this was tested on
cmake_minimum_required (VERSION 3.25)

project (myapp VERSION 0.1.0)

find_package (libui REQUIRED)

add_executable (myapp)

target_link_libraries (myapp PRIVATE libui::libui)

target_sources (myapp PRIVATE main.c)

# This will enable Unicode support for Windows, which requires using `wWinMain` instead of `main` for the
# entry-point, as we have done in the above example. It also links the Windows resource file to the executable,
# which in turn embeds the Windows manifest file.
if (WIN32)
  # This will disable the command-prompt, building a GUI application instead. Alternatively, you can instead call
  # `add_executable (myapp WIN32)`, which basically just does this.
  set_target_properties (myapp PROPERTIES WIN32_EXECUTABLE ON)

  # Enables unicode support.
  target_compile_definitions (myapp PUBLIC -DUNICODE -D_UNICODE)
  target_link_options (myapp PUBLIC -municode)

  # CMake has support for Windows resource files built in, so all you have to do is add it to your executable.
  target_sources (myapp PUBLIC myapp.rc)
endif ()
```

[//]: # (External Links)
[manifest]: https://learn.microsoft.com/en-us/windows/win32/sbscs/application-manifests
