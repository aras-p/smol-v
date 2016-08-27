// smol-v - v0.1 - public domain - https://github.com/aras-p/smol-v
// authored on 2016 by Aras Pranckevicius
// no warranty implied; use at your own risk
//
//
// ### OVERVIEW:
//
// SMOL-V encodes Vulkan/Khronos SPIR-V format programs into a form that is smaller, and is more
// compressible. No changes to the programs are done; they decode into exactly same program
// as was encoded.
//
// SPIR-V is a very verbose format, several times larger than same programs expressed in other
// shader formats (e.g. DX11 bytecode, GLSL, DX9 bytecode etc.). The SSA-form with ever increasing
// IDs is not very appreciated by regular data compressors either. SMOL-V does several things
// to improve this:
//
// - Many words, especially ones that most often have small values, are encoded using
//   "varint" scheme (1-5 bytes per word, with just one byte for values in 0..127 range).
//   See https://developers.google.com/protocol-buffers/docs/encoding
//
// - Some IDs used in the program are delta-encoded, relative to previously seen IDs (e.g. Result
//   IDs). Often instructions reference things that were computed just before, so this results in
//   small deltas. These values are also encoded using "varint" scheme.
//
// A somewhat similar utility is spirv-remap from glslang, see
// https://github.com/KhronosGroup/glslang/blob/master/README-spirv-remap.txt
//
//
// ### USAGE:
//
// Add source/smolv.h and source/smolv.cpp to your C++ project build.
// Currently it might require C++11 or somesuch; I only tested with Mac Xcode 7.3 so far.
//
// smolv::Encode and smolv::Decode is the basic functionality.
//
// Other functions are for development/statistics purposes, to figure out frequencies and
// distributions of the instructions.
//
// There's a test + compression benchmarking suite in testing/testmain.cpp; using that needs adding
// other files under testing/external to the build too (3rd party code: glslang remapper, ZStd, LZ4).
//
//
// ### LIMITATIONS / TODO:
//
// - Big-endian SPIR-V are not supported yet.
// - The whole thing might not work on Big-Endian CPUs. It might, but I'm not 100% sure.
// - Not much prevention is done against malformed/corrupted inputs, TODO.
// - Out of memory cases are not handled. The code will either throw exception
//   or crash, depending on your compilation flags.

#pragma once

#include <stdint.h>
#include <vector>

namespace smolv
{
	typedef std::vector<uint8_t> ByteArray;

	// -------------------------------------------------------------------
	// Encoding / Decoding

	// Encode SPIR-V into SMOL-V.
	//
	// Resulting data is appended to outSmolv array (the array is not cleared).
	//
	// Returns false on malformed SPIR-V input; if that happens the output array might get
	// partial/broken SMOL-V program.
	bool Encode(const void* spirvData, size_t spirvSize, ByteArray& outSmolv);


	// Decode SMOL-V into SPIR-V.
	//
	// Resulting data is appended to outSpirv array (the array is not cleared).
	//
	// Returns false on malformed input; if that happens the output array might get
	// partial/broken SPIR-V program.
	
	bool Decode(const void* smolvData, size_t smolvSize, ByteArray& outSpirv);


	// -------------------------------------------------------------------
	// Computing instruction statistics on SPIR-V/SMOL-V programs

	struct InputStats;

	InputStats* InputStatsCreate();
	void InputStatsDelete(InputStats* s);

	bool InputStatsCalculate(InputStats* stats, const void* spirvData, size_t spirvSize);
	bool InputStatsCalculateSmol(InputStats* stats, const void* smolvData, size_t smolvSize);
	void InputStatsPrint(const InputStats* stats);

} // namespace smolv
