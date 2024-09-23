# SMOL-V Change Log

For an overview, see [readme](README.md).

## 2024 Sep 23

* Added support for SPIR-V 1.6 version.
* Build: switched the benchmark/test application
  to CMake, away from manually maintained VS/Xcode/make files.

## 2020 05 25

* Added option to strip only some of debug names via `StripOpNameFilterFunc`.
* Added support for decoding SMOL-V files encoded by Unity 2017-2020
  versions (which used code as it was on 2016-08-31), via
  `kDecodeFlagUse20160831AsZeroVersion` flag.

## 2020 02 24

* Added mechanism for "versioning" SMOL-V encodings.
  Note: files produced by SMOL-V 2020-02-13 version that happened
  to contain ExecutionModeId..GroupNonUniformQuadSwap opcodes
  will not decode properly; please re-encode them.

## 2020 02 13

* Added support for SPIR-V 1.4 and 1.5 versions.

## 2019 05 02

* Better handling of invalid SPIR-V inputs.


## 2018 10 27

* Added support for SPIR-V 1.2 and 1.3 versions.


## 2016 09 04

* Tests: added suite of shaders from Shadertoy.


## 2016 09 01

* Improve compression for programs already processed by
  spirv-remap. "Relative to result ID" entries now encode
  negative numbers in a more compact way.
* More compact encoding of MemberDecorate op sequences.
* Tests: added suite of shaders from DOTA2 and Talos Principle.


## 2016 08 31

* Optional flag to strip debug information from SPIR-V: `kEncodeFlagStripDebugInfo`. *(Florian Penzkofer)*


## 2016 08 27

* Initial version.
