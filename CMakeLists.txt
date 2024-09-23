cmake_minimum_required (VERSION 3.8)

project ("smol-v-test")

add_executable (smol-v-test)
target_compile_features(smol-v-test PRIVATE cxx_std_11)
target_compile_definitions(smol-v-test PRIVATE _CRT_SECURE_NO_WARNINGS)
target_sources(smol-v-test PRIVATE
	source/smolv.cpp
	source/smolv.h
	testing/testmain.cpp
	testing/external/glslang/SPIRV/doc.cpp
	testing/external/glslang/SPIRV/SPVRemapper.cpp
	testing/external/lz4/lz4.c
	testing/external/lz4/lz4hc.c
	testing/external/miniz/miniz.c
	testing/external/zstd/common/entropy_common.c
	testing/external/zstd/common/error_private.c
	testing/external/zstd/common/fse_decompress.c
	testing/external/zstd/common/xxhash.c
	testing/external/zstd/common/zstd_common.c
	testing/external/zstd/compress/fse_compress.c
	testing/external/zstd/compress/hist.c
	testing/external/zstd/compress/huf_compress.c
	testing/external/zstd/compress/zstd_compress.c
	testing/external/zstd/compress/zstd_compress_literals.c
	testing/external/zstd/compress/zstd_compress_sequences.c
	testing/external/zstd/compress/zstd_compress_superblock.c
	testing/external/zstd/compress/zstd_double_fast.c
	testing/external/zstd/compress/zstd_fast.c
	testing/external/zstd/compress/zstd_lazy.c
	testing/external/zstd/compress/zstd_ldm.c
	testing/external/zstd/compress/zstd_opt.c
)
target_include_directories(smol-v-test PRIVATE
	testing/external/zstd
	testing/external/zstd/common
)
