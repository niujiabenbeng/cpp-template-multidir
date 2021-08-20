#! /bin/bash

curr_dir=$(dirname $(realpath $0))
proj_dir=$(realpath ${curr_dir}/..)
format_tool=${curr_dir}/clang-format.py
lint_tool=${curr_dir}/clang-tidy.py

source_dirs="
${proj_dir}/public/include
${proj_dir}/public/src
${proj_dir}/public/tools
${proj_dir}/public/unittests
"

exclude_files="
"

get_all_source_files() {
    for dir_name in ${source_dirs}; do
        if [ ! -d ${dir_name} ]; then continue; fi
        for file_name in $(ls ${dir_name}); do
            file_name=${dir_name}/${file_name}
            file_name=$(realpath --relative-to=${proj_dir} ${file_name})
            if [[ ! ${exclude_files} == *${file_name}* ]]; then
                echo ${file_name}
            fi
        done
    done
}

if [ $# != 1 ]; then
    echo "usage: clang-tools.sh format|lint|clean" && exit 0
fi

if [ $1 == "format" ]; then
    cd ${proj_dir} && ${format_tool} $(get_all_source_files)
elif [ $1 == "lint" ]; then
    cd ${proj_dir} && ${lint_tool} $(get_all_source_files)
elif [ $1 == "clean" ]; then
    find ${proj_dir} -type f -name '*.4cf' -print -exec rm {} \;
    find ${proj_dir} -type f -name '*.4ct' -print -exec rm {} \;
else
    echo "Unknown task $1"
fi
