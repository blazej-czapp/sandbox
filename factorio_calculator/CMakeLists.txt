cmake_minimum_required(VERSION 3.22)

project(factorio_calculator)

set(CMAKE_CXX_STANDARD 17)

add_executable(factorio_calculator
        factorio_calculator.cpp)

# gtest setup after: https://stackoverflow.com/a/27160614/194825
##################################
# Download and install GoogleTest
include(ExternalProject)
ExternalProject_Add(googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(googletest source_dir binary_dir)

################
# Define a test
add_executable(factorio_test test.cpp)

set_property(TARGET factorio_test PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

######################################
# Configure the test to use GoogleTest
add_dependencies(factorio_test googletest)
include_directories(${source_dir}/googletest/include)
# link_directories(${binary_dir})
message(${binary_dir})
#target_link_libraries(test ${binary_dir}/libgtest.a)
find_library(GTEST_LIB gtest
                PATHS ${binary_dir}/lib/Debug)
target_link_libraries(factorio_test PUBLIC ${GTEST_LIB})

find_library(GTEST_MAIN_LIB gtest_main
                PATHS ${binary_dir}/lib/Debug)

target_link_libraries(factorio_test PUBLIC ${GTEST_MAIN_LIB})


##################################
# Just make the test runnable with
#   $ make test
enable_testing()
add_test(NAME    factorio_test
         COMMAND factorio_test)