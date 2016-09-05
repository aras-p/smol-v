CC=gcc
CXX=g++

INCLUDES=-Itesting/external/zstd -Itesting/external/zstd/common
CFLAGS=$(INCLUDES) -O2
CXXFLAGS=$(CFLAGS) -std=c++11

C_SRCS=testing/external/lz4/lz4.c \
testing/external/lz4/lz4hc.c \
testing/external/miniz/miniz.c \
testing/external/zstd/common/entropy_common.c \
testing/external/zstd/common/fse_decompress.c \
testing/external/zstd/common/xxhash.c \
testing/external/zstd/common/zstd_common.c \
testing/external/zstd/compress/fse_compress.c \
testing/external/zstd/compress/huf_compress.c \
testing/external/zstd/compress/zbuff_compress.c \
testing/external/zstd/compress/zstd_compress.c

CPP_SRCS=source/smolv.cpp \
testing/external/glslang/SPIRV/doc.cpp \
testing/external/glslang/SPIRV/SPVRemapper.cpp \
testing/testmain.cpp

C_OBJS=$(C_SRCS:.c=.o)
CPP_OBJS=$(CPP_SRCS:.cpp=.o)

all: smolv

clean:
	rm -f smolv $(C_OBJS) $(CPP_OBJS)

smolv: $(C_OBJS) $(CPP_OBJS)
	$(CXX) -o $@ $(C_OBJS) $(CPP_OBJS)
 
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

