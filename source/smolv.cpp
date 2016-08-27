#include "smolv.h"
#include "spirv.h"
#include <stdint.h>
#include <vector>
#include <map>
#include <string>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))


// --------------------------------------------------------------------------------------------
// Metadata about known SPIR-V operations

static const size_t kKnownOpsCount = SpvOpModuleProcessed+1;


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
static_assert(ARRAY_SIZE(kSpirvOpNames) == kKnownOpsCount, "kSpirvOpNames table mismatch with known SpvOps");

static const char* smolv_GetOpName(SpvOp op)
{
	if (op < 0 || op >= kKnownOpsCount)
		return "???";
	return kSpirvOpNames[op];
}

struct OpData
{
	uint8_t hasResult;
	uint8_t hasType;
};
static const OpData kSpirvOpData[] =
{
	{0,0}, // Nop
	{1,1}, // Undef
	{0,0}, // SourceContinued
	{0,0}, // Source
	{0,0}, // SourceExtension
	{0,0}, // Name
	{0,0}, // MemberName
	{0,0}, // String
	{0,0}, // Line
	{1,1}, // #9
	{0,0}, // Extension
	{1,0}, // ExtInstImport
	{1,1}, // ExtInst
	{1,1}, // #13
	{0,0}, // MemoryModel
	{0,0}, // EntryPoint
	{0,0}, // ExecutionMode
	{0,0}, // Capability
	{1,1}, // #18
	{1,0}, // TypeVoid
	{1,0}, // TypeBool
	{1,0}, // TypeInt
	{1,0}, // TypeFloat
	{1,0}, // TypeVector
	{1,0}, // TypeMatrix
	{1,0}, // TypeImage
	{1,0}, // TypeSampler
	{1,0}, // TypeSampledImage
	{1,0}, // TypeArray
	{1,0}, // TypeRuntimeArray
	{1,0}, // TypeStruct
	{1,0}, // TypeOpaque
	{1,0}, // TypePointer
	{1,0}, // TypeFunction
	{1,0}, // TypeEvent
	{1,0}, // TypeDeviceEvent
	{1,0}, // TypeReserveId
	{1,0}, // TypeQueue
	{1,0}, // TypePipe
	{0,0}, // TypeForwardPointer
	{1,1}, // #40
	{1,1}, // ConstantTrue
	{1,1}, // ConstantFalse
	{1,1}, // Constant
	{1,1}, // ConstantComposite
	{1,1}, // ConstantSampler
	{1,1}, // ConstantNull
	{1,1}, // #47
	{1,1}, // SpecConstantTrue
	{1,1}, // SpecConstantFalse
	{1,1}, // SpecConstant
	{1,1}, // SpecConstantComposite
	{1,1}, // SpecConstantOp
	{1,1}, // #53
	{1,1}, // Function
	{1,1}, // FunctionParameter
	{0,0}, // FunctionEnd
	{1,1}, // FunctionCall
	{1,1}, // #58
	{1,1}, // Variable
	{1,1}, // ImageTexelPointer
	{1,1}, // Load
	{0,0}, // Store
	{0,0}, // CopyMemory
	{0,0}, // CopyMemorySized
	{1,1}, // AccessChain
	{1,1}, // InBoundsAccessChain
	{1,1}, // PtrAccessChain
	{1,1}, // ArrayLength
	{1,1}, // GenericPtrMemSemantics
	{1,1}, // InBoundsPtrAccessChain
	{0,0}, // Decorate
	{0,0}, // MemberDecorate
	{1,0}, // DecorationGroup
	{0,0}, // GroupDecorate
	{0,0}, // GroupMemberDecorate
	{1,1}, // #76
	{1,1}, // VectorExtractDynamic
	{1,1}, // VectorInsertDynamic
	{1,1}, // VectorShuffle
	{1,1}, // CompositeConstruct
	{1,1}, // CompositeExtract
	{1,1}, // CompositeInsert
	{1,1}, // CopyObject
	{1,1}, // Transpose
	{1,1}, // #85
	{1,1}, // SampledImage
	{1,1}, // ImageSampleImplicitLod
	{1,1}, // ImageSampleExplicitLod
	{1,1}, // ImageSampleDrefImplicitLod
	{1,1}, // ImageSampleDrefExplicitLod
	{1,1}, // ImageSampleProjImplicitLod
	{1,1}, // ImageSampleProjExplicitLod
	{1,1}, // ImageSampleProjDrefImplicitLod
	{1,1}, // ImageSampleProjDrefExplicitLod
	{1,1}, // ImageFetch
	{1,1}, // ImageGather
	{1,1}, // ImageDrefGather
	{1,1}, // ImageRead
	{0,0}, // ImageWrite
	{1,1}, // Image
	{1,1}, // ImageQueryFormat
	{1,1}, // ImageQueryOrder
	{1,1}, // ImageQuerySizeLod
	{1,1}, // ImageQuerySize
	{1,1}, // ImageQueryLod
	{1,1}, // ImageQueryLevels
	{1,1}, // ImageQuerySamples
	{1,1}, // #108
	{1,1}, // ConvertFToU
	{1,1}, // ConvertFToS
	{1,1}, // ConvertSToF
	{1,1}, // ConvertUToF
	{1,1}, // UConvert
	{1,1}, // SConvert
	{1,1}, // FConvert
	{1,1}, // QuantizeToF16
	{1,1}, // ConvertPtrToU
	{1,1}, // SatConvertSToU
	{1,1}, // SatConvertUToS
	{1,1}, // ConvertUToPtr
	{1,1}, // PtrCastToGeneric
	{1,1}, // GenericCastToPtr
	{1,1}, // GenericCastToPtrExplicit
	{1,1}, // Bitcast
	{1,1}, // #125
	{1,1}, // SNegate
	{1,1}, // FNegate
	{1,1}, // IAdd
	{1,1}, // FAdd
	{1,1}, // ISub
	{1,1}, // FSub
	{1,1}, // IMul
	{1,1}, // FMul
	{1,1}, // UDiv
	{1,1}, // SDiv
	{1,1}, // FDiv
	{1,1}, // UMod
	{1,1}, // SRem
	{1,1}, // SMod
	{1,1}, // FRem
	{1,1}, // FMod
	{1,1}, // VectorTimesScalar
	{1,1}, // MatrixTimesScalar
	{1,1}, // VectorTimesMatrix
	{1,1}, // MatrixTimesVector
	{1,1}, // MatrixTimesMatrix
	{1,1}, // OuterProduct
	{1,1}, // Dot
	{1,1}, // IAddCarry
	{1,1}, // ISubBorrow
	{1,1}, // UMulExtended
	{1,1}, // SMulExtended
	{1,1}, // #153
	{1,1}, // Any
	{1,1}, // All
	{1,1}, // IsNan
	{1,1}, // IsInf
	{1,1}, // IsFinite
	{1,1}, // IsNormal
	{1,1}, // SignBitSet
	{1,1}, // LessOrGreater
	{1,1}, // Ordered
	{1,1}, // Unordered
	{1,1}, // LogicalEqual
	{1,1}, // LogicalNotEqual
	{1,1}, // LogicalOr
	{1,1}, // LogicalAnd
	{1,1}, // LogicalNot
	{1,1}, // Select
	{1,1}, // IEqual
	{1,1}, // INotEqual
	{1,1}, // UGreaterThan
	{1,1}, // SGreaterThan
	{1,1}, // UGreaterThanEqual
	{1,1}, // SGreaterThanEqual
	{1,1}, // ULessThan
	{1,1}, // SLessThan
	{1,1}, // ULessThanEqual
	{1,1}, // SLessThanEqual
	{1,1}, // FOrdEqual
	{1,1}, // FUnordEqual
	{1,1}, // FOrdNotEqual
	{1,1}, // FUnordNotEqual
	{1,1}, // FOrdLessThan
	{1,1}, // FUnordLessThan
	{1,1}, // FOrdGreaterThan
	{1,1}, // FUnordGreaterThan
	{1,1}, // FOrdLessThanEqual
	{1,1}, // FUnordLessThanEqual
	{1,1}, // FOrdGreaterThanEqual
	{1,1}, // FUnordGreaterThanEqual
	{1,1}, // #192
	{1,1}, // #193
	{1,1}, // ShiftRightLogical
	{1,1}, // ShiftRightArithmetic
	{1,1}, // ShiftLeftLogical
	{1,1}, // BitwiseOr
	{1,1}, // BitwiseXor
	{1,1}, // BitwiseAnd
	{1,1}, // Not
	{1,1}, // BitFieldInsert
	{1,1}, // BitFieldSExtract
	{1,1}, // BitFieldUExtract
	{1,1}, // BitReverse
	{1,1}, // BitCount
	{1,1}, // #206
	{1,1}, // DPdx
	{1,1}, // DPdy
	{1,1}, // Fwidth
	{1,1}, // DPdxFine
	{1,1}, // DPdyFine
	{1,1}, // FwidthFine
	{1,1}, // DPdxCoarse
	{1,1}, // DPdyCoarse
	{1,1}, // FwidthCoarse
	{1,1}, // #216
	{1,1}, // #217
	{0,0}, // EmitVertex
	{0,0}, // EndPrimitive
	{0,0}, // EmitStreamVertex
	{0,0}, // EndStreamPrimitive
	{1,1}, // #222
	{1,1}, // #223
	{0,0}, // ControlBarrier
	{0,0}, // MemoryBarrier
	{1,1}, // #226
	{1,1}, // AtomicLoad
	{0,0}, // AtomicStore
	{1,1}, // AtomicExchange
	{1,1}, // AtomicCompareExchange
	{1,1}, // AtomicCompareExchangeWeak
	{1,1}, // AtomicIIncrement
	{1,1}, // AtomicIDecrement
	{1,1}, // AtomicIAdd
	{1,1}, // AtomicISub
	{1,1}, // AtomicSMin
	{1,1}, // AtomicUMin
	{1,1}, // AtomicSMax
	{1,1}, // AtomicUMax
	{1,1}, // AtomicAnd
	{1,1}, // AtomicOr
	{1,1}, // AtomicXor
	{1,1}, // #243
	{1,1}, // #244
	{1,1}, // Phi
	{0,0}, // LoopMerge
	{0,0}, // SelectionMerge
	{1,0}, // Label
	{0,0}, // Branch
	{0,0}, // BranchConditional
	{0,0}, // Switch
	{0,0}, // Kill
	{0,0}, // Return
	{0,0}, // ReturnValue
	{0,0}, // Unreachable
	{0,0}, // LifetimeStart
	{0,0}, // LifetimeStop
	{1,1}, // #258
	{1,1}, // GroupAsyncCopy
	{0,0}, // GroupWaitEvents
	{1,1}, // GroupAll
	{1,1}, // GroupAny
	{1,1}, // GroupBroadcast
	{1,1}, // GroupIAdd
	{1,1}, // GroupFAdd
	{1,1}, // GroupFMin
	{1,1}, // GroupUMin
	{1,1}, // GroupSMin
	{1,1}, // GroupFMax
	{1,1}, // GroupUMax
	{1,1}, // GroupSMax
	{1,1}, // #272
	{1,1}, // #273
	{1,1}, // ReadPipe
	{1,1}, // WritePipe
	{1,1}, // ReservedReadPipe
	{1,1}, // ReservedWritePipe
	{1,1}, // ReserveReadPipePackets
	{1,1}, // ReserveWritePipePackets
	{0,0}, // CommitReadPipe
	{0,0}, // CommitWritePipe
	{1,1}, // IsValidReserveId
	{1,1}, // GetNumPipePackets
	{1,1}, // GetMaxPipePackets
	{1,1}, // GroupReserveReadPipePackets
	{1,1}, // GroupReserveWritePipePackets
	{0,0}, // GroupCommitReadPipe
	{0,0}, // GroupCommitWritePipe
	{1,1}, // #289
	{1,1}, // #290
	{1,1}, // EnqueueMarker
	{1,1}, // EnqueueKernel
	{1,1}, // GetKernelNDrangeSubGroupCount
	{1,1}, // GetKernelNDrangeMaxSubGroupSize
	{1,1}, // GetKernelWorkGroupSize
	{1,1}, // GetKernelPreferredWorkGroupSizeMultiple
	{0,0}, // RetainEvent
	{0,0}, // ReleaseEvent
	{1,1}, // CreateUserEvent
	{1,1}, // IsValidEvent
	{0,0}, // SetUserEventStatus
	{0,0}, // CaptureEventProfilingInfo
	{1,1}, // GetDefaultQueue
	{1,1}, // BuildNDRange
	{1,1}, // ImageSparseSampleImplicitLod
	{1,1}, // ImageSparseSampleExplicitLod
	{1,1}, // ImageSparseSampleDrefImplicitLod
	{1,1}, // ImageSparseSampleDrefExplicitLod
	{1,1}, // ImageSparseSampleProjImplicitLod
	{1,1}, // ImageSparseSampleProjExplicitLod
	{1,1}, // ImageSparseSampleProjDrefImplicitLod
	{1,1}, // ImageSparseSampleProjDrefExplicitLod
	{1,1}, // ImageSparseFetch
	{1,1}, // ImageSparseGather
	{1,1}, // ImageSparseDrefGather
	{1,1}, // ImageSparseTexelsResident
	{0,0}, // NoLine
	{1,1}, // AtomicFlagTestAndSet
	{0,0}, // AtomicFlagClear
	{1,1}, // ImageSparseRead
	{1,1}, // SizeOf
	{1,1}, // TypePipeStorage
	{1,1}, // ConstantPipeStorage
	{1,1}, // CreatePipeFromPipeStorage
	{1,1}, // GetKernelLocalSizeForSubgroupCount
	{1,1}, // GetKernelMaxNumSubgroups
	{1,1}, // TypeNamedBarrier
	{1,1}, // NamedBarrierInitialize
	{1,1}, // MemoryNamedBarrier
	{1,1}, // ModuleProcessed
};
static_assert(ARRAY_SIZE(kSpirvOpData) == kKnownOpsCount, "kSpirvOpData table mismatch with known SpvOps");


