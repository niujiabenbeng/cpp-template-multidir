#! /bin/bash

curr_dir=$(dirname $0)
proj_dir=${curr_dir}/..
format_tool=${curr_dir}/clang-format.py
lint_tool=${curr_dir}/clang-tidy.py

source_files="
public/include/blocking_queue.h
public/include/common.h
public/include/thread_pool.h
public/include/timer.h
public/include/util.h
public/src/util.cpp
public/tools/main.cpp
public/unittests/unittest.cpp
"

if [ $# != 1 ]; then
    echo "usage: clang-tools.sh format|lint|clean" && exit 0
fi

if [ $1 == "format" ]; then
    cd ${proj_dir} && ${format_tool} ${source_files}
elif [ $1 == "lint" ]; then
    cd ${proj_dir} && ${lint_tool} ${source_files}
elif [ $1 == "clean" ]; then
    find ${proj_dir} -type f -name '*.4cf' -print -exec rm {} \;
    find ${proj_dir} -type f -name '*.4ct' -print -exec rm {} \;
else
    echo "Unknown task $1"
fi
