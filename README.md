This project is related to amd64utils Library.

The library will be responsible for handling the x86-64 structures and mechanisms, like parsing the Global Descriptor Tables, Walking page-tables and so-on. Anything related to the x86-64 internals.
Such thing means that the library should be OS/Runtime (by runtime I mostly talking about the compiler suite) agnostic [^1].


[^1] - This should be acquired but it's hard doing this for now because the compilers have different ways of doing this like inline assembly or assembly linking. In MSVC, if in x64 Mode, you can't
just inline assembly. However this can be done in GCC with __asm stuff. But this should do the work for now!