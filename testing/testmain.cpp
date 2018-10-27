// smol-v - tests code - public domain - https://github.com/aras-p/smol-v
// authored on 2016 by Aras Pranckevicius
// no warranty implied; use at your own risk

#define _CRT_SECURE_NO_WARNINGS // yes MSVC, I want to use fopen
#include "../source/smolv.h"
#include "external/lz4/lz4.h"
#include "external/lz4/lz4hc.h"
#include "external/miniz/miniz.h"
#include "external/zstd/zstd.h"
#include "external/glslang/SPIRV/SPVRemapper.h"
#include <stdio.h>
#include <map>
#include <string>


typedef std::vector<uint8_t> ByteArray;


static void ReadFile(const char* fileName, ByteArray& output)
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

static void RemapSPIRV(const void* data, size_t size, bool strip, ByteArray& output)
{
	const uint32_t* dataI = (const uint32_t*)data;
	const size_t sizeI = size/4;
	std::vector<uint32_t> buf(dataI, dataI+sizeI);
	
	spv::spirvbin_t remapper;
	remapper.remap(buf, strip ? remapper.DO_EVERYTHING : remapper.ALL_BUT_STRIP);
	output.insert(output.end(), (const uint8_t*)buf.data(), ((const uint8_t*)buf.data()) + buf.size()*4);
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

static size_t CompressMiniz(const void* data, size_t size, int level = MZ_DEFAULT_LEVEL)
{
	if (size == 0)
		return 0;
	size_t bufferSize = mz_compressBound(size);
	unsigned char* buffer = new unsigned char[bufferSize];
	size_t resSize = bufferSize;
	int res = mz_compress2(buffer, &resSize, (const unsigned char*)data, size, level);
	delete[] buffer;
	if (res != MZ_OK)
		return -1;
	return resSize;
}

int main()
{
	smolv::Stats* stats = smolv::StatsCreate();

	#define TEST_UNITY 1
	#define TEST_TALOS 1
	#define TEST_DOTA2 1
	#define TEST_SHADERTOY 1
    #define TEST_DXC 1

	// files we're testing on
	const char* kFiles[] =
	{
		#if TEST_UNITY
		// Shaders produced by Unity's pipeline (HLSL -> DX11 bytecode -> HLSLcc -> glslang):
		// vertex shaders
		"unity/s0-0001-32333750.spirv",
		"unity/s0-0002-ca3af858.spirv",
		"unity/s0-0003-6ccb7b5e.spirv",
		"unity/s0-0004-9218583a.spirv",
		"unity/s0-0004-d2ba7e35.spirv",
		"unity/s0-0005-1eb77240.spirv",
		"unity/s0-0006-c36e44b3.spirv",
		"unity/s0-0007-3454fc86.spirv",
		"unity/s0-0007-ca8748eb.spirv",
		"unity/s0-0008-ecc6f669.spirv",
		"unity/s0-0009-e4ff870b.spirv",
		"unity/s0-0010-9379a08b.spirv",
		"unity/s0-0011-19016092.spirv",
		"unity/s0-0011-fe5ada8b.spirv",
		"unity/s0-0012-fba002b2.spirv",
		"unity/s0-0012-fc99d1e7.spirv",
		"unity/s0-0013-2190e4b3.spirv",
		"unity/s0-0013-aaccb753.spirv",
		"unity/s0-0013-b945d8f9.spirv",
		"unity/s0-0014-3ba60738.spirv",
		"unity/s0-0014-42341ba0.spirv",
		"unity/s0-0015-1b4af3ab.spirv",
		"unity/s0-0016-cc22f312.spirv",
		"unity/s0-0017-9d156c5b.spirv",
		"unity/s0-0018-267005da.spirv",
		"unity/s0-0019-68bab1cd.spirv",
		"unity/s0-0020-cb89e824.spirv",
		"unity/s0-0021-774ade5f.spirv",
		"unity/s0-0023-e226aa8a.spirv",
		"unity/s0-0024-9b5e5139.spirv",
		"unity/s0-0025-e1567d6d.spirv",
		"unity/s0-0026-717968d2.spirv",
		"unity/s0-0027-223d615c.spirv",
		"unity/s0-0028-290869cd.spirv",
		"unity/s0-0029-fc9c1174.spirv",
		"unity/s0-0031-1a0d9226.spirv",
		"unity/s0-0032-9f4fab1d.spirv",
		"unity/s0-0034-5c3d45dc.spirv",
		"unity/s0-0045-f2078956.spirv",
		"unity/s0-0046-0b926d9c.spirv",
		// fragment shaders
		"unity/s1-0000-5ca04fe4.spirv",
		"unity/s1-0000-cf9fe2e0.spirv",
		"unity/s1-0001-04d9d27b.spirv",
		"unity/s1-0001-aa1ecaf0.spirv",
		"unity/s1-0002-8d2ed6da.spirv",
		"unity/s1-0003-c54216ae.spirv",
		"unity/s1-0004-ac0f5549.spirv",
		"unity/s1-0005-93ebd823.spirv",
		"unity/s1-0006-85d79507.spirv",
		"unity/s1-0007-aff64c99.spirv",
		"unity/s1-0008-6e421249.spirv",
		"unity/s1-0009-0c858280.spirv",
		"unity/s1-0010-1b50ab90.spirv",
		"unity/s1-0011-2fed16ab.spirv",
		"unity/s1-0011-f3d46288.spirv",
		"unity/s1-0012-21b778d5.spirv",
		"unity/s1-0012-5428b42d.spirv",
		"unity/s1-0013-241e9fc8.spirv",
		"unity/s1-0013-35edd084.spirv",
		"unity/s1-0014-2ea8dc83.spirv",
		"unity/s1-0014-5c2d2a73.spirv",
		"unity/s1-0015-3b3a60bf.spirv",
		"unity/s1-0016-3c30e5e7.spirv",
		"unity/s1-0016-40492bcb.spirv",
		"unity/s1-0017-6c18345b.spirv",
		"unity/s1-0017-884cc79d.spirv",
		"unity/s1-0018-319798ba.spirv",
		"unity/s1-0019-1e7cb4ff.spirv",
		"unity/s1-0020-7363f5c5.spirv",
		"unity/s1-0021-e914f581.spirv",
		"unity/s1-0022-30eff697.spirv",
		"unity/s1-0024-0ce8d0e7.spirv",
		"unity/s1-0025-3c7f4035.spirv",
		"unity/s1-0026-1db01998.spirv",
		"unity/s1-0027-51fa0f15.spirv",
		"unity/s1-0028-84042ffc.spirv",
		"unity/s1-0029-8b7741ae.spirv",
		"unity/s1-0030-b919e80f.spirv",
		"unity/s1-0032-c069697f.spirv",
		"unity/s1-0033-0b804090.spirv",
		"unity/s1-0034-bad8bbff.spirv",
		"unity/s1-0035-e2a55d77.spirv",
		"unity/s1-0036-6bbcc1ac.spirv",
		"unity/s1-0037-e09cedbe.spirv",
		"unity/s1-0038-e85d5917.spirv",
		"unity/s1-0039-09bb7e61.spirv",
		"unity/s1-0041-834a25b0.spirv",
		"unity/s1-0043-026e1b4a.spirv",
		"unity/s1-0045-f49f5967.spirv",
		"unity/s1-0047-9c22101b.spirv",
		"unity/s1-0049-6dd06f97.spirv",
		"unity/s1-0052-d2e4133a.spirv",
		"unity/s1-0053-511daec1.spirv",
		"unity/s1-0054-b2b7e6c0.spirv",
		"unity/s1-0055-66f03021.spirv",
		"unity/s1-0056-89c781a9.spirv",
		"unity/s1-0057-87e01eae.spirv",
		"unity/s1-0062-e52c10e6.spirv",
		"unity/s1-0063-70e7171c.spirv",
		"unity/s1-0070-7595e017.spirv",
		"unity/s1-0074-e4935128.spirv",
		"unity/s1-0084-ffb8278d.spirv",
		// hull shaders
		"unity/s2-0004-76b9ef38.spirv",
		"unity/s2-0006-655ac983.spirv",
		"unity/s2-0019-3ddaf08d.spirv",
		"unity/s2-0031-412ed89d.spirv",
		// domain shaders
		"unity/s3-0008-09cef3e4.spirv",
		"unity/s3-0019-4c006911.spirv",
		"unity/s3-0022-f40e2e1e.spirv",
		"unity/s3-0028-e081a509.spirv",
		"unity/s3-0037-18f71ada.spirv",
		// geometry shaders
		"unity/s4-0004-6ec33743.spirv",
		"unity/s4-0006-a5e06270.spirv",
		#endif // #if TEST_UNITY

		#if TEST_TALOS
		// Shaders from The Talos Principle by Croteam:
		"talos/0078C470.shc",
		"talos/00DC0D6D.shc",
		"talos/0141C822.shc",
		"talos/04CA3D7B.shc",
		"talos/0A38A8F3.shc",
		"talos/0C958994.shc",
		"talos/0D8DD830.shc",
		"talos/12A491AE.shc",
		"talos/152916BF.shc",
		"talos/17A983B3.shc",
		"talos/17D83DB7.shc",
		"talos/181EB7F4.shc",
		"talos/18CD3426.shc",
		"talos/1AE632D2.shc",
		"talos/1AFB24CF.shc",
		"talos/1D86CEC1.shc",
		"talos/2789ADE0.shc",
		"talos/2CFAEA42.shc",
		"talos/2F1269A4.shc",
		"talos/3025BBCF.shc",
		"talos/324AA691.shc",
		"talos/3278D7A2.shc",
		"talos/35EB2F5D.shc",
		"talos/36BBB957.shc",
		"talos/38DA4FDC.shc",
		"talos/39645236.shc",
		"talos/397C22DE.shc",
		"talos/3FC9340A.shc",
		"talos/42ADB187.shc",
		"talos/43851D2E.shc",
		"talos/446D15D1.shc",
		"talos/48F3A85B.shc",
		"talos/4CF9349C.shc",
		"talos/4D3AFE1F.shc",
		"talos/4DF32AA3.shc",
		"talos/54A4CD9C.shc",
		"talos/5584AAD6.shc",
		"talos/560C8AAE.shc",
		"talos/58897F63.shc",
		"talos/5997DC95.shc",
		"talos/5C3ACFF1.shc",
		"talos/5F10146A.shc",
		"talos/62123EF6.shc",
		"talos/63846EDC.shc",
		"talos/6616D572.shc",
		"talos/68ADD87A.shc",
		"talos/6939EFB6.shc",
		"talos/6DE20E90.shc",
		"talos/71090A41.shc",
		"talos/721FAA4E.shc",
		"talos/72CF8B9E.shc",
		"talos/7311A988.shc",
		"talos/74D7C9BE.shc",
		"talos/7A632EA9.shc",
		"talos/7E791068.shc",
		"talos/7FB437EC.shc",
		"talos/862ABA13.shc",
		"talos/8698BECC.shc",
		"talos/8A8A2AD6.shc",
		"talos/8B4ABC28.shc",
		"talos/8E86B6D4.shc",
		"talos/907A8A15.shc",
		"talos/90AC21AB.shc",
		"talos/91D8BCE2.shc",
		"talos/9420BCB8.shc",
		"talos/9B8842CA.shc",
		"talos/9E5A3BEE.shc",
		"talos/A1B17C65.shc",
		"talos/A6D5DB71.shc",
		"talos/AA43457F.shc",
		"talos/AF45DFA4.shc",
		"talos/B115E1B7.shc",
		"talos/B1683E56.shc",
		"talos/B60DED50.shc",
		"talos/C3C6FB0C.shc",
		"talos/C55A421D.shc",
		"talos/C8A3D253.shc",
		"talos/CD4F5F10.shc",
		"talos/CDD726BF.shc",
		"talos/CE39D960.shc",
		"talos/D03CB186.shc",
		"talos/D071359B.shc",
		"talos/D0D4B04A.shc",
		"talos/D3A302F3.shc",
		"talos/D69AF138.shc",
		"talos/D8175C09.shc",
		"talos/D95FF51D.shc",
		"talos/DEDA997A.shc",
		"talos/E0B57835.shc",
		"talos/E367B56D.shc",
		"talos/E87626E9.shc",
		"talos/EDF61398.shc",
		"talos/F187344E.shc",
		"talos/F18905F7.shc",
		"talos/F4E8AB75.shc",
		"talos/F54184AD.shc",
		"talos/F757DA36.shc",
		"talos/F84FE6FD.shc",
		"talos/F9D3D588.shc",
		"talos/FDA341FA.shc",
		#endif // #if TEST_TALOS

		#if TEST_DOTA2
		// Shaders from DOTA2 by Valve Corporation:
		"dota2/1021831.spv",
		"dota2/1021921.spv",
		"dota2/1021937.spv",
		"dota2/1021957.spv",
		"dota2/1021982.spv",
		"dota2/1022104.spv",
		"dota2/1022164.spv",
		"dota2/1022300.spv",
		"dota2/1022326.spv",
		"dota2/1022372.spv",
		"dota2/11705.spv",
		"dota2/13490.spv",
		"dota2/13978.spv",
		"dota2/14074.spv",
		"dota2/145092.spv",
		"dota2/145323.spv",
		"dota2/14953.spv",
		"dota2/15212.spv",
		"dota2/153549.spv",
		"dota2/153702.spv",
		"dota2/153831.spv",
		"dota2/153884.spv",
		"dota2/156684.spv",
		"dota2/20451.spv",
		"dota2/21632.spv",
		"dota2/348634.spv",
		"dota2/366254.spv",
		"dota2/366294.spv",
		"dota2/366335.spv",
		"dota2/366455.spv",
		"dota2/366476.spv",
		"dota2/366477.spv",
		"dota2/366495.spv",
		"dota2/366515.spv",
		"dota2/366595.spv",
		"dota2/366734.spv",
		"dota2/366792.spv",
		"dota2/366871.spv",
		"dota2/367003.spv",
		"dota2/367029.spv",
		"dota2/367073.spv",
		"dota2/367079.spv",
		"dota2/367808.spv",
		"dota2/368065.spv",
		"dota2/368221.spv",
		"dota2/368894.spv",
		"dota2/368909.spv",
		"dota2/368930.spv",
		"dota2/368961.spv",
		"dota2/369162.spv",
		"dota2/369219.spv",
		"dota2/369220.spv",
		"dota2/369500.spv",
		"dota2/384867.spv",
		"dota2/459866.spv",
		"dota2/467711.spv",
		"dota2/496189.spv",
		"dota2/496971.spv",
		"dota2/497247.spv",
		"dota2/497531.spv",
		"dota2/497581.spv",
		"dota2/497587.spv",
		"dota2/497729.spv",
		"dota2/497875.spv",
		"dota2/498912.spv",
		"dota2/569461.spv",
		"dota2/569528.spv",
		"dota2/575430.spv",
		"dota2/582516.spv",
		"dota2/582644.spv",
		"dota2/582673.spv",
		"dota2/582715.spv",
		"dota2/582764.spv",
		"dota2/602523.spv",
		"dota2/608325.spv",
		"dota2/611688.spv",
		"dota2/61501.spv",
		"dota2/61536.spv",
		"dota2/61549.spv",
		"dota2/61906.spv",
		"dota2/61965.spv",
		"dota2/61988.spv",
		"dota2/61994.spv",
		"dota2/62032.spv",
		"dota2/62068.spv",
		"dota2/62081.spv",
		"dota2/62122.spv",
		"dota2/63939.spv",
		"dota2/659922.spv",
		"dota2/661085.spv",
		"dota2/788830.spv",
		"dota2/788923.spv",
		"dota2/789454.spv",
		"dota2/789505.spv",
		"dota2/789632.spv",
		"dota2/794725.spv",
		"dota2/802774.spv",
		"dota2/824933.spv",
		"dota2/826137.spv",
		"dota2/826321.spv",
		"dota2/826327.spv",
		"dota2/826339.spv",
		"dota2/826378.spv",
		"dota2/826431.spv",
		"dota2/827576.spv",
		"dota2/852315.spv",
		"dota2/878444.spv",
		"dota2/880389.spv",
		"dota2/922799.spv",
		"dota2/952476.spv",
		#endif // #if TEST_DOTA2
		
		#if TEST_SHADERTOY
		"shadertoy/st-4sfGWX.spv",
		"shadertoy/st-MdX3Rr.spv",
		"shadertoy/st-MdX3zr.spv",
		"shadertoy/st-MdlGW7.spv",
		"shadertoy/st-Mlt3Wn.spv",
		"shadertoy/st-Ms2SD1.spv",
		"shadertoy/st-MsXGWr.spv",
		"shadertoy/st-Msl3Rr.spv",
		"shadertoy/st-Mt3GWs.spv",
		"shadertoy/st-Xds3zN.spv",
		"shadertoy/st-XdsGDB.spv",
		"shadertoy/st-XltGDr.spv",
		"shadertoy/st-XsX3RB.spv",
		"shadertoy/st-XslGRr.spv",
		"shadertoy/st-XtlSD7.spv",
		"shadertoy/st-XtsSWs.spv",
		"shadertoy/st-ld3Gz2.spv",
		"shadertoy/st-lsSXzD.spv",
		#endif // #if TEST_SHADERTOY
        
        #if TEST_DXC
        // Shaders produced by Microsoft's DXC (shader model 6 -> vulkan 1.1)
        "dxc/imgui-vs.spv",
        #endif // #if TEST_DXC
	};

	// all test data lumped together, to check how well it compresses as a whole block
	ByteArray spirvAll;
	ByteArray spirvRemapAll[2];
	ByteArray smolvAll[2];

	// go over all test files
	int errorCount = 0;
	for (size_t i = 0; i < sizeof(kFiles)/sizeof(kFiles[0]); ++i)
	{
		// Read
		printf("Reading %s\n", kFiles[i]);
		ByteArray spirv;
		ReadFile((std::string("tests/spirv-dumps/") + kFiles[i]).c_str(), spirv);
		if (spirv.empty())
		{
			printf("ERROR: failed to read %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Basic SPIR-V input stats
		if (!smolv::StatsCalculate(stats, spirv.data(), spirv.size()))
		{
			printf("ERROR: failed to calc instruction stats (invalid SPIR-V?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Encode to SMOL-V
		ByteArray smolv;
		if (!smolv::Encode(spirv.data(), spirv.size(), smolv, 0))
		{
			printf("ERROR: failed to encode (invalid invalid SPIR-V?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Decode back to SPIR-V
		size_t spirvDecodedSize = smolv::GetDecodedBufferSize(smolv.data(), smolv.size());
		ByteArray spirvDecoded;
		spirvDecoded.resize(spirvDecodedSize);
		if (!smolv::Decode(smolv.data(), smolv.size(), spirvDecoded.data(), spirvDecodedSize))
		{
			printf("ERROR: failed to decode back (bug?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Check that it decoded 100% the same
		if (spirv != spirvDecoded)
		{
			printf("ERROR: did not encode+decode properly (bug?) %s\n", kFiles[i]);
			const uint8_t* spirvPtr = spirv.data();
			const uint8_t* spirvPtrEnd = spirvPtr + spirv.size();
			const uint8_t* spirvDecodedPtr = spirvDecoded.data();
			const uint8_t* spirvDecodedEnd = spirvDecodedPtr + spirvDecoded.size();
			int idx = 0;
			while (spirvPtr < spirvPtrEnd && spirvDecodedPtr < spirvDecodedEnd)
			{
				if (*spirvPtr != *spirvDecodedPtr)
				{
					printf("  byte #%04i %02x -> %02x\n", idx, *spirvPtr, *spirvDecodedPtr);
				}
				++spirvPtr;
				++spirvDecodedPtr;
				++idx;
			}
			
			++errorCount;
			break;
		}

		// Encode to SMOL-V, with debug info stripping
		ByteArray smolvStripped;
		if (!smolv::Encode(spirv.data(), spirv.size(), smolvStripped, smolv::kEncodeFlagStripDebugInfo))
		{
			printf("ERROR: failed to encode with stripping (invalid invalid SPIR-V?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// SMOL encoding stats
		if (!smolv::StatsCalculateSmol(stats, smolvStripped.data(), smolvStripped.size()))
		{
			printf("ERROR: failed to calc SMOLV instruction stats (bug?) %s\n", kFiles[i]);
			++errorCount;
			break;
		}

		// Append to "whole blob" arrays
		spirvAll.insert(spirvAll.end(), spirv.begin(), spirv.end());
		smolvAll[0].insert(smolvAll[0].end(), smolv.begin(), smolv.end());
		smolvAll[1].insert(smolvAll[1].end(), smolvStripped.begin(), smolvStripped.end());
		RemapSPIRV(spirv.data(), spirv.size(), false, spirvRemapAll[0]);
		RemapSPIRV(spirv.data(), spirv.size(), true, spirvRemapAll[1]);
	}
	
	if (errorCount != 0)
	{
		printf("Got ERRORS: %i\n", errorCount);
		smolv::StatsDelete(stats);
		return 1;
	}
	
	// Print instruction stats
	smolv::StatsPrint(stats);
	smolv::StatsDelete(stats);

	// Compress various ways (as a whole blob) and print sizes
	for (int i = 0; i < 2; ++i)
	{
		printf("Compressing %s...\n", i==0 ? "RAW" : "STRIPPED");
		typedef std::map<std::string, size_t> CompressorSizeMap;
		CompressorSizeMap sizes;

		;;sizes["0 Remap"] = spirvRemapAll[i].size();
		sizes["0 SMOL-V"] = smolvAll[i].size();

		;;sizes["1    zlib"] = CompressMiniz(spirvAll.data(), spirvAll.size());
		;;sizes["1 re+zlib"] = CompressMiniz(spirvRemapAll[i].data(), spirvRemapAll[i].size());
		sizes["1 sm+zlib"] = CompressMiniz(smolvAll[i].data(), smolvAll[i].size());

		;;sizes["2    LZ4HC"] = CompressLZ4HC(spirvAll.data(), spirvAll.size());
		;;sizes["2 re+LZ4HC"] = CompressLZ4HC(spirvRemapAll[i].data(), spirvRemapAll[i].size());
		sizes["2 sm+LZ4HC"] = CompressLZ4HC(smolvAll[i].data(), smolvAll[i].size());

		;;sizes["3    Zstd"] = CompressZstd(spirvAll.data(), spirvAll.size());
		;;sizes["3 re+Zstd"] = CompressZstd(spirvRemapAll[i].data(), spirvRemapAll[i].size());
		sizes["3 sm+Zstd"] = CompressZstd(smolvAll[i].data(), smolvAll[i].size());

		;;sizes["4    Zstd20"] = CompressZstd(spirvAll.data(), spirvAll.size(), 20);
		;;sizes["4 re+Zstd20"] = CompressZstd(spirvRemapAll[i].data(), spirvRemapAll[i].size(), 20);
		sizes["4 sm+Zstd20"] = CompressZstd(smolvAll[i].data(), smolvAll[i].size(), 20);
		
		
		printf("Original size: %.1fKB\n", spirvAll.size()/1024.0f);
		for (CompressorSizeMap::const_iterator it = sizes.begin(), itEnd = sizes.end(); it != itEnd; ++it)
		{
			printf("%-13s %6.1fKB %5.1f%%\n", it->first.c_str(), it->second/1024.0f, (float)it->second/(float)(spirvAll.size())*100.0f);
		}
	}
	

	return 0;
}
