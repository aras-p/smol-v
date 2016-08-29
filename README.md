# SMOL-V: like Vulkan/Khronos SPIR-V, but smaller.

## Overview

SMOL-V encodes Vulkan/Khronos [SPIR-V](https://www.khronos.org/registry/spir-v/)
format programs into a form that is smaller, and is more
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
- Reordering instruction opcodes so that the most common ones are the smallest values, for smaller
  varint encoding.
- Encoding several instructions in a more compact form, e.g. the "typical <=4 component swizzle"
  shape of a VectorShuffle instruction.

A somewhat similar utility is [spirv-remap from glslang](https://github.com/KhronosGroup/glslang/blob/master/README-spirv-remap.txt).


## Usage

Add `source/smolv.h` and `source/smolv.cpp` to your C++ project build.
It might require C++11 or somesuch; I only tested with Visual Studio 2010, 2015 and Mac Xcode 7.3.

`smolv::Encode` and `smolv::Decode` is the basic functionality.

Other functions are for development/statistics purposes, to figure out frequencies and
distributions of the instructions.

There's a test + compression benchmarking suite in `testing/testmain.cpp`, using that needs adding
other files under testing/external to the build too (3rd party code: glslang remapper, ZStd, LZ4).


## Limitations / TODO

- SPIR-V where the words got stored in big-endian layout is not supported yet.
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

As of 2016 August 28, results on 113 shaders (under `tests/spirv-dumps`) are:

```
Compression: original size 1314.8KB
0 Remap       1314.1KB  99.9%
0 SMOL-V       448.3KB  34.1%
1    LZ4HC     329.9KB  25.1%
1 re+LZ4HC     241.8KB  18.4%
1 sm+LZ4HC     128.0KB   9.7%
2    Zstd      279.3KB  21.2%
2 re+Zstd      188.7KB  14.4%
2 sm+Zstd      117.4KB   8.9%
3    Zstd20    187.0KB  14.2%
3 re+Zstd20    129.0KB   9.8%
3 sm+Zstd20     92.0KB   7.0%
```

* "Remap" is spirv-remap from glslang, without debug info stripping (SMOL-V does not strip debug info either).
* LZ4HC and Zstd are general compression algorithms at default settings (Zstd20 is ZStd compression with almost max setting of 20).
* "re+" is "remapper + compression", "sm+" is "SMOL-V + compression".
* Compression is done on the whole blob of all the test programs (not individually for each program).
