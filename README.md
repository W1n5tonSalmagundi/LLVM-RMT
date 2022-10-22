# LLVM-RMT

...Is a language agnostic developer tool that uses whatever relevant LLVM backend is available to obtain LLVM intermediate representation of a codebase, then, a function call graph from the given project, and finally, maps it to a diagram describing how the individual 'systems' depend on each other through what functions, as described by mappings written into a user supplied configuration file.

This tool is inspired by, but not affiliated with, [Gail C. Murphy's original tool](https://www.cs.ubc.ca/~murphy/jRMTool/doc/) and corresponding paper.

## Building and ~~using~~ testing by hand

Building through the bundled makefile should painlessly produce an executable binary 'llvm-rmt', in the 'bin' directory.

Current unusual dependencies are limited to the SUSv2 realpath definition and math.h of the c standard library, and typically linked against glibc.

It is not currently functional, but the executable will look inside its' current working directory for a 'test-mappings.config' for function call to system link mappings first, then operates on the file/directory specified as the first shell argument.