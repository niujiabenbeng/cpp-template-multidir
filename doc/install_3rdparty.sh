#! /bin/bash

cd $(dirname $0)

THREADS=8
ROOT=${PWD}
CACHE=${ROOT}/.cache
TOOLS=${HOME}/Documents/tools
mkdir -p ${ROOT} ${CACHE} ${TOOLS}

download_file() {
    URL=$1
    MD5=$3
    FILE=${CACHE}/$2
    if [ ! -e $FILE ]; then
        wget -O ${FILE} ${URL}
    fi
    echo "$MD5 ${FILE}" | md5sum -c --status
    if [ $? -ne 0 ]; then
        echo "Failed to check md5sum: ${FILE}" && exit
    fi
}

install_opencv_from_source() {
    if [ -e ${TOOLS}/opencv ]; then
        echo "opencv has already been installed." && return 0
    fi

    URL="https://github.com/opencv/opencv/archive/3.4.2.zip"
    NAME="opencv-3.4.2.zip"
    MD5="9e9ebe9c1fe98c468f6e53f5c3c49716"
    download_file ${URL} ${NAME} ${MD5}

    TEMP=${NAME%.zip}
    cd ${CACHE} && unzip ${NAME} && cd ${TEMP}
    sudo apt install -y build-essential pkg-config libgtk2.0-dev
    sudo apt install -y libavcodec-dev libavformat-dev libswscale-dev
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${TOOLS}/opencv \
          -DBUILD_JAVA=OFF \
          -DBUILD_PACKAGE=OFF \
          -DBUILD_PERF_TESTS=OFF \
          -DBUILD_TESTS=OFF \
          -DBUILD_opencv_apps=OFF \
          -DBUILD_opencv_calib3d=OFF \
          -DBUILD_opencv_dnn=OFF \
          -DBUILD_opencv_java_bindings_generator=OFF \
          -DBUILD_opencv_ml=OFF \
          -DBUILD_opencv_objdetect=OFF \
          -DBUILD_opencv_photo=OFF \
          -DBUILD_opencv_python2=OFF \
          -DBUILD_opencv_python_bindings_generator=OFF \
          -DBUILD_opencv_shape=OFF \
          -DBUILD_opencv_stitching=OFF \
          -DBUILD_opencv_superres=OFF \
          -DBUILD_opencv_video=OFF \
          -DBUILD_opencv_videostab=OFF \
          -DOPENCV_DNN_OPENCL=OFF \
          -DWITH_CUFFT=OFF \
          ..
    make -j${THREADS} && make install
}

install_jpeg_turbo_from_source() {
    if [ -e ${TOOLS}/jpeg-turbo ]; then
        echo "jpeg-turbo has already been installed." && return 0
    fi

    URL="https://github.com/libjpeg-turbo/libjpeg-turbo/archive/2.0.0.tar.gz"
    NAME="libjpeg-turbo-2.0.0.tar.gz"
    MD5="e643c8cafdf5c40567fa11b2c0f4c20c"
    download_file ${URL} ${NAME} ${MD5}

    TEMP=${NAME%.tar.gz}
    cd ${CACHE} && tar zxvf ${NAME} && cd ${TEMP}
    sudo apt install -y nasm
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${TOOLS}/jpeg-turbo \
          ..
    make -j${THREADS} && make install
}

install_boost_from_source() {
    if [ -e ${TOOLS}/boost ]; then
        echo "boost has already been installed." && return 0
    fi

    URL="https://ayera.dl.sourceforge.net/project/boost/boost/1.59.0/boost_1_59_0.tar.gz"
    NAME="boost_1_59_0.tar.gz"
    MD5="51528a0e3b33d9e10aaa311d9eb451e3"
    download_file ${URL} ${NAME} ${MD5}

    TEMP=${NAME%.tar.gz}
    cd ${CACHE} && tar zxvf ${NAME} && cd ${TEMP}
    ./bootstrap.sh --prefix=${TOOLS}/boost
    ### only compile two modules: system and filesystem
    ./b2 cxxflags=-fPIC cflags=-fPIC --build-dir=build \
         --with-system --with-filesystem \
         -j${THREADS} variant=release install
}

