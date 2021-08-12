# 多级目录工程Makefile总入口

SUBMODULE := public

# 这里可以选择用哪个编译器
export GG = clang++

lib:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE lib; done

all:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE all; done

tools:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE tools; done

tests:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE tests; done

clean:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE clean; done

.PHONY: lib all tools tests clean
