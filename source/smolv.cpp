#include "smolv.h"
#include "spirv.h"
#include <stdint.h>
#include <vector>
#include <map>
#include <string>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

static const char* kSpirvOpNames[] =
{
	"Nop",
	"Undef",
	"SourceContinued",
	"Source",
	"SourceExtension",
	"Name",
	"MemberName",
	"String",
	"Line",
	"#9",
	"Extension",
	"ExtInstImport",
	"ExtInst",
	"#13",
	"MemoryModel",
	"EntryPoint",
	"ExecutionMode",
	"Capability",
	"#18",
	"TypeVoid",
	"TypeBool",
	"TypeInt",
	"TypeFloat",
	"TypeVector",
	"TypeMatrix",
	"TypeImage",
	"TypeSampler",
	"TypeSampledImage",
	"TypeArray",
	"TypeRuntimeArray",
	"TypeStruct",
	"TypeOpaque",
	"TypePointer",
	"TypeFunction",
	"TypeEvent",
	"TypeDeviceEvent",
	"TypeReserveId",
	"TypeQueue",
	"TypePipe",
	"TypeForwardPointer",
	"#40",
	"ConstantTrue",
	"ConstantFalse",
	"Constant",
	"ConstantComposite",
	"ConstantSampler",
	"ConstantNull",
	"#47",
	"SpecConstantTrue",
	"SpecConstantFalse",
	"SpecConstant",
	"SpecConstantComposite",
	"SpecConstantOp",
	"#53",
	"Function",
	"FunctionParameter",
	"FunctionEnd",
	"FunctionCall",
	"#58",
	"Variable",
	"ImageTexelPointer",
	"Load",
	"Store",
	"CopyMemory",
	"CopyMemorySized",
	"AccessChain",
	"InBoundsAccessChain",
	"PtrAccessChain",
	"ArrayLength",
	"GenericPtrMemSemantics",
	"InBoundsPtrAccessChain",
	"Decorate",
	"MemberDecorate",
	"DecorationGroup",
	"GroupDecorate",
	"GroupMemberDecorate",
	"#76",
	"VectorExtractDynamic",
	"VectorInsertDynamic",
	"VectorShuffle",
	"CompositeConstruct",
	"CompositeExtract",
	"CompositeInsert",
	"CopyObject",
	"Transpose",
	"#85",
	"SampledImage",
	"ImageSampleImplicitLod",
	"ImageSampleExplicitLod",
	"ImageSampleDrefImplicitLod",
	"ImageSampleDrefExplicitLod",
	"ImageSampleProjImplicitLod",
	"ImageSampleProjExplicitLod",
	"ImageSampleProjDrefImplicitLod",
	"ImageSampleProjDrefExplicitLod",
	"ImageFetch",
	"ImageGather",
	"ImageDrefGather",
	"ImageRead",
	"ImageWrite",
	"Image",
	"ImageQueryFormat",
	"ImageQueryOrder",
	"ImageQuerySizeLod",
	"ImageQuerySize",
	"ImageQueryLod",
	"ImageQueryLevels",
	"ImageQuerySamples",
	"#108",
	"ConvertFToU",
	"ConvertFToS",
	"ConvertSToF",
	"ConvertUToF",
	"UConvert",
	"SConvert",
	"FConvert",
	"QuantizeToF16",
	"ConvertPtrToU",
	"SatConvertSToU",
	"SatConvertUToS",
	"ConvertUToPtr",
	"PtrCastToGeneric",
	"GenericCastToPtr",
	"GenericCastToPtrExplicit",
	"Bitcast",
	"#125",
	"SNegate",
	"FNegate",
	"IAdd",
	"FAdd",
	"ISub",
	"FSub",
	"IMul",
	"FMul",
	"UDiv",
	"SDiv",
	"FDiv",
	"UMod",
	"SRem",
	"SMod",
	"FRem",
	"FMod",
	"VectorTimesScalar",
	"MatrixTimesScalar",
	"VectorTimesMatrix",
	"MatrixTimesVector",
	"MatrixTimesMatrix",
	"OuterProduct",
	"Dot",
	"IAddCarry",
	"ISubBorrow",
	"UMulExtended",
	"SMulExtended",
	"#153",
	"Any",
	"All",
	"IsNan",
	"IsInf",
	"IsFinite",
	"IsNormal",
	"SignBitSet",
	"LessOrGreater",
	"Ordered",
	"Unordered",
	"LogicalEqual",
	"LogicalNotEqual",
	"LogicalOr",
	"LogicalAnd",
	"LogicalNot",
	"Select",
	"IEqual",
	"INotEqual",
	"UGreaterThan",
	"SGreaterThan",
	"UGreaterThanEqual",
	"SGreaterThanEqual",
	"ULessThan",
	"SLessThan",
	"ULessThanEqual",
	"SLessThanEqual",
	"FOrdEqual",
	"FUnordEqual",
	"FOrdNotEqual",
	"FUnordNotEqual",
	"FOrdLessThan",
	"FUnordLessThan",
	"FOrdGreaterThan",
	"FUnordGreaterThan",
	"FOrdLessThanEqual",
	"FUnordLessThanEqual",
	"FOrdGreaterThanEqual",
	"FUnordGreaterThanEqual",
	"#192",
	"#193",
	"ShiftRightLogical",
	"ShiftRightArithmetic",
	"ShiftLeftLogical",
	"BitwiseOr",
	"BitwiseXor",
	"BitwiseAnd",
	"Not",
	"BitFieldInsert",
	"BitFieldSExtract",
	"BitFieldUExtract",
	"BitReverse",
	"BitCount",
	"#206",
	"DPdx",
	"DPdy",
	"Fwidth",
	"DPdxFine",
	"DPdyFine",
	"FwidthFine",
	"DPdxCoarse",
	"DPdyCoarse",
	"FwidthCoarse",
	"#216",
	"#217",
	"EmitVertex",
	"EndPrimitive",
	"EmitStreamVertex",
	"EndStreamPrimitive",
	"#222",
	"#223",
	"ControlBarrier",
	"MemoryBarrier",
	"#226",
	"AtomicLoad",
	"AtomicStore",
	"AtomicExchange",
	"AtomicCompareExchange",
	"AtomicCompareExchangeWeak",
	"AtomicIIncrement",
	"AtomicIDecrement",
	"AtomicIAdd",
	"AtomicISub",
	"AtomicSMin",
	"AtomicUMin",
	"AtomicSMax",
	"AtomicUMax",
	"AtomicAnd",
	"AtomicOr",
	"AtomicXor",
	"#243",
	"#244",
	"Phi",
	"LoopMerge",
	"SelectionMerge",
	"Label",
	"Branch",
	"BranchConditional",
	"Switch",
	"Kill",
	"Return",
	"ReturnValue",
	"Unreachable",
	"LifetimeStart",
	"LifetimeStop",
	"#258",
	"GroupAsyncCopy",
	"GroupWaitEvents",
	"GroupAll",
	"GroupAny",
	"GroupBroadcast",
	"GroupIAdd",
	"GroupFAdd",
	"GroupFMin",
	"GroupUMin",
	"GroupSMin",
	"GroupFMax",
	"GroupUMax",
	"GroupSMax",
	"#272",
	"#273",
	"ReadPipe",
	"WritePipe",
	"ReservedReadPipe",
	"ReservedWritePipe",
	"ReserveReadPipePackets",
	"ReserveWritePipePackets",
	"CommitReadPipe",
	"CommitWritePipe",
	"IsValidReserveId",
	"GetNumPipePackets",
	"GetMaxPipePackets",
	"GroupReserveReadPipePackets",
	"GroupReserveWritePipePackets",
	"GroupCommitReadPipe",
	"GroupCommitWritePipe",
	"#289",
	"#290",
	"EnqueueMarker",
	"EnqueueKernel",
	"GetKernelNDrangeSubGroupCount",
	"GetKernelNDrangeMaxSubGroupSize",
	"GetKernelWorkGroupSize",
	"GetKernelPreferredWorkGroupSizeMultiple",
	"RetainEvent",
	"ReleaseEvent",
	"CreateUserEvent",
	"IsValidEvent",
	"SetUserEventStatus",
	"CaptureEventProfilingInfo",
	"GetDefaultQueue",
	"BuildNDRange",
	"ImageSparseSampleImplicitLod",
	"ImageSparseSampleExplicitLod",
	"ImageSparseSampleDrefImplicitLod",
	"ImageSparseSampleDrefExplicitLod",
	"ImageSparseSampleProjImplicitLod",
	"ImageSparseSampleProjExplicitLod",
	"ImageSparseSampleProjDrefImplicitLod",
	"ImageSparseSampleProjDrefExplicitLod",
	"ImageSparseFetch",
	"ImageSparseGather",
	"ImageSparseDrefGather",
	"ImageSparseTexelsResident",
	"NoLine",
	"AtomicFlagTestAndSet",
	"AtomicFlagClear",
	"ImageSparseRead",
	"SizeOf",
	"TypePipeStorage",
	"ConstantPipeStorage",
	"CreatePipeFromPipeStorage",
	"GetKernelLocalSizeForSubgroupCount",
	"GetKernelMaxNumSubgroups",
	"TypeNamedBarrier",
	"NamedBarrierInitialize",
	"MemoryNamedBarrier",
	"ModuleProcessed",
};

