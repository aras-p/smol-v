# SMOL-V: like Vulkan/Khronos SPIR-V, but smaller.

## Overview

SMOL-V encodes Vulkan/Khronos SPIR-V format programs into a form that is smaller, and is more
compressible. No changes to the programs are done; they decode into exactly same program
as was encoded.

SPIR-V is a very verbose format, several times larger than same programs expressed in other
shader formats *(e.g. DX11 bytecode, GLSL, DX9 bytecode etc.)*. The SSA-form with ever increasing
IDs is not very appreciated by regular data compressors either. SMOL-V does several things
to improve this:

- Many words, especially ones that most often have small values, are encoded using
  ["varint" scheme](https://developers.google.com/protocol-buffers/docs/encoding) (1-5 bytes per
  word, with just one byte for values in 0..127 range).

- Some IDs used in the program are delta-encoded, relative to previously seen IDs (e.g. Result
  IDs). Often instructions reference things that were computed just before, so this results in
  small deltas. These values are also encoded using "varint" scheme.

A somewhat similar utility is [spirv-remap from glslang](https://github.com/KhronosGroup/glslang/blob/master/README-spirv-remap.txt).


## Usage

Add `source/smolv.h` and `source/smolv.cpp` to your C++ project build.
Currently it might require C++11 or somesuch; I only tested with Mac Xcode 7.3 so far.

`smolv::Encode` and `smolv::Decode` is the basic functionality.

Other functions are for development/statistics purposes, to figure out frequencies and
distributions of the instructions.

There's a test + compression benchmarking suite in `testing/testmain.cpp`, using that needs adding
other files under testing/external to the build too (3rd party code: glslang remapper, ZStd, LZ4).


## Limitations / TODO

- Big-endian SPIR-V are not supported yet.
- The whole thing might not work on Big-Endian CPUs. It might, but I'm not 100% sure.
- Not much prevention is done against malformed/corrupted inputs, TODO.
- Out of memory cases are not handled. The code will either throw exception
  or crash, depending on your compilation flags.


## License

Public Domain

There is 3rd party code under the testing framework (`testing/external`); it is not required for
using SMOL-V. All that code (glslang, lz4, ZStd is BSD-licensed, and taken from github repositories of the respective
projects).


## Results

As of 2016 August 27, results on 75 shaders (under `tests/spirv-dumps`) are:

```
Compression: original size 807.0KB
0 Remap        : 806.7KB (100.0%)
0 SMOL         : 321.2KB ( 39.8%)
1    LZ4HC     : 217.3KB ( 26.9%)
1 re+LZ4HC     : 157.8KB ( 19.6%)
1 sm+LZ4HC     :  88.2KB ( 10.9%)
2    Zstd      : 182.8KB ( 22.7%)
2 re+Zstd      : 128.4KB ( 15.9%)
2 sm+Zstd      :  82.9KB ( 10.3%)
3    Zstd20    : 125.7KB ( 15.6%)
3 re+Zstd20    :  91.5KB ( 11.3%)
3 sm+Zstd20    :  63.1KB (  7.8%)
```

* "Remap" is spirv-remap from glslang, without debug info stripping (SMOL-V does not strip debug info either).
* LZ4HC and Zstd are general compression algorithms at default settings (Zstd20 is ZStd compression with almost max setting of 20).
* "re+" is "remapper + compression", "sm+" is "SMOL-V + compression".
* Compression is done on the whole blob of all the test programs (not individually for each program).
