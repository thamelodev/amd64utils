# amd64utils Library

This library is designed for handling x86-64 (amd64) structures and mechanisms. It focuses on tasks such as parsing Global Descriptor Tables (GDT) and walking page tables. The goal is to provide a unified interface for working with x86-64 internals in an OS- and runtime-agnostic manner.

## Features

- GDT Parsing
- Page Table Walking
- OS/Runtime Agnostic (with some limitations)

## Challenges

Achieving OS/runtime agnosticism is difficult due to differences in compiler capabilities. For instance:

- MSVC (in x64 mode) doesn't support inline assembly.
- GCC allows inline assembly with `__asm`.