const int kKnownOpsCount = ARRAY_SIZE(kSpirvOpNames);


struct smolv::InputStats
{
	size_t opCounts[kKnownOpsCount] = {};
	size_t opSizes[kKnownOpsCount] = {};
	size_t totalOps = 0;
	size_t totalSize = 0;
	size_t inputCount = 0;
	std::map<std::string, size_t> compressedSizes;
};


smolv::InputStats* smolv::InputStatsCreate()
{
	return new InputStats();
}

void smolv::InputStatsDelete(smolv::InputStats *s)
{
	delete s;
}


static bool smolv_CheckSpirVHeader(const uint32_t* words, size_t wordCount)
{
	if (wordCount < 5)
		return false;

	uint32_t headerMagic = words[0];
	//@TODO: big endian
	if (headerMagic != 0x07230203)
		return false;
	uint32_t headerVersion = words[1];
	if (headerVersion != 0x00010000 && headerVersion != 0x00010100)
		return false; // only support 1.0 and 1.1

	return true;
}


bool smolv::InputStatsCalculate(smolv::InputStats* stats, const void* spirvData, size_t spirvSize)
{
	if (!stats)
		return false;

	const size_t wordCount = spirvSize / 4;
	const uint32_t* words = (const uint32_t*)spirvData;
	const uint32_t* wordsEnd = words + wordCount;
	if (!smolv_CheckSpirVHeader(words, wordCount))
		return false;
	
	// go over instructions
	stats->inputCount++;
	stats->totalSize += wordCount;

	printf("%4s %-20s Ln\n", "Offs", "Op");
	size_t offset = 5;
	words += 5;
	while (words < wordsEnd)
	{
		uint32_t instrLen = words[0] >> 16;
		if (instrLen < 1)
			return false; // malformed instruction, lenght needs to be at least 1
		if (instrLen > wordCount)
			return false; // malformed instruction, goes past end of data
		SpvOp op = (SpvOp)(words[0] & 0xFFFF);
		if (op < kKnownOpsCount)
		{
			printf("%04i %-20s %2i\n", (int)offset, kSpirvOpNames[op], instrLen);
			stats->opCounts[op]++;
			stats->opSizes[op] += instrLen;
		}
		else
		{
			printf("%04i Op#%i %2i\n", (int)offset, op, instrLen);
		}
		words += instrLen;
		offset += instrLen;
		stats->totalOps++;
	}
	
	return true;
}


