#pragma once

#include <stdint.h>
#include <vector>

namespace smolv
{

	struct InputStats;

	InputStats* InputStatsCreate();
	void InputStatsDelete(InputStats* s);

	bool InputStatsCalculate(InputStats* stats, const void* spirvData, size_t spirvSize);
	bool InputStatsRecordCompressedSize(InputStats* stats, const char* compressor, size_t compressedSize);
	void InputStatsPrint(const InputStats* stats);


	//bool SmolvCompress(const void* spirvData, size_t spirvSize, std::vector<uint8_t>& outSmolv);

} // namespace smolv