static bool smolv_OpHasResult(SpvOp op)
{
	if (op < 0 || op >= kKnownOpsCount)
		return false;
	return kSpirvOpData[op].hasResult;
}

static bool smolv_OpHasType(SpvOp op)
{
	if (op < 0 || op >= kKnownOpsCount)
		return false;
	return kSpirvOpData[op].hasType;
}


// --------------------------------------------------------------------------------------------


static bool smolv_CheckGenericHeader(const uint32_t* words, size_t wordCount, uint32_t expectedMagic)
{
	if (!words)
		return false;
	if (wordCount < 5)
		return false;
	
	uint32_t headerMagic = words[0];
	if (headerMagic != expectedMagic)
		return false;
	uint32_t headerVersion = words[1];
	if (headerVersion != 0x00010000 && headerVersion != 0x00010100)
		return false; // only support 1.0 and 1.1
	
	return true;
}

static const int kSpirVHeaderMagic = 0x07230203;
static const int kSmolHeaderMagic = 0x534D4F4C; // "SMOL"

static bool smolv_CheckSpirVHeader(const uint32_t* words, size_t wordCount)
{
	return smolv_CheckGenericHeader(words, wordCount, kSpirVHeaderMagic);
}
static bool smolv_CheckSmolHeader(const uint8_t* bytes, size_t byteCount)
{
	return smolv_CheckGenericHeader((const uint32_t*)bytes, byteCount/4, kSmolHeaderMagic);
}


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

