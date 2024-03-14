# CMakeList.txt: module 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
set(EXECUTABLE_OUTPUT_PATH "/home/xingxing/bin")
aux_source_directory(flv "FLVDIR")
aux_source_directory(aac "AACDIR")
aux_source_directory(gbk "GBKDIR")
aux_source_directory(h264 "H264DIR")
add_executable (av_test "av_test.cpp" "av_test.h" ${FLVDIR} ${AACDIR} ${GBKDIR} ${H264DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET av_test PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
