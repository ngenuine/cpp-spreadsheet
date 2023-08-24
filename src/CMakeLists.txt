cmake_minimum_required(VERSION 3.10)

project(MyExcel CXX)
set(CMAKE_CXX_STANDARD 17)

set(MY_EXCEL_FILES common.h main.cpp structures.cpp test_runner_p.h)

add_executable(myexcel ${MY_EXCEL_FILES})

# cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build . && ./myexcel