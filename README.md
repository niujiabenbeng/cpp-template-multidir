## Linux下c++工程模板 (级联目录版本)

有时候, 为了实现模块化, 需要将代码放在不同的子目录中, 每一个子目录是一个小工程,
这些小工程共同组成了这个project. 这个模板实现多级目录的编译.


#### 文件安排

每一个子目录是一个小工程, 其目录结构与`cpp_template`一致. 每一个子目录有自己的
Makefile, 主目录的Makefile调用子目录的Makefile来完成编译任务.

为了简便起见, 不提供CMakeLists.txt, 如果需要支持cmake, 请参考`cpp_template`.


### Useful tools

1. 生成`compile_commands.json`: `bear make -ir -j4 all`

2. auto-formatting: `clang-format -i include/* src/* tools/* unittests/*`

3. linting: `run-clang-tidy.py`

注意: `run-clang-tidy.py`处理的是从`compile_commands.json`中的文件.