static void smolv_Write4(smolv::ByteArray& arr, uint32_t v)
{
	arr.push_back(v & 0xFF);
	arr.push_back((v >> 8) & 0xFF);
	arr.push_back((v >> 16) & 0xFF);
	arr.push_back(v >> 24);
}
static void smolv_Write2(smolv::ByteArray& arr, uint16_t v)
{
	arr.push_back(v & 0xFF);
	arr.push_back((v >> 8) & 0xFF);
}

static bool smolv_Read4(const uint8_t*& data, const uint8_t* dataEnd, uint32_t& outv)
{
	if (data + 4 > dataEnd)
		return false;
	outv = (data[0]) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
	data += 4;
	return true;
}

static bool smolv_Read2(const uint8_t*& data, const uint8_t* dataEnd, uint16_t& outv)
{
	if (data + 2 > dataEnd)
		return false;
	outv = (data[0]) | (data[1] << 8);
	data += 2;
	return true;
}


// --------------------------------------------------------------------------------------------

// Variable-length integer encoding for unsigned integers. In each byte:
// - highest bit set if more bytes follow, cleared if this is last byte.
// - other 7 bits are the actual value payload.
// Takes 1-5 bytes to encode an integer (values between 0 and 127 take one byte, etc.).

static void smolv_WriteVarint(smolv::ByteArray& arr, uint32_t v)
{
	while (v > 127)
	{
		arr.push_back((v & 127) | 128);
		v >>= 7;
	}
	arr.push_back(v & 127);
}

