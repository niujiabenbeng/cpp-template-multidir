# 多级目录工程Makefile总入口

SUBMODULE := public

all:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE all; done

lib:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE lib; done

tools:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE tools; done

clean:
	for MODULE in $(SUBMODULE); do $(MAKE) -C $$MODULE clean; done

.PHONY: all lib tools clean
