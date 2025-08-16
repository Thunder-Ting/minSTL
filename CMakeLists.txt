cmake_minimum_required(VERSION 3.31)
project(leistl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 声明 header-only 库
add_library(leistl INTERFACE)
target_include_directories(leistl
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # 本地构建用
        $<INSTALL_INTERFACE:include>                             # 安装后用
)

# 安装头文件和目标
install(TARGETS leistl EXPORT leistlTargets)
install(DIRECTORY include/ DESTINATION include)

# 配置 cmake package 导出
install(EXPORT leistlTargets
        FILE leistlTargets.cmake
        NAMESPACE leistl::
        DESTINATION lib/cmake/leistl
)

# 可执行文件或测试
add_executable(main main.cpp)
target_link_libraries(main PRIVATE leistl)

# GTest 测试
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(googletest)

add_executable(test_vector test/test_vector.cpp)
target_link_libraries(test_vector PRIVATE gtest_main leistl)
enable_testing()
add_test(NAME test_vector COMMAND test_vector)
