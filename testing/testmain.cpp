#include "../source/smolv.h"
#include "external/lz4.h"
#include "external/lz4hc.h"


typedef std::vector<uint8_t> ByteArray;

ByteArray ReadFile(const char* fileName)
{
	ByteArray arr;
	FILE* f = fopen(fileName, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		fseek(f, 0, SEEK_SET);
		arr.resize(size);
		fread(arr.data(), size, 1, f);
		fclose(f);
	}
	return arr;
}

int main()
{
	smolv::InputStats* stats = smolv::InputStatsCreate();

	const char* kFiles[] =
	{
		"tests/spirv-dumps/s0-0016-cc22f312.spirv",
		"tests/spirv-dumps/s0-0025-e1567d6d.spirv",
		"tests/spirv-dumps/s0-0028-290869cd.spirv",
		"tests/spirv-dumps/s0-0045-f2078956.spirv",
		"tests/spirv-dumps/s1-0001-04d9d27b.spirv",
		"tests/spirv-dumps/s1-0009-0c858280.spirv",
		"tests/spirv-dumps/s1-0012-5428b42d.spirv",
		"tests/spirv-dumps/s1-0052-d2e4133a.spirv",
		"tests/spirv-dumps/s1-0084-ffb8278d.spirv",
		"tests/spirv-dumps/s2-0019-3ddaf08d.spirv",
		"tests/spirv-dumps/s3-0022-f40e2e1e.spirv",
		"tests/spirv-dumps/s4-0006-a5e06270.spirv",
	};
	for (size_t i = 0; i < sizeof(kFiles)/sizeof(kFiles[0]); ++i)
	{
		printf("Reading %s\n", kFiles[i]);
		ByteArray spirv = ReadFile(kFiles[i]);
		if (spirv.empty())
		{
			printf("ERROR: failed to read %s\n", kFiles[i]);
			break;
		}
		if (!smolv::InputStatsCalculate(stats, spirv.data(), spirv.size()))
		{
			printf("ERROR: invalid SPIR-V %s\n", kFiles[i]);
			break;
		}

		int spirvSize = (int)spirv.size();
		int maxLZ4Size = LZ4_compressBound(spirvSize);
		char* destLZ4HC = new char[maxLZ4Size];
		int sizeLZ4HC = LZ4_compress_HC ((const char*)spirv.data(), destLZ4HC, spirvSize, maxLZ4Size, 0);
		smolv::InputStatsRecordCompressedSize(stats, "LZ4HC", sizeLZ4HC);
		delete[] destLZ4HC;
		printf("\n");
	}
	smolv::InputStatsPrint(stats);
	
	smolv::InputStatsDelete(stats);
	return 0;
}
