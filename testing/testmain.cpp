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
	if (size == 0)
		return 0;
	int bufferSize = LZ4_compressBound((int)size);
	char* buffer = new char[bufferSize];
	size_t resSize = LZ4_compress_HC ((const char*)data, buffer, (int)size, bufferSize, level);
	delete[] buffer;
	return resSize;
}

static size_t CompressZstd(const void* data, size_t size, int level = 0)
{
	if (size == 0)
		return 0;
	size_t bufferSize = ZSTD_compressBound(size);
	char* buffer = new char[bufferSize];
	size_t resSize = ZSTD_compress(buffer, bufferSize, data, size, level);
	delete[] buffer;
	return resSize;
}


int main()
{
	smolv::InputStats* stats = smolv::InputStatsCreate();

	// files we're testing on
	const char* kFiles[] =
	{
		"tests/spirv-dumps/s0-0001-32333750.spirv",
		"tests/spirv-dumps/s0-0003-6ccb7b5e.spirv",
		"tests/spirv-dumps/s0-0004-9218583a.spirv",
		"tests/spirv-dumps/s0-0004-d2ba7e35.spirv",
		"tests/spirv-dumps/s0-0006-c36e44b3.spirv",
		"tests/spirv-dumps/s0-0007-3454fc86.spirv",
		"tests/spirv-dumps/s0-0007-ca8748eb.spirv",
		"tests/spirv-dumps/s0-0008-ecc6f669.spirv",
		"tests/spirv-dumps/s0-0009-e4ff870b.spirv",
		"tests/spirv-dumps/s0-0011-19016092.spirv",
		"tests/spirv-dumps/s0-0011-fe5ada8b.spirv",
		"tests/spirv-dumps/s0-0012-fba002b2.spirv",
		"tests/spirv-dumps/s0-0013-2190e4b3.spirv",
		"tests/spirv-dumps/s0-0013-b945d8f9.spirv",
		"tests/spirv-dumps/s0-0014-3ba60738.spirv",
		"tests/spirv-dumps/s0-0015-1b4af3ab.spirv",
		"tests/spirv-dumps/s0-0016-cc22f312.spirv",
		"tests/spirv-dumps/s0-0017-9d156c5b.spirv",
		"tests/spirv-dumps/s0-0018-267005da.spirv",
		"tests/spirv-dumps/s0-0019-68bab1cd.spirv",
		"tests/spirv-dumps/s0-0021-774ade5f.spirv",
		"tests/spirv-dumps/s0-0024-9b5e5139.spirv",
		"tests/spirv-dumps/s0-0025-e1567d6d.spirv",
		"tests/spirv-dumps/s0-0028-290869cd.spirv",
		"tests/spirv-dumps/s0-0031-1a0d9226.spirv",
		"tests/spirv-dumps/s0-0032-9f4fab1d.spirv",
		"tests/spirv-dumps/s0-0045-f2078956.spirv",
		"tests/spirv-dumps/s0-0046-0b926d9c.spirv",

		"tests/spirv-dumps/s1-0001-04d9d27b.spirv",
		"tests/spirv-dumps/s1-0009-0c858280.spirv",
		"tests/spirv-dumps/s1-0012-5428b42d.spirv",
		"tests/spirv-dumps/s1-0052-d2e4133a.spirv",
		"tests/spirv-dumps/s1-0084-ffb8278d.spirv",
		"tests/spirv-dumps/s2-0019-3ddaf08d.spirv",
		"tests/spirv-dumps/s3-0022-f40e2e1e.spirv",
		"tests/spirv-dumps/s4-0006-a5e06270.spirv",
	};

	// all test data lumped together, to check how well it compresses as a whole block
	ByteArray spirvAll;
	ByteArray smolvAll;

	// go over all test files
	int errorCount = 0;
	for (size_t i = 0; i < sizeof(kFiles)/sizeof(kFiles[0]); ++i)
	{
		// Read
		printf("Reading %s\n", kFiles[i]);
		ByteArray spirv;
		ReadFile(kFiles[i], spirv);
		if (spirv.empty())
		{
			printf("ERROR: failed to read %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Basic SPIR-V input stats
		if (!smolv::InputStatsCalculate(stats, spirv.data(), spirv.size()))
		{
			printf("ERROR: failed to calc instruction stats (invalid SPIR-V?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Encode to SMOL-V
		ByteArray smolv;
		if (!smolv::Encode(spirv.data(), spirv.size(), smolv))
		{
			printf("ERROR: failed to encode (invalid invalid SPIR-V?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Decode back to SPIR-V
		ByteArray spirvDecoded;
		if (!smolv::Decode(smolv.data(), smolv.size(), spirvDecoded))
		{
			printf("ERROR: failed to decode back (bug?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Check that it decoded 100% the same
		if (spirv != spirvDecoded)
		{
			printf("ERROR: did not encode+decode properly (bug?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		spirvAll.insert(spirvAll.end(), spirv.begin(), spirv.end());
		smolvAll.insert(smolvAll.end(), smolv.begin(), smolv.end());
		
		printf("\n");
	}

	// Compress various ways (as a whole blob) and record sizes
	smolv::InputStatsRecordCompressedSize(stats, "0 SMOL", smolvAll.size());

	smolv::InputStatsRecordCompressedSize(stats, "1 LZ4HC", CompressLZ4HC(spirvAll.data(), spirvAll.size()));
	smolv::InputStatsRecordCompressedSize(stats, "2 smLZ4HC", CompressLZ4HC(smolvAll.data(), smolvAll.size()));

	smolv::InputStatsRecordCompressedSize(stats, "3 Zstd", CompressZstd(spirvAll.data(), spirvAll.size()));
	smolv::InputStatsRecordCompressedSize(stats, "4 smZstd", CompressZstd(smolvAll.data(), smolvAll.size()));

	smolv::InputStatsRecordCompressedSize(stats, "5 Zstd20", CompressZstd(spirvAll.data(), spirvAll.size(), 20));
	smolv::InputStatsRecordCompressedSize(stats, "6 smZstd20", CompressZstd(smolvAll.data(), smolvAll.size(), 20));
	
	smolv::InputStatsPrint(stats);
	
	smolv::InputStatsDelete(stats);

	if (errorCount != 0)
	{
		printf("Got ERRORS: %i\n", errorCount);
		return 1;
	}
	return 0;
}
