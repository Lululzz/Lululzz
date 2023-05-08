#最低版本要求
cmake_minimum_required(VERSION 3.10)
 
project(getcontours)
 
# 找包
find_package(OpenCV REQUIRED
            )

#添加可执行文件
add_executable(test opencv.cpp src/get_contours.cpp)

target_include_directories(test PUBLIC include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test ${OpenCV_LIBS})