bool smolv::InputStatsRecordCompressedSize(InputStats* stats, const char* compressor, size_t compressedSize)
{
	if (!stats)
		return false;
	stats->compressedSizes[std::string(compressor)] += compressedSize;
	return true;
}


void smolv::InputStatsPrint(const InputStats* stats)
{
	if (!stats)
		return;

	typedef std::pair<SpvOp,size_t> OpCounter;
	OpCounter counts[kKnownOpsCount];
	OpCounter sizes[kKnownOpsCount];
	for (int i = 0; i < kKnownOpsCount; ++i)
	{
		counts[i].first = (SpvOp)i;
		counts[i].second = stats->opCounts[i];
		sizes[i].first = (SpvOp)i;
		sizes[i].second = stats->opSizes[i];
	}
	std::sort(counts, counts + kKnownOpsCount, [](OpCounter a, OpCounter b) { return a.second > b.second; });
	std::sort(sizes, sizes + kKnownOpsCount, [](OpCounter a, OpCounter b) { return a.second > b.second; });
	
	printf("Stats for %i SPIR-V inputs, total size %i words (%.1fKB):\n", (int)stats->inputCount, (int)stats->totalSize, stats->totalSize * 4.0f / 1024.0f);
	printf("Most occuring ops:\n");
	for (int i = 0; i < 15; ++i)
	{
		SpvOp op = counts[i].first;
		printf(" #%2i: %-20s %4i (%4.1f%%)\n", i, kSpirvOpNames[op], (int)counts[i].second, (float)counts[i].second / (float)stats->totalOps * 100.0f);
	}
	printf("Largest total size of ops:\n");
	for (int i = 0; i < 15; ++i)
	{
		SpvOp op = sizes[i].first;
		printf(" #%2i: %-20s %4i (%4.1f%%) avg len %.1f\n",
			   i,
			   kSpirvOpNames[op],
			   (int)sizes[i].second,
			   (float)sizes[i].second / (float)stats->totalSize * 100.0f,
			   (float)sizes[i].second / (float)stats->opCounts[op]
		);
	}
	
	printf("Compression: original size %.1fKB\n", stats->totalSize*4.0f/1024.0f);
	for (auto&& kv : stats->compressedSizes)
	{
		printf("%6s: %5.1fKB (%5.1f%%)\n", kv.first.c_str(), kv.second/1024.0f, (float)kv.second/(stats->totalSize*4.0f)*100.0f);
	}
}

