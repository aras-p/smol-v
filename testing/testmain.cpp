#include "../source/smolv.h"
#include "external/lz4/lz4.h"
#include "external/lz4/lz4hc.h"
#include "external/zstd/zstd.h"


typedef std::vector<uint8_t> ByteArray;


void ReadFile(const char* fileName, ByteArray& output)
{
	FILE* f = fopen(fileName, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		fseek(f, 0, SEEK_SET);
		size_t pos = output.size();
		output.resize(pos + size);
		fread(output.data() + pos, size, 1, f);
		fclose(f);
	}
}


static size_t CompressLZ4HC(const void* data, size_t size, int level = 0)
{
	int bufferSize = LZ4_compressBound((int)size);
	char* buffer = new char[bufferSize];
	size_t resSize = LZ4_compress_HC ((const char*)data, buffer, (int)size, bufferSize, level);
	delete[] buffer;
	return resSize;
}

static size_t CompressZstd(const void* data, size_t size, int level = 0)
{
	size_t bufferSize = ZSTD_compressBound(size);
	char* buffer = new char[bufferSize];
	size_t resSize = ZSTD_compress(buffer, bufferSize, data, size, level);
	delete[] buffer;
	return resSize;
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
	ByteArray spirvAll;
	for (size_t i = 0; i < sizeof(kFiles)/sizeof(kFiles[0]); ++i)
	{
		printf("Reading %s\n", kFiles[i]);
		ByteArray spirv;
		ReadFile(kFiles[i], spirv);
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

		spirvAll.insert(spirvAll.end(), spirv.begin(), spirv.end());
		
		smolv::InputStatsRecordCompressedSize(stats, "LZ4HC", CompressLZ4HC(spirv.data(), spirv.size()));
		smolv::InputStatsRecordCompressedSize(stats, "LZ4HC15", CompressLZ4HC(spirv.data(), spirv.size(), 16));
		smolv::InputStatsRecordCompressedSize(stats, "Zstd", CompressZstd(spirv.data(), spirv.size()));
		smolv::InputStatsRecordCompressedSize(stats, "Zstd12", CompressZstd(spirv.data(), spirv.size(), 12));
		smolv::InputStatsRecordCompressedSize(stats, "Zstd20", CompressZstd(spirv.data(), spirv.size(), 20));
		printf("\n");
	}

	smolv::InputStatsRecordCompressedSize(stats, "__LZ4HC", CompressLZ4HC(spirvAll.data(), spirvAll.size()));
	smolv::InputStatsRecordCompressedSize(stats, "__LZ4HC15", CompressLZ4HC(spirvAll.data(), spirvAll.size(), 16));
	smolv::InputStatsRecordCompressedSize(stats, "__Zstd", CompressZstd(spirvAll.data(), spirvAll.size()));
	smolv::InputStatsRecordCompressedSize(stats, "__Zstd12", CompressZstd(spirvAll.data(), spirvAll.size(), 12));
	smolv::InputStatsRecordCompressedSize(stats, "__Zstd20", CompressZstd(spirvAll.data(), spirvAll.size(), 20));
	
	smolv::InputStatsPrint(stats);
	
	smolv::InputStatsDelete(stats);
	return 0;
}
