# amd64utils Library

This library is designed for handling x86-64 (amd64) structures and mechanisms. It focuses on tasks such as parsing Global Descriptor Tables (GDT) and walking page tables. 

## Features

- Segmentation Parsing
- Page Table Walking

## Challenges

Achieving OS/runtime agnosticism is difficult due to differences in compiler capabilities. For instance:

- MSVC (in x64 mode) doesn't support inline assembly.
- GCC allows inline assembly with `__asm`.