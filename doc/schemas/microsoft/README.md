# Microsoft Schemas

This directory contains schemas for Microsoft manifest files used for IDE integration and XML validation tools.

> **_Source:_** https://learn.microsoft.com/en-us/windows/win32/sbscs/manifest-files-reference

## [Index](../../../README.md)

- [doc](../../README.md)
  - [examples](../../examples/README.md)
  - [linux](../../linux/README.md)
  - [osx](../../osx/README.md)
  - [schemas](../README.md)
    - **[microsoft](./README.md)**
      - [configuration.xsd](./configuration.xsd)
      - [manifest.xsd](./manifest.xsd)
      - [publisher.xsd](./publisher.xsd)
  - [win32](../../win32/README.md)

### `configuration.xsd`

Redirects the assembly versions of assembly dependencies using
[per-application configuration](https://learn.microsoft.com/en-us/windows/win32/sbscs/per-application-configuration).

### `manifest.xsd`

Describes the names, versions, resources, and assembly dependencies of
[side-by-side assemblies](https://learn.microsoft.com/en-us/windows/win32/sbscs/assembly-manifests).

### `publisher.xsd`

Redirects the assembly versions of assembly dependencies on a per-assembly basis using a
[publisher configuration](https://learn.microsoft.com/en-us/windows/win32/sbscs/publisher-configuration).
