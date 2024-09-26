# amd64utils Library

`amd64utils` is a library designed to work with x86-64 (amd64) architecture structures and mechanisms. It focuses on tasks such as parsing Global Descriptor Tables (GDT), walking page tables, and managing other internals of the x86-64 architecture. 

The key objective of this library is to provide a unified, low-level interface for interacting with x86-64 internals in an OS- and runtime-agnostic manner. 

## Features

- **GDT Parsing**: Parse and manage the Global Descriptor Tables.
- **Page Table Walking**: Traverse and interact with page tables.
- **OS/Runtime Agnostic**: Designed to be independent of operating systems and compilers.
  - The library aims to support multiple compiler suites, but differences in inline assembly support (e.g., between MSVC and GCC) can pose challenges. While MSVC (in x64 mode) doesn't support inline assembly, GCC provides the ability to work with `__asm`. For now, the library uses a solution that works across most common setups.

## Goals

- Offer comprehensive utilities for x86-64 internal structures.
- Maintain compatibility with various compiler toolchains and operating systems.
  
## Challenges

Achieving OS/runtime agnosticism is difficult due to the variations in compiler capabilities. For example:

- **MSVC**: In x64 mode, inline assembly isn't supported.
- **GCC/Clang**: Inline assembly is possible via the `__asm` keyword, allowing more flexibility.
  
This discrepancy is a current limitation that we aim to overcome through improved abstraction in future versions.

## Roadmap

- Expand support for more OSs and compilers.
- Optimize performance for different environments.
- Add more low-level utilities for managing x86-64 internals.