install_mango_from_source() {
    if [ -e ${TOOLS}/mango ]; then
        echo "mango has already been installed." && return 0
    fi

    git clone https://chenli:Cl19871123@git.ainfinit.com/chenli/mango.git ${CACHE}/mango
    sudo apt install -y libglew-dev libsdl2-dev libsdl2-image-dev
    sudo apt install -y libglm-dev libfreetype6-dev

    cd ${CACHE}/mango && mkdir -p compile && cd compile
    cmake -DCMAKE_INSTALL_PREFIX=${TOOLS}/mango \
        -DMANGO_DISABLE_IMAGE_ZPNG=ON \
        -DMANGO_DISABLE_IMAGE_WEBP=ON \
        -DMANGO_DISABLE_IMAGE_TGA=ON \
        -DMANGO_DISABLE_IMAGE_SGI=ON \
        -DMANGO_DISABLE_IMAGE_PVR=ON \
        -DMANGO_DISABLE_IMAGE_PNM=ON \
        -DMANGO_DISABLE_IMAGE_PNG=ON \
        -DMANGO_DISABLE_IMAGE_PKM=ON \
        -DMANGO_DISABLE_IMAGE_PCX=ON \
        -DMANGO_DISABLE_IMAGE_KTX=ON \
        -DMANGO_DISABLE_IMAGE_IFF=ON \
        -DMANGO_DISABLE_IMAGE_HDR=ON \
        -DMANGO_DISABLE_IMAGE_GIF=ON \
        -DMANGO_DISABLE_IMAGE_DDS=ON \
        -DMANGO_DISABLE_IMAGE_C64=ON \
        -DMANGO_DISABLE_IMAGE_BMP=ON \
        -DMANGO_DISABLE_IMAGE_ATARI=ON \
        -DMANGO_DISABLE_IMAGE_ASTC=ON \
        -DMANGO_DISABLE_ARCHIVE_ZIP=ON \
        -DMANGO_DISABLE_ARCHIVE_RAR=ON \
        -DMANGO_DISABLE_ARCHIVE_MGX=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=ON \
        ../build
    make -j${THREADS} && make install
}

install_oss_from_source() {
    if [ -e ${TOOLS}/aliyun ]; then
        echo "oss has already been installed." && return 0
    fi

    URL="https://github.com/aliyun/aliyun-oss-cpp-sdk/archive/1.9.0.tar.gz"
    NAME="aliyun-oss-cpp-sdk-1.9.0.tar.gz"
    MD5="fad795d796a2dd22eb91b0eb76a68498"
    download_file ${URL} ${NAME} ${MD5}

    TEMP=${NAME%.tar.gz}
    cd ${CACHE} && tar zxvf ${NAME} && cd ${TEMP}
    sudo apt install -y libcurl4-openssl-dev libssl-dev

    mkdir -p build && cd build
    cmake -DBUILD_SHARED_LIBS=ON \
        -DCMAKE_INSTALL_PREFIX=${TOOLS}/aliyun \
        -DTARGET_OUTPUT_NAME_PREFIX=oss- ..
    make -j${THREADS} && make install
}

install_date_from_source() {
    if [ -e ${TOOLS}/date ]; then
        echo "date has already been installed." && return 0
    fi

    URL="https://github.com/HowardHinnant/date/archive/refs/tags/v3.0.0.tar.gz"
    NAME="date-3.0.0.tar.gz"
    MD5="c76681532f87644c59c19938961bc85c"
    download_file ${URL} ${NAME} ${MD5}

    TEMP=${NAME%.tar.gz}
    cd ${CACHE} && tar zxvf ${NAME} && cd ${TEMP}

    mkdir -p build && cd build
    cmake -DBUILD_TZ_LIB=ON -DCMAKE_INSTALL_PREFIX=${TOOLS}/date ..
    make -j${THREADS} && make install
}

install_gtest_from_source() {
    if [ -e /usr/local/lib/libgtest.a ]; then
       echo "gtest has already been installed." && return 0
    fi

    URL="https://github.com/google/googletest/archive/release-1.8.1.tar.gz"
    NAME="googletest-release-1.8.1.tar.gz"
    MD5="2e6fbeb6a91310a16efe181886c59596"
    download_file ${URL} ${NAME} ${MD5}

    TEMP=${NAME%.tar.gz}
    cd ${CACHE} && tar zxvf ${NAME} && cd ${TEMP}

    mkdir -p build && cd build
    cmake .. && make -j${THREADS} && sudo make install
}

################################################################################
################################################################################
################################################################################

sudo apt update
sudo apt install -y build-essential
sudo apt install -y automake autoconf
sudo apt install -y git wget curl cmake cmake-curses-gui

install_opencv_from_source
# install_boost_from_source
install_jpeg_turbo_from_source
install_mango_from_source
install_oss_from_source
install_date_from_source
# install_gtest_from_source
