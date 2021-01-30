################################################################################
### Makefile template, based on the following blog:
###  http://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure
################################################################################

CC := clang++
CFLAGS := -g -Wall -fpic -O3 -std=c++14

INCDIR   := include
SRCDIR   := src
TOOLSDIR := tools
BUILDDIR := build
PROJECT  := public

# 这里添加第三方库
INCLUDE := ./3rdparty/boost/include
LIBRARY := ./3rdparty/boost/lib
LIBS    := dl m z rt glog gtest gflags curl crypto jsoncpp pthread \
           boost_system boost_filesystem

# makefile中, 函数名与参数之间用空格隔开, 多个参数之间用逗号隔开
INCLUDE := $(foreach INC,$(INCLUDE),-isystem $(INC))
LIBRARY := $(foreach LIB,$(LIBRARY),-L $(LIB))
LIBS    := $(foreach LIB,$(LIBS),-l$(LIB))
INCLUDE += -I $(INCDIR)

# $(SRCDIR)包含所有的库cpp
SRC_SRC := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJ_SRC := $(addprefix $(BUILDDIR)/,$(SRC_SRC:.cpp=.o))
TGT_SRC := $(BUILDDIR)/lib/lib$(PROJECT).so

# $(TOOLSDIR)包含所有的有main函数的cpp
SRC_TOOLS := $(shell find $(TOOLSDIR) -type f -name *.cpp)
OBJ_TOOLS := $(addprefix $(BUILDDIR)/,$(SRC_TOOLS:.cpp=.o))
TGT_TOOLS := $(addprefix $(BUILDDIR)/,$(SRC_TOOLS:.cpp=.bin))

# 提前建好所有与build相关的目录
ALL_BUILD_DIRS := $(sort $(dir $(OBJ_SRC) $(TGT_SRC) $(TGT_TOOLS)))
ALL_BUILD_DIRS := $(shell mkdir -p $(ALL_BUILD_DIRS))

lib: $(TGT_SRC)

tools: $(TGT_TOOLS)

all: $(TGT_SRC) $(TGT_TOOLS)

$(TGT_SRC): $(OBJ_SRC)
	$(CC) -shared -o $@ $^ $(LIBRARY) $(LIBS)

$(TGT_TOOLS): %.bin : %.o $(OBJ_SRC)
	$(CC) -o $@ $^ $(LIBRARY) $(LIBS)

$(OBJ_SRC) $(OBJ_TOOLS): $(BUILDDIR)/%.o : %.cpp
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

.PHONY: clean lib tools all