static uint32_t smolv_ReadVarint(const uint8_t*& data, const uint8_t* dataEnd)
{
	uint32_t v = 0;
	uint32_t shift = 0;
	while (data < dataEnd)
	{
		uint8_t b = *data;
		v |= (b & 127) << shift;
		shift += 7;
		data++;
		if (!(b & 128))
			break;
	}
	return v;
}


// SPIR-V has about 330 ops defined, and majority of lengths are under 16 words. If our op falls
// into this case, then write it out in two bytes as: 1LLL_LLOO_OOOO_OOOO (5 bits length,
// 10 bits op, highest bit set).

static void smolv_WriteOpLength(smolv::ByteArray& arr, uint32_t len, SpvOp op)
{
	if (len < 32 && op < 1024)
	{
		uint16_t v = 0x8000 | (len << 10) | op;
		smolv_Write2(arr, v);
	}
	else
	{
		uint32_t v = (len << 16) | op;
		//@TODO: would not decode properly if op is > 0x7FFF.
		smolv_Write4(arr, v);
	}
}

static bool smolv_ReadOpLength(const uint8_t*& data, const uint8_t* dataEnd, uint32_t& outLen, SpvOp& outOp)
{
	uint16_t v1, v2;
	if (!smolv_Read2(data, dataEnd, v1))
		return false;
	if (v1 & 0x8000)
	{
		outLen = (v1 >> 10) & 0x1F;
		outOp = (SpvOp)(v1 & 0x3FF);
		return true;
	}
	if (!smolv_Read2(data, dataEnd, v2))
		return false;
	outLen = v2;
	outOp = (SpvOp)v1;
	return true;
}


