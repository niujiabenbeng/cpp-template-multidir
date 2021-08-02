#! /bin/bash

CURR=$(dirname $0)
TOOLS=${HOME}/Documents/tools
LIBRARY=$(realpath ${CURR}/../3rdparty)

link_3rdparty() {
    if [ ! -e ${TOOLS}/$1 ]; then
        echo "${TOOLS}/$1 does not found" && return 1
    fi
    if [ -e ${LIBRARY}/$1 ]; then
        echo "${LIBRARY}/$1 already exists." && return 1
    fi
    ln -s ${TOOLS}/$1 ${LIBRARY}/$1
}

mkdir -p ${LIBRARY}
link_3rdparty opencv
link_3rdparty jpeg-turbo
link_3rdparty mango
link_3rdparty aliyun
link_3rdparty date
