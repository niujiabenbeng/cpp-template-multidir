################################################################################
### Makefile template, based on the following blog:
###  http://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure
################################################################################

CC ?= clang++
CFLAGS := -g -Wall -fpic -O3 -std=c++17

INCDIR   := include
SRCDIR   := src
TOOLSDIR := tools
TESTSDIR := unittest
BUILDDIR := build
PROJECT  := public

# 这里添加第三方库
INCLUDE := ../3rdparty/boost/include \
           ../3rdparty/opencv/include \
           ../3rdparty/date/include
LIBRARY := ../3rdparty/boost/lib \
           ../3rdparty/opencv/lib \
           ../3rdparty/date/lib
LIBS    := dl m z rt glog gflags curl crypto jsoncpp pthread date-tz \
           opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc \
           boost_system boost_filesystem

# 这里添加不想被编译的文件, 文件路径相对于工程目录.
# 这里两个cpp文件并不存在, 只是用来做例子.
EXCLUDE := src/exclude.cpp tools/exclude.cpp

# makefile中, 函数名与参数之间用空格隔开, 多个参数之间用逗号隔开
INCLUDE := $(foreach INC,$(INCLUDE),-isystem $(INC))
LIBRARY := $(foreach LIB,$(LIBRARY),-L $(LIB))
LIBS    := $(foreach LIB,$(LIBS),-l$(LIB))
INCLUDE += -I $(INCDIR)

# $(SRCDIR)包含所有的库cpp
SRC_SRC := $(shell find $(SRCDIR) -type f -name *.cpp)
SRC_SRC := $(filter-out $(EXCLUDE),$(SRC_SRC))
OBJ_SRC := $(addprefix $(BUILDDIR)/,$(SRC_SRC:.cpp=.o))
TGT_SRC := $(BUILDDIR)/lib/lib$(PROJECT).so

# $(TOOLSDIR)包含所有的有main函数的cpp
SRC_TOOLS := $(shell find $(TOOLSDIR) -type f -name *.cpp)
SRC_TOOLS := $(filter-out $(EXCLUDE),$(SRC_TOOLS))
OBJ_TOOLS := $(addprefix $(BUILDDIR)/,$(SRC_TOOLS:.cpp=.o))
TGT_TOOLS := $(addprefix $(BUILDDIR)/,$(SRC_TOOLS:.cpp=.bin))

# $(TESTSDIR)包含所有的单元测试的cpp
# 将unittest和tools分开是因为unittest通常会用到gtest, 而tools并不会
SRC_TESTS := $(shell find $(TESTSDIR) -type f -name *.cpp)
SRC_TESTS := $(filter-out $(EXCLUDE),$(SRC_TESTS))
OBJ_TESTS := $(addprefix $(BUILDDIR)/,$(SRC_TESTS:.cpp=.o))
TGT_TESTS := $(addprefix $(BUILDDIR)/,$(SRC_TESTS:.cpp=.bin))

# 提前建好所有与build相关的目录
BUILD_DIRS := $(sort $(dir $(OBJ_SRC) $(TGT_SRC) $(TGT_TOOLS) $(TGT_TESTS)))
BUILD_DIRS := $(shell mkdir -p $(BUILD_DIRS))

lib: $(TGT_SRC)

tools: $(TGT_TOOLS)

tests: $(TGT_TESTS)

all: $(TGT_SRC) $(TGT_TOOLS)

$(TGT_SRC): $(OBJ_SRC)
	$(CC) -shared -o $@ $^ $(LIBRARY) $(LIBS)

$(TGT_TOOLS): %.bin : %.o $(OBJ_SRC)
	$(CC) -o $@ $^ $(LIBRARY) $(LIBS)

$(TGT_TESTS): %.bin : %.o $(OBJ_SRC)
	$(CC) -o $@ $^ $(LIBRARY) $(LIBS) -lgtest

$(OBJ_SRC) $(OBJ_TOOLS) $(OBJ_TESTS): $(BUILDDIR)/%.o : %.cpp
	$(CC) $(CFLAGS) -MP -MMD -c -o $@ $< $(INCLUDE)

ifneq ($(filter clean, $(MAKECMDGOALS)), clean)
    # 这里'-'表示忽略错误.
    # 缺省状态下只编译lib, 这里把tools的头文件依赖也包含进去虽然略显多余,
    # 但是由于.d文件只包含依赖, 并没有实际的编译指令, 所以并不耗费时间.
    -include $(OBJ_SRC:.o=.d)
    -include $(OBJ_TOOLS:.o=.d)
endif

clean:
	rm -rf $(BUILDDIR)

.PHONY: clean lib tools tests all