#define _SMOLV_READ_OP() \
	uint32_t instrLen = words[0] >> 16; \
	if (instrLen < 1) return false; /* malformed instruction, lenght needs to be at least 1 */ \
	if (words + instrLen > wordsEnd) return false; /* malformed instruction, goes past end of data */ \
	SpvOp op = (SpvOp)(words[0] & 0xFFFF)


bool smolv::Encode(const void* spirvData, size_t spirvSize, ByteArray& outSmolv)
{
	const size_t wordCount = spirvSize / 4;
	const uint32_t* words = (const uint32_t*)spirvData;
	const uint32_t* wordsEnd = words + wordCount;
	if (!smolv_CheckSpirVHeader(words, wordCount))
		return false;

	//@TODO: reserve
	// header
	smolv_Write4(outSmolv, kSmolHeaderMagic);
	smolv_Write4(outSmolv, words[1]); // version
	smolv_Write4(outSmolv, words[2]); // generator
	smolv_Write4(outSmolv, words[3]); // bound
	smolv_Write4(outSmolv, words[4]); // schema

	words += 5;
	while (words < wordsEnd)
	{
		_SMOLV_READ_OP();

		smolv_WriteOpLength(outSmolv, instrLen, op);
		for (int i = 1; i < instrLen; ++i)
			smolv_Write4(outSmolv, words[i]);
		
		words += instrLen;
	}
	
	return true;
}


