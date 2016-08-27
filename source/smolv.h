// smol-v - v0.1 - public domain - https://github.com/aras-p/smol-v
// authored on 2016 by Aras Pranckevicius
// no warranty implied; use at your own risk

#pragma once

#include <stdint.h>
#include <vector>

namespace smolv
{
	typedef std::vector<uint8_t> ByteArray;

	bool Encode(const void* spirvData, size_t spirvSize, ByteArray& outSmolv);
	bool Decode(const void* smolvData, size_t smolvSize, ByteArray& outSpirv);

	struct InputStats;

	InputStats* InputStatsCreate();
	void InputStatsDelete(InputStats* s);

	bool InputStatsCalculate(InputStats* stats, const void* spirvData, size_t spirvSize);
	bool InputStatsCalculateSmol(InputStats* stats, const void* smolvData, size_t smolvSize);
	bool InputStatsRecordCompressedSize(InputStats* stats, const char* compressor, size_t compressedSize);
	void InputStatsPrint(const InputStats* stats);

} // namespace smolv
