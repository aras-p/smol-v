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
	uint8_t hasResult;	// does it have result ID?
	uint8_t hasType;	// does it have type ID?
	uint8_t deltaFromResult; // how many words after (optional) type+result to write out as deltas from result?
	uint8_t varrest;	// should the rest of words be written in varint encoding?
};
static const OpData kSpirvOpData[] =
{
	{0, 0, 0, 0}, // Nop
	{1, 1, 0, 0}, // Undef
	{0, 0, 0, 0}, // SourceContinued
	{0, 0, 0, 0}, // Source
	{0, 0, 0, 0}, // SourceExtension
	{0, 0, 0, 0}, // Name
	{0, 0, 0, 0}, // MemberName
	{0, 0, 0, 0}, // String
	{0, 0, 0, 0}, // Line
	{1, 1, 0, 0}, // #9
	{0, 0, 0, 0}, // Extension
	{1, 0, 0, 0}, // ExtInstImport
	{1, 1, 0, 0}, // ExtInst
	{1, 1, 0, 0}, // #13
	{0, 0, 0, 0}, // MemoryModel
	{0, 0, 0, 0}, // EntryPoint
	{0, 0, 0, 0}, // ExecutionMode
	{0, 0, 0, 0}, // Capability
	{1, 1, 0, 0}, // #18
	{1, 0, 0, 0}, // TypeVoid
	{1, 0, 0, 0}, // TypeBool
	{1, 0, 0, 0}, // TypeInt
	{1, 0, 0, 0}, // TypeFloat
	{1, 0, 0, 0}, // TypeVector
	{1, 0, 0, 0}, // TypeMatrix
	{1, 0, 0, 0}, // TypeImage
	{1, 0, 0, 0}, // TypeSampler
	{1, 0, 0, 0}, // TypeSampledImage
	{1, 0, 0, 0}, // TypeArray
	{1, 0, 0, 0}, // TypeRuntimeArray
	{1, 0, 0, 0}, // TypeStruct
	{1, 0, 0, 0}, // TypeOpaque
	{1, 0, 0, 0}, // TypePointer
	{1, 0, 0, 0}, // TypeFunction
	{1, 0, 0, 0}, // TypeEvent
	{1, 0, 0, 0}, // TypeDeviceEvent
	{1, 0, 0, 0}, // TypeReserveId
	{1, 0, 0, 0}, // TypeQueue
	{1, 0, 0, 0}, // TypePipe
	{0, 0, 0, 0}, // TypeForwardPointer
	{1, 1, 0, 0}, // #40
	{1, 1, 0, 0}, // ConstantTrue
	{1, 1, 0, 0}, // ConstantFalse
	{1, 1, 0, 0}, // Constant
	{1, 1, 0, 0}, // ConstantComposite
	{1, 1, 0, 0}, // ConstantSampler
	{1, 1, 0, 0}, // ConstantNull
	{1, 1, 0, 0}, // #47
	{1, 1, 0, 0}, // SpecConstantTrue
	{1, 1, 0, 0}, // SpecConstantFalse
	{1, 1, 0, 0}, // SpecConstant
	{1, 1, 0, 0}, // SpecConstantComposite
	{1, 1, 0, 0}, // SpecConstantOp
	{1, 1, 0, 0}, // #53
	{1, 1, 0, 0}, // Function
	{1, 1, 0, 0}, // FunctionParameter
	{0, 0, 0, 0}, // FunctionEnd
	{1, 1, 0, 0}, // FunctionCall
	{1, 1, 0, 0}, // #58
	{1, 1, 0, 0}, // Variable
	{1, 1, 0, 0}, // ImageTexelPointer
	{1, 1, 1, 1}, // Load
	{0, 0, 2, 1}, // Store
	{0, 0, 0, 0}, // CopyMemory
	{0, 0, 0, 0}, // CopyMemorySized
	{1, 1, 0, 1}, // AccessChain
	{1, 1, 0, 0}, // InBoundsAccessChain
	{1, 1, 0, 0}, // PtrAccessChain
	{1, 1, 0, 0}, // ArrayLength
	{1, 1, 0, 0}, // GenericPtrMemSemantics
	{1, 1, 0, 0}, // InBoundsPtrAccessChain
	{0, 0, 0, 1}, // Decorate
	{0, 0, 0, 1}, // MemberDecorate
	{1, 0, 0, 0}, // DecorationGroup
	{0, 0, 0, 0}, // GroupDecorate
	{0, 0, 0, 0}, // GroupMemberDecorate
	{1, 1, 0, 0}, // #76
	{1, 1, 0, 0}, // VectorExtractDynamic
	{1, 1, 0, 0}, // VectorInsertDynamic
	{1, 1, 2, 1}, // VectorShuffle
	{1, 1, 0, 0}, // CompositeConstruct
	{1, 1, 0, 0}, // CompositeExtract
	{1, 1, 0, 0}, // CompositeInsert
	{1, 1, 0, 0}, // CopyObject
	{1, 1, 0, 0}, // Transpose
	{1, 1, 0, 0}, // #85
	{1, 1, 0, 0}, // SampledImage
	{1, 1, 0, 0}, // ImageSampleImplicitLod
	{1, 1, 0, 0}, // ImageSampleExplicitLod
	{1, 1, 0, 0}, // ImageSampleDrefImplicitLod
	{1, 1, 0, 0}, // ImageSampleDrefExplicitLod
	{1, 1, 0, 0}, // ImageSampleProjImplicitLod
	{1, 1, 0, 0}, // ImageSampleProjExplicitLod
	{1, 1, 0, 0}, // ImageSampleProjDrefImplicitLod
	{1, 1, 0, 0}, // ImageSampleProjDrefExplicitLod
	{1, 1, 0, 0}, // ImageFetch
	{1, 1, 0, 0}, // ImageGather
	{1, 1, 0, 0}, // ImageDrefGather
	{1, 1, 0, 0}, // ImageRead
	{0, 0, 0, 0}, // ImageWrite
	{1, 1, 0, 0}, // Image
	{1, 1, 0, 0}, // ImageQueryFormat
	{1, 1, 0, 0}, // ImageQueryOrder
	{1, 1, 0, 0}, // ImageQuerySizeLod
	{1, 1, 0, 0}, // ImageQuerySize
	{1, 1, 0, 0}, // ImageQueryLod
	{1, 1, 0, 0}, // ImageQueryLevels
	{1, 1, 0, 0}, // ImageQuerySamples
	{1, 1, 0, 0}, // #108
	{1, 1, 0, 0}, // ConvertFToU
	{1, 1, 0, 0}, // ConvertFToS
	{1, 1, 0, 0}, // ConvertSToF
	{1, 1, 0, 0}, // ConvertUToF
	{1, 1, 0, 0}, // UConvert
	{1, 1, 0, 0}, // SConvert
	{1, 1, 0, 0}, // FConvert
	{1, 1, 0, 0}, // QuantizeToF16
	{1, 1, 0, 0}, // ConvertPtrToU
	{1, 1, 0, 0}, // SatConvertSToU
	{1, 1, 0, 0}, // SatConvertUToS
	{1, 1, 0, 0}, // ConvertUToPtr
	{1, 1, 0, 0}, // PtrCastToGeneric
	{1, 1, 0, 0}, // GenericCastToPtr
	{1, 1, 0, 0}, // GenericCastToPtrExplicit
	{1, 1, 0, 0}, // Bitcast
	{1, 1, 0, 0}, // #125
	{1, 1, 1, 0}, // SNegate
	{1, 1, 1, 0}, // FNegate
	{1, 1, 2, 0}, // IAdd
	{1, 1, 2, 0}, // FAdd
	{1, 1, 2, 0}, // ISub
	{1, 1, 2, 0}, // FSub
	{1, 1, 2, 0}, // IMul
	{1, 1, 2, 0}, // FMul
	{1, 1, 2, 0}, // UDiv
	{1, 1, 2, 0}, // SDiv
	{1, 1, 2, 0}, // FDiv
	{1, 1, 2, 0}, // UMod
	{1, 1, 2, 0}, // SRem
	{1, 1, 2, 0}, // SMod
	{1, 1, 2, 0}, // FRem
	{1, 1, 2, 0}, // FMod
	{1, 1, 2, 0}, // VectorTimesScalar
	{1, 1, 2, 0}, // MatrixTimesScalar
	{1, 1, 2, 0}, // VectorTimesMatrix
	{1, 1, 2, 0}, // MatrixTimesVector
	{1, 1, 2, 0}, // MatrixTimesMatrix
	{1, 1, 2, 0}, // OuterProduct
	{1, 1, 2, 0}, // Dot
	{1, 1, 2, 0}, // IAddCarry
	{1, 1, 2, 0}, // ISubBorrow
	{1, 1, 2, 0}, // UMulExtended
	{1, 1, 2, 0}, // SMulExtended
	{1, 1, 0, 0}, // #153
	{1, 1, 0, 0}, // Any
	{1, 1, 0, 0}, // All
	{1, 1, 0, 0}, // IsNan
	{1, 1, 0, 0}, // IsInf
	{1, 1, 0, 0}, // IsFinite
	{1, 1, 0, 0}, // IsNormal
	{1, 1, 0, 0}, // SignBitSet
	{1, 1, 0, 0}, // LessOrGreater
	{1, 1, 0, 0}, // Ordered
	{1, 1, 0, 0}, // Unordered
	{1, 1, 0, 0}, // LogicalEqual
	{1, 1, 0, 0}, // LogicalNotEqual
	{1, 1, 0, 0}, // LogicalOr
	{1, 1, 0, 0}, // LogicalAnd
	{1, 1, 0, 0}, // LogicalNot
	{1, 1, 0, 0}, // Select
	{1, 1, 0, 0}, // IEqual
	{1, 1, 0, 0}, // INotEqual
	{1, 1, 0, 0}, // UGreaterThan
	{1, 1, 0, 0}, // SGreaterThan
	{1, 1, 0, 0}, // UGreaterThanEqual
	{1, 1, 0, 0}, // SGreaterThanEqual
	{1, 1, 0, 0}, // ULessThan
	{1, 1, 0, 0}, // SLessThan
	{1, 1, 0, 0}, // ULessThanEqual
	{1, 1, 0, 0}, // SLessThanEqual
	{1, 1, 0, 0}, // FOrdEqual
	{1, 1, 0, 0}, // FUnordEqual
	{1, 1, 0, 0}, // FOrdNotEqual
	{1, 1, 0, 0}, // FUnordNotEqual
	{1, 1, 0, 0}, // FOrdLessThan
	{1, 1, 0, 0}, // FUnordLessThan
	{1, 1, 0, 0}, // FOrdGreaterThan
	{1, 1, 0, 0}, // FUnordGreaterThan
	{1, 1, 0, 0}, // FOrdLessThanEqual
	{1, 1, 0, 0}, // FUnordLessThanEqual
	{1, 1, 0, 0}, // FOrdGreaterThanEqual
	{1, 1, 0, 0}, // FUnordGreaterThanEqual
	{1, 1, 0, 0}, // #192
	{1, 1, 0, 0}, // #193
	{1, 1, 0, 0}, // ShiftRightLogical
	{1, 1, 0, 0}, // ShiftRightArithmetic
	{1, 1, 0, 0}, // ShiftLeftLogical
	{1, 1, 0, 0}, // BitwiseOr
	{1, 1, 0, 0}, // BitwiseXor
	{1, 1, 0, 0}, // BitwiseAnd
	{1, 1, 0, 0}, // Not
	{1, 1, 0, 0}, // BitFieldInsert
	{1, 1, 0, 0}, // BitFieldSExtract
	{1, 1, 0, 0}, // BitFieldUExtract
	{1, 1, 0, 0}, // BitReverse
	{1, 1, 0, 0}, // BitCount
	{1, 1, 0, 0}, // #206
	{1, 1, 0, 0}, // DPdx
	{1, 1, 0, 0}, // DPdy
	{1, 1, 0, 0}, // Fwidth
	{1, 1, 0, 0}, // DPdxFine
	{1, 1, 0, 0}, // DPdyFine
	{1, 1, 0, 0}, // FwidthFine
	{1, 1, 0, 0}, // DPdxCoarse
	{1, 1, 0, 0}, // DPdyCoarse
	{1, 1, 0, 0}, // FwidthCoarse
	{1, 1, 0, 0}, // #216
	{1, 1, 0, 0}, // #217
	{0, 0, 0, 0}, // EmitVertex
	{0, 0, 0, 0}, // EndPrimitive
	{0, 0, 0, 0}, // EmitStreamVertex
	{0, 0, 0, 0}, // EndStreamPrimitive
	{1, 1, 0, 0}, // #222
	{1, 1, 0, 0}, // #223
	{0, 0, 0, 0}, // ControlBarrier
	{0, 0, 0, 0}, // MemoryBarrier
	{1, 1, 0, 0}, // #226
	{1, 1, 0, 0}, // AtomicLoad
	{0, 0, 0, 0}, // AtomicStore
	{1, 1, 0, 0}, // AtomicExchange
	{1, 1, 0, 0}, // AtomicCompareExchange
	{1, 1, 0, 0}, // AtomicCompareExchangeWeak
	{1, 1, 0, 0}, // AtomicIIncrement
	{1, 1, 0, 0}, // AtomicIDecrement
	{1, 1, 0, 0}, // AtomicIAdd
	{1, 1, 0, 0}, // AtomicISub
	{1, 1, 0, 0}, // AtomicSMin
	{1, 1, 0, 0}, // AtomicUMin
	{1, 1, 0, 0}, // AtomicSMax
	{1, 1, 0, 0}, // AtomicUMax
	{1, 1, 0, 0}, // AtomicAnd
	{1, 1, 0, 0}, // AtomicOr
	{1, 1, 0, 0}, // AtomicXor
	{1, 1, 0, 0}, // #243
	{1, 1, 0, 0}, // #244
	{1, 1, 0, 0}, // Phi
	{0, 0, 0, 0}, // LoopMerge
	{0, 0, 0, 0}, // SelectionMerge
	{1, 0, 0, 0}, // Label
	{0, 0, 0, 0}, // Branch
	{0, 0, 0, 0}, // BranchConditional
	{0, 0, 0, 0}, // Switch
	{0, 0, 0, 0}, // Kill
	{0, 0, 0, 0}, // Return
	{0, 0, 0, 0}, // ReturnValue
	{0, 0, 0, 0}, // Unreachable
	{0, 0, 0, 0}, // LifetimeStart
	{0, 0, 0, 0}, // LifetimeStop
	{1, 1, 0, 0}, // #258
	{1, 1, 0, 0}, // GroupAsyncCopy
	{0, 0, 0, 0}, // GroupWaitEvents
	{1, 1, 0, 0}, // GroupAll
	{1, 1, 0, 0}, // GroupAny
	{1, 1, 0, 0}, // GroupBroadcast
	{1, 1, 0, 0}, // GroupIAdd
	{1, 1, 0, 0}, // GroupFAdd
	{1, 1, 0, 0}, // GroupFMin
	{1, 1, 0, 0}, // GroupUMin
	{1, 1, 0, 0}, // GroupSMin
	{1, 1, 0, 0}, // GroupFMax
	{1, 1, 0, 0}, // GroupUMax
	{1, 1, 0, 0}, // GroupSMax
	{1, 1, 0, 0}, // #272
	{1, 1, 0, 0}, // #273
	{1, 1, 0, 0}, // ReadPipe
	{1, 1, 0, 0}, // WritePipe
	{1, 1, 0, 0}, // ReservedReadPipe
	{1, 1, 0, 0}, // ReservedWritePipe
	{1, 1, 0, 0}, // ReserveReadPipePackets
	{1, 1, 0, 0}, // ReserveWritePipePackets
	{0, 0, 0, 0}, // CommitReadPipe
	{0, 0, 0, 0}, // CommitWritePipe
	{1, 1, 0, 0}, // IsValidReserveId
	{1, 1, 0, 0}, // GetNumPipePackets
	{1, 1, 0, 0}, // GetMaxPipePackets
	{1, 1, 0, 0}, // GroupReserveReadPipePackets
	{1, 1, 0, 0}, // GroupReserveWritePipePackets
	{0, 0, 0, 0}, // GroupCommitReadPipe
	{0, 0, 0, 0}, // GroupCommitWritePipe
	{1, 1, 0, 0}, // #289
	{1, 1, 0, 0}, // #290
	{1, 1, 0, 0}, // EnqueueMarker
	{1, 1, 0, 0}, // EnqueueKernel
	{1, 1, 0, 0}, // GetKernelNDrangeSubGroupCount
	{1, 1, 0, 0}, // GetKernelNDrangeMaxSubGroupSize
	{1, 1, 0, 0}, // GetKernelWorkGroupSize
	{1, 1, 0, 0}, // GetKernelPreferredWorkGroupSizeMultiple
	{0, 0, 0, 0}, // RetainEvent
	{0, 0, 0, 0}, // ReleaseEvent
	{1, 1, 0, 0}, // CreateUserEvent
	{1, 1, 0, 0}, // IsValidEvent
	{0, 0, 0, 0}, // SetUserEventStatus
	{0, 0, 0, 0}, // CaptureEventProfilingInfo
	{1, 1, 0, 0}, // GetDefaultQueue
	{1, 1, 0, 0}, // BuildNDRange
	{1, 1, 0, 0}, // ImageSparseSampleImplicitLod
	{1, 1, 0, 0}, // ImageSparseSampleExplicitLod
	{1, 1, 0, 0}, // ImageSparseSampleDrefImplicitLod
	{1, 1, 0, 0}, // ImageSparseSampleDrefExplicitLod
	{1, 1, 0, 0}, // ImageSparseSampleProjImplicitLod
	{1, 1, 0, 0}, // ImageSparseSampleProjExplicitLod
	{1, 1, 0, 0}, // ImageSparseSampleProjDrefImplicitLod
	{1, 1, 0, 0}, // ImageSparseSampleProjDrefExplicitLod
	{1, 1, 0, 0}, // ImageSparseFetch
	{1, 1, 0, 0}, // ImageSparseGather
	{1, 1, 0, 0}, // ImageSparseDrefGather
	{1, 1, 0, 0}, // ImageSparseTexelsResident
	{0, 0, 0, 0}, // NoLine
	{1, 1, 0, 0}, // AtomicFlagTestAndSet
	{0, 0, 0, 0}, // AtomicFlagClear
	{1, 1, 0, 0}, // ImageSparseRead
	{1, 1, 0, 0}, // SizeOf
	{1, 1, 0, 0}, // TypePipeStorage
	{1, 1, 0, 0}, // ConstantPipeStorage
	{1, 1, 0, 0}, // CreatePipeFromPipeStorage
	{1, 1, 0, 0}, // GetKernelLocalSizeForSubgroupCount
	{1, 1, 0, 0}, // GetKernelMaxNumSubgroups
	{1, 1, 0, 0}, // TypeNamedBarrier
	{1, 1, 0, 0}, // NamedBarrierInitialize
	{1, 1, 0, 0}, // MemoryNamedBarrier
	{1, 1, 0, 0}, // ModuleProcessed
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

static int smolv_OpDeltaFromResult(SpvOp op)
{
	if (op < 0 || op >= kKnownOpsCount)
		return 0;
	return kSpirvOpData[op].deltaFromResult;
}

static bool smolv_OpVarRest(SpvOp op)
{
	if (op < 0 || op >= kKnownOpsCount)
		return false;
	return kSpirvOpData[op].varrest;
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
	size_t opSizesSmol[kKnownOpsCount] = {};
	size_t totalOps = 0;
	size_t totalSize = 0;
	size_t totalSizeSmol = 0;
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

static bool smolv_ReadVarint(const uint8_t*& data, const uint8_t* dataEnd, uint32_t& outVal)
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
	outVal = v;
	return true; //@TODO: report failures
}

static uint32_t smolv_ZigEncode(int32_t i)
{
	return (i << 1) ^ (i >> 31);
}

static int32_t smolv_ZigDecode(uint32_t u)
{
	 return (u & 1) ? ((u >> 1) ^ ~0) : (u >> 1);
}


// Shuffling bits of length + opcode to be more compact in varint encoding in typical cases:
// 0b LLLL LLLL LLLL LLLL OOOO OOOO OOOO OOOO is how SPIR-V encodes it (L=length, O=op), we shuffle into:
// 0b LLLL LLLL LLLO OOOO OOOL LLLL OOOO OOOO, so that typical lengths (<32) and ops (<256) fit under
// 14 bits and encoded into 2 bytes with varint.

static void smolv_WriteLengthOp(smolv::ByteArray& arr, uint32_t len, SpvOp op)
{
	uint32_t oplen = ((len >> 5) << 21) | ((op >> 8) << 13) | ((len & 0x1F) << 8) | (op & 0xFF);
	smolv_WriteVarint(arr, oplen);
}

static bool smolv_ReadLengthOp(const uint8_t*& data, const uint8_t* dataEnd, uint32_t& outLen, SpvOp& outOp)
{
	uint32_t val;
	if (!smolv_ReadVarint(data, dataEnd, val))
		return false;
	outLen = ((val >> 21) << 5) | ((val >> 8) & 0x1F);
	outOp = (SpvOp)(((val >> 13) << 8) | (val & 0xFF));
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

	uint32_t prevResult = 0;
	uint32_t prevDecorate = 0;

	words += 5;
	while (words < wordsEnd)
	{
		_SMOLV_READ_OP();

		// length + opcode
		smolv_WriteLengthOp(outSmolv, instrLen, op);

		size_t ioffs = 1;
		// write type as varint, if we have it
		if (smolv_OpHasType(op))
		{
			smolv_WriteVarint(outSmolv, words[ioffs]);
			ioffs++;
		}
		// write result as delta+varint, if we have it
		if (smolv_OpHasResult(op))
		{
			uint32_t v = words[ioffs];
			smolv_WriteVarint(outSmolv, smolv_ZigEncode(v - prevResult));
			prevResult = v;
			ioffs++;
		}

		// Decorate & MemberDecorate: IDs relative to previous decorate
		if (op == SpvOpDecorate || op == SpvOpMemberDecorate)
		{
			uint32_t v = words[ioffs];
			smolv_WriteVarint(outSmolv, v - prevDecorate);
			prevDecorate = v;
			ioffs++;
		}

		// Write out this many IDs, encoding them relative to result ID
		int relativeCount = smolv_OpDeltaFromResult(op);
		for (int i = 0; i < relativeCount; ++i)
		{
			smolv_WriteVarint(outSmolv, prevResult - words[ioffs]); ioffs++;
		}
		if (smolv_OpVarRest(op))
		{
			// write out rest of words with variable encoding (expected to be small integers)
			for (; ioffs < instrLen; ++ioffs)
				smolv_WriteVarint(outSmolv, words[ioffs]);
		}
		else
		{
			// write out rest of words without any encoding
			for (; ioffs < instrLen; ++ioffs)
				smolv_Write4(outSmolv, words[ioffs]);
		}
		
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

	uint32_t prevResult = 0;
	uint32_t prevDecorate = 0;

	while (bytes < bytesEnd)
	{
		// read length + opcode
		uint32_t instrLen;
		SpvOp op;
		if (!smolv_ReadLengthOp(bytes, bytesEnd, instrLen, op))
			return false;
		smolv_Write4(outSpirv, (instrLen << 16) | op);

		size_t ioffs = 1;

		// read type as varint, if we have it
		if (smolv_OpHasType(op))
		{
			if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			smolv_Write4(outSpirv, val);
			ioffs++;
		}
		// read result as delta+varint, if we have it
		if (smolv_OpHasResult(op))
		{
			if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			val = prevResult + smolv_ZigDecode(val);
			smolv_Write4(outSpirv, val);
			prevResult = val;
			ioffs++;
		}
		
		// Decorate: IDs relative to previous decorate
		if (op == SpvOpDecorate || op == SpvOpMemberDecorate)
		{
			if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			val = prevDecorate + val;
			smolv_Write4(outSpirv, val);
			prevDecorate = val;
			ioffs++;
		}

		// Read this many IDs, that are relative to result ID
		int relativeCount = smolv_OpDeltaFromResult(op);
		for (int i = 0; i < relativeCount; ++i)
		{
			if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			smolv_Write4(outSpirv, prevResult - val);
			ioffs++;
		}
		if (smolv_OpVarRest(op))
		{
			// read rest of words with variable encoding
			for (; ioffs < instrLen; ++ioffs)
			{
				if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
				smolv_Write4(outSpirv, val);
			}
		}
		else
		{
			// read rest of words without any encoding
			for (; ioffs < instrLen; ++ioffs)
			{
				if (!smolv_Read4(bytes, bytesEnd, val)) return false;
				smolv_Write4(outSpirv, val);
			}
		}
	}
	
	return true;
}



// --------------------------------------------------------------------------------------------
// Calculating instruction count / space stats on SPIR-V and SMOL-V


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

	//printf("%4s %-20s Ln\n", "Offs", "Op");
	size_t offset = 5;
	words += 5;
	while (words < wordsEnd)
	{
		_SMOLV_READ_OP();

		if (op < kKnownOpsCount)
		{
			//printf("%04i %-20s %2i  ", (int)offset, smolv_GetOpName(op), instrLen);
			stats->opCounts[op]++;
			stats->opSizes[op] += instrLen;
		}
		else
		{
			//printf("%04i Op#%i %2i  ", (int)offset, op, instrLen);
		}

		switch(op)
		{
			case SpvOpDecorate:
				/*
				if (instrLen < 3)
					return false;
				printf("OpDecor id %3i dec %3i ", words[1], words[2]);
				for (int i = 3; i < instrLen; ++i)
					printf("%i ", words[i]);
				printf("\n");
				 */
				break;
			case SpvOpLoad:
				/*
				if (instrLen < 4)
					return false;
				printf("t %3i res %3i ptr %3i ", words[1], words[2], words[3]);
				for (int i = 4; i < instrLen; ++i)
					printf("%i ", words[i]);
				 printf("\n");
				 */
				break;
			case SpvOpAccessChain:
				/*
				if (instrLen < 4)
					return false;
				printf("t %3i res %3i bas %3i ", words[1], words[2], words[3]);
				for (int i = 4; i < instrLen; ++i)
					printf("%i ", words[i]);
				 printf("\n");
				 */
				break;
			case SpvOpVectorShuffle:
				/*
				if (instrLen < 5)
					return false;
				printf("t %3i res %3i v1 %3i v2 %3i ", words[1], words[2], words[3], words[4]);
				for (int i = 5; i < instrLen; ++i)
					printf("%i ", words[i]);
				 printf("\n");
				*/
				break;
			default:
				break;
		}
		
		words += instrLen;
		offset += instrLen;
		stats->totalOps++;
	}
	
	return true;
}


bool smolv::InputStatsCalculateSmol(smolv::InputStats* stats, const void* smolvData, size_t smolvSize)
{
	if (!stats)
		return false;
	
	const uint8_t* bytes = (const uint8_t*)smolvData;
	const uint8_t* bytesEnd = bytes + smolvSize;
	if (!smolv_CheckSmolHeader(bytes, smolvSize))
		return false;
	
	// go over instructions
	stats->totalSizeSmol += smolvSize;
	
	uint32_t val;
	
	while (bytes < bytesEnd)
	{
		// read length + opcode
		const uint8_t* instrBegin = bytes;
		uint32_t instrLen;
		SpvOp op;
		if (!smolv_ReadLengthOp(bytes, bytesEnd, instrLen, op))
			return false;
		
		size_t ioffs = 1;
		if (smolv_OpHasType(op))
		{
			if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			ioffs++;
		}
		if (smolv_OpHasResult(op))
		{
			if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			ioffs++;
		}
		
		if (op == SpvOpDecorate || op == SpvOpMemberDecorate)
		{
			if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			ioffs++;
		}
		
		int relativeCount = smolv_OpDeltaFromResult(op);
		for (int i = 0; i < relativeCount; ++i)
		{
			if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			ioffs++;
		}
		if (smolv_OpVarRest(op))
		{
			for (; ioffs < instrLen; ++ioffs)
			{
				if (!smolv_ReadVarint(bytes, bytesEnd, val)) return false;
			}
		}
		else
		{
			for (; ioffs < instrLen; ++ioffs)
			{
				if (!smolv_Read4(bytes, bytesEnd, val)) return false;
			}
		}
		
		if (op < kKnownOpsCount)
		{
			stats->opSizesSmol[op] += bytes - instrBegin;
		}
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
	OpCounter sizesSmol[kKnownOpsCount];
	for (int i = 0; i < kKnownOpsCount; ++i)
	{
		counts[i].first = (SpvOp)i;
		counts[i].second = stats->opCounts[i];
		sizes[i].first = (SpvOp)i;
		sizes[i].second = stats->opSizes[i];
		sizesSmol[i].first = (SpvOp)i;
		sizesSmol[i].second = stats->opSizesSmol[i];
	}
	std::sort(counts, counts + kKnownOpsCount, [](OpCounter a, OpCounter b) { return a.second > b.second; });
	std::sort(sizes, sizes + kKnownOpsCount, [](OpCounter a, OpCounter b) { return a.second > b.second; });
	std::sort(sizesSmol, sizesSmol + kKnownOpsCount, [](OpCounter a, OpCounter b) { return a.second > b.second; });
	
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
			   (int)sizes[i].second*4,
			   (float)sizes[i].second / (float)stats->totalSize * 100.0f,
			   (float)sizes[i].second*4 / (float)stats->opCounts[op]
		);
	}
	printf("Largest total size of ops in SMOL:\n");
	for (int i = 0; i < 15; ++i)
	{
		SpvOp op = sizesSmol[i].first;
		printf(" #%2i: %-20s %4i (%4.1f%%) avg len %.1f\n",
			   i,
			   kSpirvOpNames[op],
			   (int)sizesSmol[i].second,
			   (float)sizesSmol[i].second / (float)stats->totalSizeSmol * 100.0f,
			   (float)sizesSmol[i].second / (float)stats->opCounts[op]
			   );
	}
	
	printf("Compression: original size %.1fKB\n", stats->totalSize*4.0f/1024.0f);
	for (auto&& kv : stats->compressedSizes)
	{
		printf("%-15s: %5.1fKB (%5.1f%%)\n", kv.first.c_str(), kv.second/1024.0f, (float)kv.second/(stats->totalSize*4.0f)*100.0f);
	}
}