bool smolv::Decode(const void* smolvData, size_t smolvSize, ByteArray& outSpirv)
{
	const uint8_t* bytes = (const uint8_t*)smolvData;
	const uint8_t* bytesEnd = bytes + smolvSize;
	if (!smolv_CheckSmolHeader(bytes, smolvSize))
		return false;

	uint32_t val;

	// header
	smolv_Write4(outSpirv, kSpirVHeaderMagic); bytes += 4;
	smolv_Read4(bytes, bytesEnd, val); smolv_Write4(outSpirv, val); // version
	smolv_Read4(bytes, bytesEnd, val); smolv_Write4(outSpirv, val); // generator
	smolv_Read4(bytes, bytesEnd, val); smolv_Write4(outSpirv, val); // bound
	smolv_Read4(bytes, bytesEnd, val); smolv_Write4(outSpirv, val); // schema

	while (bytes < bytesEnd)
	{
		uint32_t instrLen;
		SpvOp op;
		if (!smolv_ReadOpLength(bytes, bytesEnd, instrLen, op))
			return false;

		val = (instrLen << 16) | op; smolv_Write4(outSpirv, val);
		for (int i = 1; i < instrLen; ++i)
		{
			if (!smolv_Read4(bytes, bytesEnd, val))
				return false;
			smolv_Write4(outSpirv, val);
		}
	}
	
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
		_SMOLV_READ_OP();

		if (op < kKnownOpsCount)
		{
			printf("%04i %-20s %2i  ", (int)offset, smolv_GetOpName(op), instrLen);
			stats->opCounts[op]++;
			stats->opSizes[op] += instrLen;
		}
		else
		{
			printf("%04i Op#%i %2i  ", (int)offset, op, instrLen);
		}

		switch(op)
		{
			case SpvOpDecorate:
				if (instrLen < 3)
					return false;
				printf("id %3i dec %3i ", words[1], words[2]);
				for (int i = 3; i < instrLen; ++i)
					printf("%i ", words[i]);
				break;
			case SpvOpLoad:
				if (instrLen < 4)
					return false;
				printf("t %3i res %3i ptr %3i ", words[1], words[2], words[3]);
				for (int i = 4; i < instrLen; ++i)
					printf("%i ", words[i]);
				break;
			case SpvOpAccessChain:
				if (instrLen < 4)
					return false;
				printf("t %3i res %3i bas %3i ", words[1], words[2], words[3]);
				for (int i = 4; i < instrLen; ++i)
					printf("%i ", words[i]);
				break;
			case SpvOpVectorShuffle:
				if (instrLen < 5)
					return false;
				printf("t %3i res %3i v1 %3i v2 %3i ", words[1], words[2], words[3], words[4]);
				for (int i = 5; i < instrLen; ++i)
					printf("%i ", words[i]);
				break;
			default:
				break;
		}
		printf("\n");
		
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
		printf("%-15s: %5.1fKB (%5.1f%%)\n", kv.first.c_str(), kv.second/1024.0f, (float)kv.second/(stats->totalSize*4.0f)*100.0f);
	}
}

