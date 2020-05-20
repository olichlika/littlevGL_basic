cmake_minimum_required(VERSION 3.10)
project(Ui C)

#set(CMAKE_C_STANDARD 11)
SET(CROSS_COMPILE 1)
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_COMPILER "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/bin/arm-linux-gcc")
#set(CMAKE_C_COMPILER "/root/toolschain/4.4.3/bin/arm-linux-gcc")

#set(CMAKE_CXX_COMPILER "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/bin/arm-linux-gcc++")

SET(TOOLCHAIN_DIR "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/bin/")
#SET(TOOLCHAIN_DIR "/root/toolschain/4.4.3/bin/")

SET(CMAKE_FIND_ROOT_PATH  "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6" "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/lib" "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/include")
#SET(CMAKE_FIND_ROOT_PATH  "/root/toolschain/4.4.3/" "/root/toolschain/4.4.3/lib" "/root/toolschain/4.4.3/include")


#set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_C_FLAGS "-lpthread")

#include_directories(/root/jz2440_v3/busybox_install_3.4.5/include)
#link_directories(/root/jz2440_v3/busybox_install_3.4.5/lib)

file(GLOB_RECURSE INCLUDES "lv_drivers/*.h" "lv_examples/*.h"  "lvgl/*.h"  "./*.h" "tslib/*.h")
file(GLOB_RECURSE SOURCES  "lv_drivers/*.c" "lv_examples/*.c"  "lvgl/*.c" "tslib/*.c")

include_directories(.)
include_directories(tslib)
include_directories(lvgl)
include_directories(lvgl/lv_core)
include_directories(lvgl/lv_draw)
include_directories(lvgl/lv_hal)
include_directories(lvgl/lv_misc)
include_directories(lvgl/lv_misc/lv_fonts)
include_directories(lvgl/lv_objx)
include_directories(lvgl/lv_themes)

add_executable(Ui ${SOURCES} ${INCLUDES} main.c)

TARGET_LINK_LIBRARIES(Ui dl)